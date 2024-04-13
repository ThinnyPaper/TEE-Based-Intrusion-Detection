#include "secure_storage.h"
#include <stdint.h>
#include <string.h>

#include "db.h"

//存储结构：索引表+db_line*n
//一个哈希表对应完整路径到偏移量
//开放寻址双重散列解决冲突


static uint32_t hash_index(const char* str) {
    //djb2
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}
static uint32_t hash_step(const char *str) {
    uint32_t hash = 0;
    int c;
    while ((c = *str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

void insert_to_hash_table(const char* path, uint32_t offset) {
    uint32_t index = hash_index(path) % TABLE_SIZE;
    uint32_t step = hash_step(path) % TABLE_SIZE;

    if (step == 0) step = 1;

   while (hash_table[index].offset!=0) {
        index = (index + step) % TABLE_SIZE;
    }
    hash_table[index] = data;
}

static TEE_Result store_db_line(uint32_t param_types, TEE_Param params[4]) {
    TEE_Result res;
    TEE_ObjectHandle object = TEE_HANDLE_NULL;
    uint32_t obj_data_flag = TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_CREATE;

    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                               TEE_PARAM_TYPE_NONE,
                                               TEE_PARAM_TYPE_NONE,
                                               TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    db_line *data = (db_line *)params[0].memref.buffer;
    size_t data_len = sizeof(db_line);

    // 使用fullpath作为对象ID
    res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
                                     data->fullpath, strlen(data->fullpath),
                                     obj_data_flag, TEE_HANDLE_NULL,
                                     data, data_len, &object);
    if (res != TEE_SUCCESS) {
        return res;
    }

    TEE_CloseObject(object);
    return TEE_SUCCESS;
}
