#ifndef _SECURE_STORAGE_H
#define _SECURE_STORAGE_H

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include "config.h"

#define RETOK 0
#define RETFAIL -1

typedef struct {
    uint32_t hash_index;
    uint32_t filepath_offset;   
    uint32_t db_line_offset;
    size_t pathlen;
} HashIndexEntry;

#define TABLE_SIZE MAX_FILE_NUMEBER

typedef struct {
    unsigned int cnt_file=0;
    HashIndexEntry hash_table[TABLE_SIZE];
}IndexTable;

#define INDEX_TABLE_ID "idtt_index_table"
#define DB_ID "idtt_database"


extern IndexTable index_table;
extern TEE_ObjectHandle *index_object;
extern TEE_ObjectHandle *db_object;

bool check_db_exist();
bool open_db(uint32_t flag);
bool load_index(uint32_t flag);
bool store_index();
void close_index_obj();
void close_db_obj();
TEE_Result init_db_obj();
TEE_Result store_db_line(uint32_t param_types, TEE_Param params[4]);
TEE_Result check_file(uint32_t param_types, TEE_Param params[4]);

#endif