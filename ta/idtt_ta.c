#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "ta_command.h"
#include "db_line.h"
#include "secure_storage.h"

bool it_loaded=false; //index_table
bool db_opened=false;
bool index_changed=false;
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
   
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	if(index_changed){
		store_index();
	}
	close_index_obj();
	close_db_obj();

	IMSG("Session has been closed\n");
}


TEE_Result TA_InvokeCommandEntryPoint(void *session, uint32_t command_id, uint32_t param_types, TEE_Param params[4])
{	
	TEE_Result ret;

    switch (command_id) {
	case TA_CMD_INITDB: {
		if(check_db_exist()){
			IMSG("Database exist. Can not init.\n");
			return TEE_ERROR_GENERIC;
		}
		ret=init_db_obj();
		if(ret==TEE_SUCCESS){
			it_loaded=true; 
			db_opened=true;
			
		}
		return ret;
		break;
		}
    case TA_CMD_STORE: {
		if(!it_loaded){
			uint32_t index_load_flag = TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_OVERWRITE;
			if(load_index(index_load_flag)==true){
				it_loaded=true;
			}else{
				IMSG("Load index failed.\n");
				return TEE_ERROR_ACCESS_DENIED;
			}		
		}
		if(!db_opened){
			uint32_t db_open_flag = TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_READ;
			if(open_db(db_open_flag)==true){
				db_opened=true;
			}else{
				IMSG("Open database failed.\n");
				return TEE_ERROR_ACCESS_DENIED;
			}
		}
		ret=store_db_line(param_types, params);	
		if(ret==TEE_SUCCESS){
			index_changed=true;	
		}
		return ret;
		break;
	}
	case TA_CMD_CHECK: {
		if(!it_loaded){
			uint32_t index_load_flag = TEE_DATA_FLAG_ACCESS_READ;
			if(load_index(index_load_flag)==true){
				it_loaded=true;
			}else{
				IMSG("Load index failed.\n");
				return TEE_ERROR_ACCESS_DENIED;
			}
		}
		if(!db_opened){
			uint32_t db_open_flag = TEE_DATA_FLAG_ACCESS_READ;
			if(open_db(db_open_flag)==true){
				db_opened=true;
			}else{
				IMSG("Open database failed.\n");
				return TEE_ERROR_ACCESS_DENIED;
			}
		}		
		ret=check_file(param_types, params);	
		//传递结果
		return ret;
		break;
	}
    default:{
        return TEE_ERROR_NOT_SUPPORTED;
    }
	}
	//not suppose to get here
	return TEE_ERROR_GENERIC;
}