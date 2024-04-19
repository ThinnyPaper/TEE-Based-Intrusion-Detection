#include "secure_storage.h"
#include <stdint.h>
#include <string.h>

#include "db_line.h"
#include "idtt_ta_command.h"

//存储结构：索引表+db_line*n
//一个哈希表对应完整路径到偏移量
//开放寻址双重散列解决冲突
bool check_db_exist(){
    TEE_Result res;
    TEE_ObjectHandle object_handle = TEE_HANDLE_NULL;
    // 尝试打开指定 ID 的持久对象
    res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
                                   DB_ID, strlen(DB_ID),
                                   TEE_DATA_FLAG_ACCESS_READ, &object_handle);
    if (res == TEE_SUCCESS) {
        // 对象存在，关闭对象句柄
        TEE_CloseObject(object_handle);
        return true;
    }
    return false;
}
TEE_Result init_db_obj(){
    //index_table
    TEE_Result res;
    uint32_t flag = TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_OVERWRITE;
    //TODO: index obj需不需要打开？直接存就行？
    res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE, INDEX_TABLE_ID, strlen(INDEX_TABLE_ID)+1, flag, TEE_HANDLE_NULL, NULL, 0, index_object);
    if(res!=TEE_SUCCESS) return TEE_ERROR_GENERIC;
    res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE, DB_ID, strlen(DB_ID)+1, flag, TEE_HANDLE_NULL, NULL, 0, db_object);
    if(res!=TEE_SUCCESS) return TEE_ERROR_GENERIC;
    return TEE_SUCCESS;
}

bool open_db(uint32_t flag){
    TEE_Result res;
    res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, DB_ID, strlen(DB_ID)+1, flag, *db_object);
	if (res != TEE_SUCCESS) {
		EMSG("Failed to open db object, res=0x%08x", res);
		return false;
	}
    return true;
}

bool load_index(uint32_t flag){
    TEE_Result res;
    res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, INDEX_TABLE_ID, strlen(INDEX_TABLE_ID)+1, flag, *index_object);
	if (res != TEE_SUCCESS) {
		EMSG("Failed to open index table object, res=0x%08x", res);
		return false;
	}

    uint32_t count=0;
    res = TEE_ReadObjectData(index_object, &index_table, sizeof(index_table),
				 &count);

	if (res != TEE_SUCCESS || count != sizeof(index_table)) {
		EMSG("TEE_ReadObjectData failed 0x%08x, read %" PRIu32 " over %u",
				res, count, sizeof(index_table));
        return false;
	}
    return true;    
}

bool store_index(){
    TEE_Result res;
    // 打开对象
    res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, INDEX_TABLE_ID, strlen(INDEX_TABLE_ID)+1,
                                   TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_OVERWRITE,
                                   index_object);
    if (res = TEE_ERROR_ITEM_NOT_FOUND) {
        TEE_Result create_res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE, INDEX_TABLE_ID, strlen(INDEX_TABLE_ID)+1, TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_OVERWRITE, TEE_HANDLE_NULL, NULL, 0, index_object);
        if(create_res!=TEE_SUCCESS) {
            EMSG("TEE_CreatePersistentObject failed, res=0x%08x", res);
            return false;
        }
    }else if(res!=TEE_SUCCESS){
        EMSG("TEE_OpenPersistentObject failed, res=0x%08x", res);
        return false;
    }

    // 清空对象内容
    res = TEE_TruncateObjectData(index_object, 0);
    if (res != TEE_SUCCESS) {
        EMSG("TEE_TruncateObjectData failed, res=0x%08x", res);
        TEE_CloseObject(*index_object);
        return false;
    }

    // 写入新数据
    res = TEE_WriteObjectData(*index_object, &index_table, sizeof(index_table));
    if (res != TEE_SUCCESS) {
        TEE_CloseObject(*index_object);
        EMSG("TEE_WriteObjectData failed, res=0x%08x", res);
        return false;
    }
    return true;
}


void close_index_obj(){
    TEE_CloseObject(*index_object);
}
void close_db_obj(){
    TEE_CloseObject(*db_object);
}

static uint32_t get_hash_index(const char* str) {
    //djb2
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}
static uint32_t get_hash_step(const char *str) {
    uint32_t hash = 0;
    int c;
    while ((c = *str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}


static uint32_t insert_to_hash_table(const char* path) {
    uint32_t hash = get_hash_index(path);
    uint32_t index = hash % TABLE_SIZE;
    uint32_t step = get_hash_step(path) % TABLE_SIZE;

    if (step == 0) step = 1;
    
    while (index_table.hash_table[index].pathlen!=0) {
        //TODO 检查是否重复
        index = (index + step) % TABLE_SIZE;
    }
    index_table.hash_table[index].hash_index=hash;
    index_table.hash_table[index].pathlen=strlen(path)+1;
    return index;
}

static char* get_filepath_by_offset(uint32_t offset, size_t pathlen){
    TEE_Result res;
    uint32_t read_bytes;
    char *path=TEE_Malloc(pathlen, 0);
    res = TEE_ReadObjectData(*db_object, path, pathlen, &read_bytes);

	if (res != TEE_SUCCESS || read_bytes != pathlen) {
		EMSG("TEE_ReadObjectData failed 0x%08x, read %" PRIu32 " over %u", res, read_bytes, pathlen);
        TEE_Free(path);
		return NULL;
	}
    return path;

}

static uint32_t get_db_index(const char* path) {
    //不存在返回TABLE_SIZE+1；
    uint32_t hash = get_hash_index(path);
    uint32_t index = hash % TABLE_SIZE;
    uint32_t step = get_hash_step(path) % TABLE_SIZE;
    uint32_t original_index = index;  // 保存原始索引以检测循环

    if (step == 0) step = 1;
    
    while (index_table.hash_table[index].pathlen!=0) {
        if(index_table.hash_table[index].hash_index!=hash){
            continue;
        }
        char* stored_path = get_filepath_by_offset(index_table.hash_table[index].filepath_offset, index_table.hash_table[index].pathlen);
        if (strcmp(stored_path, path) == 0) {
            TEE_Free(stored_path);
            return index;  // 找到匹配，返回当前索引
        }

        index = (index + step) % TABLE_SIZE;
        if (index == original_index) {
            break;
        }
        TEE_Free(stored_path);
    }
   
    return UINT32_MAX;  // 使用UINT32_MAX表示未找到
}

TEE_Result store_db_line(uint32_t param_types, TEE_Param params[4]) {
    if(index_table.cnt_file>=TABLE_SIZE){
        DMSG("File numbers exceeds limit\n");
        return TEE_ERROR_EXCESS_DATA;
    }

    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
    if (param_types != exp_param_types){
        return TEE_ERROR_BAD_PARAMETERS;
    }
    char* filepath=(char*)params[0].memref.buffer;
	size_t filepath_sz=params[0].memref.size;
    db_line* line=(db_line*)params[1].memref.buffer;
    size_t line_sz=params[1].memref.size;

    //检查是否已存在
    if(get_db_index(filepath)!=UINT32_MAX){
        EMSG("File has allready exist in db.\n");
        return TEE_ERROR_GENERIC;
    }
    //建立索引
    uint32_t index= insert_to_hash_table(filepath);

    //写filepath
    TEE_ObjectInfo info;
    TEE_Result res = TEE_GetObjectInfo1(*db_object, &info);
    if (res != TEE_SUCCESS) {
        IMSG("TEE_GetObjectInfo1 failed, object might not been open.\n");
        return res;
    }
    uint32_t filepath_offset = info.dataSize;
    res = TEE_WriteObjectData(*db_object, filepath, filepath_sz);
    if (res != TEE_SUCCESS) {
        IMSG("TEE_WriteObjectData failed.\n");
        return res;
    }
    
    //写db_line 
    res = TEE_GetObjectInfo1(*db_object, &info);
    if (res != TEE_SUCCESS) {
        IMSG("TEE_GetObjectInfo1 failed, object might not been open.\n");
        return res;
    }
    uint32_t line_offset = info.dataSize;
    res = TEE_WriteObjectData(*db_object, line, line_sz);
    if (res != TEE_SUCCESS) {
        IMSG("TEE_WriteObjectData failed.\n");
        return res;
    }

    //更新索引表offset
    index_table.hash_table[index].filepath_offset=filepath_offset;
    index_table.hash_table[index].db_line_offset=line_offset;
    
    index_table.cnt_file++;

    return TEE_SUCCESS;
}

TEE_Result check_file(uint32_t param_types, TEE_Param params[4]) {
    if(index_table.cnt_file==0){
        DMSG("No file checksum stored yet.\n");
        return TEE_ERROR_GENERIC;
    }

    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_VALUE_OUTPUT, TEE_PARAM_TYPE_NONE);
    if (param_types != exp_param_types){
        return TEE_ERROR_BAD_PARAMETERS;
    }
    char* filepath=(char*)params[0].memref.buffer;
	size_t filepath_sz=params[0].memref.size;
    db_line* line=(db_line*)params[1].memref.buffer;
    size_t line_sz=params[1].memref.size;
    //查索引
    uint32_t index=get_db_index(filepath);
    uint32_t line_offset=index_table.hash_table[index].db_line_offset;
    if(index==UINT32_MAX){
        params[2].value.a=TA_CHECK_RESULT_NO_MATCH_FILE;
        return TEE_SUCCESS;
    }
    //读出存储的db_line
    db_line *stored_line=TEE_Malloc(sizeof(db_line),0);

    TEE_Result res;
    uint32_t read_bytes;
    res = TEE_ReadObjectData(*db_object, stored_line, sizeof(db_line), &read_bytes);
	if (res != TEE_SUCCESS || read_bytes !=sizeof(db_line)) {
		EMSG("TEE_ReadObjectData failed 0x%08x, read %" PRIu32 " over %u", res, read_bytes, sizeof(db_line));
        TEE_Free(stored_line);
		return TEE_ERROR_GENERIC;
	}

    uint32_t check_flag=0;
    if (line->perm != stored_line->perm) check_flag|=CHECK_PERM_INCONSIS;
    if (line->uid != stored_line->uid) check_flag|=CHECK_UID_INCONSIS;
    if (line->gid != stored_line->gid) check_flag|=CHECK_GID_INCONSIS;
    if (line->atime != stored_line->atime) check_flag|=CHECK_ATIME_INCONSIS;
    if (line->ctime != stored_line->ctime) check_flag|=CHECK_CTIME_INCONSIS;
    if (line->mtime != stored_line->mtime) check_flag|=CHECK_MTIME_INCONSIS;
    if (line->inode != stored_line->inode) check_flag|=CHECK_INODE_INCONSIS;
    if (line->nlink != stored_line->nlink) check_flag|=CHECK_NLINK_INCONSIS;
    if (line->size != stored_line->size) check_flag|=CHECK_SIZE_INCONSIS;
    if (line->bcount != stored_line->bcount) check_flag|=CHECK_BCOUNT_INCONSIS;
    if (memcmp(line->hash_sha256, stored_line->hash_sha256, 32) != 0) check_flag|=CHECK_SHA256_INCONSIS;
    if (memcmp(line->hash_whirlpool, stored_line->hash_whirlpool, 64) != 0) check_flag|=CHECK_WHIRLPOOL_INCONSIS;

    TEE_Free(stored_line);
    
    if(check_flag==0){
        params[2].value.a=TA_CHECK_RESULT_CONSISTENT;
    }else{
        params[2].value.a=TA_CHECK_RESULT_MODIFIED;
        params[2].value.b=check_flag;
    }
    return TEE_SUCCESS;

}

    
    





