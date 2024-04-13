


#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "ta_command.h"
#include "db.h"
#include "secure_storage.h"

bool it_open=false; //index_table
bool db_open=false;
bool change_index=false;
IndexTable index_table={0};
TEE_ObjectHandle *index_object=NULL;
TEE_ObjectHandle *db_object=NULL;

TEE_Result TA_CreateEntryPoint(void)
{
	//DMSG("has been called");
	return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void)
{
	//DMSG("has been called");
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	IMSG("Session has been opened\n");
	(void)&param_types;
    (void)&params;
    (void)&sess_ctx;
	//打开object
	/*

	TEE_Result res;

    // 尝试打开指定 ID 的持久对象
    res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, DB_ID, strlen(DB_ID), TEE_DATA_FLAG_ACCESS_READ, &object_handle);
	//只读

    if (res == TEE_SUCCESS) {
        db_open=true;
    }else if (res == TEE_ERROR_ITEM_NOT_FOUND) {
		DMSG("No database exist\n");
    }
*/
    
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	IMSG("Session has been closed\n");
}


TEE_Result TA_InvokeCommandEntryPoint(void *session, uint32_t command_id, uint32_t param_types, TEE_Param params[4])
{
    switch (command_id) {
	case TA_CMD_INITDB: {
		if(check_db_exist()){
			IMSG("Database exist. Can not init.\n");
			return TEE_ERROR_GENERIC;
		}
		return init_db_obj();
		break;
	}
    case TA_CMD_STORE: {
		change_index=true;
		if(!db_open){
			if(open_db()==RETOK){
				db_open=true;
			}else{
				IMSG("Open database failed.\n");
				return TEE_ERROR_ACCESS_DENIED
			}
		}
		return store_db_line();
		//看有没有存在的db
		//创建objext
		//indextable占位
        //store_db_line(uint32_t param_types, TEE_Param params[4])
		//索引更新
		
		break;
	}
	case TA_CMD_CHECK: {
		return check_db;
		//看有没有存在的db
		//load index_talbe
		//check one
		break;
	}
    default:{
        return TEE_ERROR_NOT_SUPPORTED;
    }
	}
}