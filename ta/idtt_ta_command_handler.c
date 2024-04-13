


#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "ta_command.h"
#include "db.h"


/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
	//DMSG("has been called");

	return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
	//DMSG("has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	(void)&param_types;
    (void)&params;
    (void)&sess_ctx;

    DMSG("Session has been opened\n");
    return TEE_SUCCESS;

	/*
	 * The DMSG() macro is non-standard, TEE Internal API doesn't
	 * specify any means to logging from a TA.
	 */
	//IMSG("Hello World!\n");

	/* If return value != TEE_SUCCESS the session will not be created. */
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	IMSG("Session has been closed\n");
}


TEE_Result TA_InvokeCommandEntryPoint(void *session, uint32_t command_id, uint32_t param_types, TEE_Param params[4])
{
    switch (command_id) {
    case CMD_STORTE_DB: {
        //store_db_line(uint32_t param_types, TEE_Param params[4])
		
		break;
	}
	case CMD_CHECK_DB; {

		break;
	}


    default:
        return TEE_ERROR_NOT_SUPPORTED;
    }
}