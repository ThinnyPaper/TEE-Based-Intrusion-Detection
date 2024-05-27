/*
 * IDTT (Intrusion Detection Tool Based on TEE)
 *
 * Copyright (C) 2024 Yang Zhe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _SECURE_STORAGE_H
#define _SECURE_STORAGE_H

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#define MAX_FILE_NUMEBER 20
typedef struct {
    uint32_t hash_index;
    uint32_t filepath_offset;   
    uint32_t db_line_offset;
    size_t pathlen;
} HashIndexEntry;

#define TABLE_SIZE MAX_FILE_NUMEBER

typedef struct {
    unsigned int cnt_file;
    HashIndexEntry hash_table[TABLE_SIZE];
}IndexTable;

#define INDEX_TABLE_ID "idtt_index_table"
#define DB_ID "idtt_database"


extern IndexTable index_table;
extern TEE_ObjectHandle *index_object;
extern TEE_ObjectHandle *db_object;

bool check_db_exist(void);
void clean_all_obj(void);
bool open_db(uint32_t flag);
bool load_index(uint32_t flag);
bool store_index(void);
void close_index_obj(void);
void close_db_obj(void);
TEE_Result init_db_obj(void);
TEE_Result store_db_line(uint32_t param_types, TEE_Param params[4]);
TEE_Result check_file(uint32_t param_types, TEE_Param params[4]);

#endif