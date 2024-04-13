#ifndef SECURE_STORAGE_H_INCLUDE
#define SECURE_STORAGE_H_INCLUDE

#include <tee_internal_api.h>

#include "config.h"

#define RETOK 0
#define RETFAIL -1

typedef struct {
    uint32_t hash_index;
    uint32_t filepath_offset;   
    uint32_t db_line_offset;
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
TEE_Result init_db_obj();






#endif