#include <sys/stat.h>
#include <mhash.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "db.h"
#define BLOCK_SIZE 1024
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

    strncpy(ret->fullpath, file, sizeof(file));
    ret->perm = (unsigned int)fileInfo.st_mode;
    ret->uid = (unsigned int)fileInfo.st_uid;      
    ret->gid = (unsigned int)fileInfo.st_gid;       
    ret->atime = (long long)fileInfo.st_atime;        
    ret->ctime = (long long)fileInfo.st_ctime;
    ret->mtime = (long long)fileInfo.st_mtime;        
    ret->inode = (unsigned long long)fileInfo.st_ino;   
    ret->nlink = (unsigned int)fileInfo.st_nlink;   
    ret->size = (long long)fileInfo.st_size;     
    ret->bcount = (long long)fileInfo.st_blocks;   
    /*  
    printf("mode%d\n",ret->perm);
    printf("uid%d\n",ret->uid);
    printf("gid%d\n",ret->gid);
    printf("atime%d\n",(ret->atime));
    printf("ctime%d\n",ret->ctime);
    printf("mtime%d\n",(ret->mtime));
    printf("inode%d\n",(ret->inode));
    printf("nlink%d\n",(ret->nlink));
    printf("size%d\n",(ret->size));
    */


    //mhash
    //TODO 优化读取
    MHASH td_sha256, td_whirlpool;
    unsigned char buffer[BLOCK_SIZE];
    unsigned char *hash_sha256, *hash_whirlpool;
    size_t bytesRead;
    // 打开文件
    FILE* fp = fopen(file, "rb");
    if (!fp) {
        printf("Failed to open file\n");
        return NULL;
    }

    // 初始化mhash计算为 SHA-256 和 WHIRLPOOL
    td_sha256 = mhash_init(MHASH_SHA256);
    td_whirlpool = mhash_init(MHASH_WHIRLPOOL);
    if (td_sha256 == MHASH_FAILED || td_whirlpool == MHASH_FAILED) {
        fclose(fp);
        printf("hash failed\n");
        return NULL;
    }
    
    // 读取文件并计算哈希
    while ((bytesRead = fread(buffer, 1, BLOCK_SIZE, fp)) > 0) {
        mhash(td_sha256, buffer, bytesRead);
        mhash(td_whirlpool, buffer, bytesRead);
    }
    
    // 获取最终的哈希值
    hash_sha256 = (unsigned char *)mhash_end(td_sha256);
    hash_whirlpool = (unsigned char *)mhash_end(td_whirlpool);
    memcpy(ret->hash_sha256, hash_sha256, 32); 
    ret->sha256=hash_sha256;
    free(hash_sha256); 
    memcpy(ret->hash_whirlpool, hash_whirlpool, 64); 
    free(hash_whirlpool); 

    fclose(fp);
    
    return ret;
}