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

#include <sys/stat.h>
#include <gcrypt.h>

//#include <mhash.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "db_line.h"
#include "util.h"
#include <time.h>
#define BLOCK_SIZE 1024
static double SHAtime;
/*/
  //char* filename;
  char fullpath[PATH_MAX];

  //byte* hashsums[num_hashes];//计算所有
  mode_t perm; //权限
  long uid; //文件所有者的用户ID
  long gid; //文件所属组的组ID
  time_t atime; //文件的最后访问时间
  time_t ctime; //文件状态最后更改的时间
  time_t mtime; //文件内容最后修改的时间
  long inode; //文件的inode编号
  long nlink; //链接到该文件的硬链接数
  long long size; //文件大小
*/
db_line* gen_file_to_db_line(char* file){
    if(file==NULL){
        printf("empty file name\n");
        return NULL;
    }
    if (access(file, R_OK) != 0) {
        perror("File is not accessible");
        return NULL;
    }
    struct stat fileInfo;
    if (stat(file, &fileInfo) < 0) {
        printf("Error getting file information\n");
        return NULL;
    }
    db_line* ret=checked_malloc(sizeof(db_line));

    
    ret->perm = (unsigned int)fileInfo.st_mode;
    ret->uid = (unsigned int)fileInfo.st_uid;      
    ret->gid = (unsigned int)fileInfo.st_gid;       
    //ret->atime = (long long)fileInfo.st_atime;        
    ret->ctime = (long long)fileInfo.st_ctime;
    ret->mtime = (long long)fileInfo.st_mtime;        
    ret->inode = (unsigned long long)fileInfo.st_ino;   
    ret->nlink = (unsigned int)fileInfo.st_nlink;   
    ret->size = (long long)fileInfo.st_size;     
    ret->bcount = (long long)fileInfo.st_blocks;   

    clock_t start, end;
    double cpu_time;
    start=clock();
    //gcrypt
    gcry_md_hd_t sha256_handle;
    //gcry_md_hd_t whirlpool_handle;
    unsigned char buffer[BLOCK_SIZE];
    size_t read_bytes;

    // 打开文件
    FILE* fp = fopen(file, "rb");
    if (!fp) {
        printf("Failed to open file\n");
        return NULL;
    }

    // 初始化 SHA-256
    if (gcry_md_open(&sha256_handle, GCRY_MD_SHA256, 0)) {
        fprintf(stderr, "Failed to initialize SHA-256 hash\n");
        fclose(fp);
        return NULL;
    }

    // 读取文件并更新哈希
    while ((read_bytes = fread(buffer, 1, BLOCK_SIZE, fp)) > 0) {
        gcry_md_write(sha256_handle, buffer, read_bytes);
       // gcry_md_write(whirlpool_handle, buffer, read_bytes);
    }

    // 检查是否因为除了EOF之外的错误而停止读取
    if (ferror(fp)) {
        printf("Error reading from file");
        gcry_md_close(sha256_handle);
        //gcry_md_close(whirlpool_handle);
        fclose(fp);
        return NULL;
    }

    // 获取最终的哈希值
    memcpy(ret->hash_sha256, gcry_md_read(sha256_handle, GCRY_MD_SHA256), 32);
   // memcpy(ret->hash_whirlpool, gcry_md_read(whirlpool_handle, GCRY_MD_WHIRLPOOL), 64);

    // 清理
    gcry_md_close(sha256_handle);
    //gcry_md_close(whirlpool_handle);
    fclose(fp);
    end=clock();
    cpu_time=((double)(end-start))/CLOCKS_PER_SEC;
    SHAtime+=cpu_time;
    printf("Time of culculate SHA-256 is %f seconds.\n", SHAtime);
    return ret;
}