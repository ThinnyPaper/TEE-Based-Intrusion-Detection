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

#include "idtt.h"
#include <tee_client_api.h>
#include <unistd.h>
#include <getopt.h> 
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "db_line.h"
#include "readconf.h"
#include "genfile.h"
#include "util.h"
#include "idtt_ta_command.h"
#include "idtt_ta.h"

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif
#define CONFIG_FILE "/root/idtt.conf"

idtt_config* conf;
char* filename;
static double alltime;
static void usage(int exitvalue){
  fprintf(stdout,
	"Usage: idtt cammand [args]\n\n"
    "Cammands:\n"
    "  -i\t--init\t\tInitialize the secure storage\n"
    "  -a\t--checkall\t\tCheck all file compare to the initial information"
    "  -c [file]\t--check=[file]\t\tCheck [file] compare to the initial information\n"
    "  -h\t--help\t\tPrint this usage"
    "\n"
	  );
  
  exit(exitvalue);
}

static void read_param(int argc,char**argv){
    int op = -1;
    int i=0;

    static struct option options[] =
    {
    { "help", no_argument, NULL, 'h' },
    { "init", no_argument, NULL, 'i'},
    { "checkall", no_argument, NULL, 'a'},
    { "check", required_argument, NULL, 'c'},
    { NULL,0,NULL,0 }
    };
    
    while((op=getopt_long(argc, argv, "hiac:", options, &i))!=-1){
        switch(op){
            case 'h':{
                usage(0);
                break;
            }
            case 'i':{
                if(conf->action==0){ 
                    conf->action=DO_INIT; 
                    printf("command: init database\n"); 
                    break;
                } else { 
                    exit(-1); 
                } 
            }
            case 'a':{
                if(conf->action==0){ 
                    conf->action=DO_CHECKALL; 
                    printf("command: check all files\n"); 
                    break;
                } else { 
                    exit(-1); 
                } 
            }
            case 'c':{
                if(conf->action==0){ 
                    conf->action=DO_CHECK; 
                    filename=checked_malloc(sizeof(optarg));
                    strncpy(filename,optarg,sizeof(optarg)-1);
                    printf("command: check file %s\n", optarg); 
                    break;
                } else { 
                    exit(-1); 
                } 
            }
        }
    }

}

static void setdefaults_idtt_config(){
    //set idtt_config defalt;
    conf->hostname=NULL;
    conf->filelist=NULL;
    conf->config_file=
#ifdef CONFIG_FILE
        CONFIG_FILE
#else
        NULL
#endif
;
    conf->action=NO_ACTION;

}

static TEEC_Result do_check_one(TEEC_Session *sess, char* filepath){
    //time
    clock_t start, end;
    double cpu_time;
    start=clock();

    printf("Checking %s\n",filepath);
    db_line *line=gen_file_to_db_line(filepath);
    //to TEE
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t err_origin;
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_OUTPUT, TEEC_NONE);
    op.params[0].tmpref.buffer = filepath;
    op.params[0].tmpref.size = strlen(filepath)+1;
    op.params[1].tmpref.buffer = line;
    op.params[1].tmpref.size = sizeof(db_line);

    res=TEEC_InvokeCommand(sess, TA_CMD_CHECK, &op, &err_origin);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand TA_CMD_CHECK ERROR\n");
        return res;
    }
    uint32_t check_result=op.params[2].value.a;
    if(check_result==TA_CHECK_RESULT_CONSISTENT){
        printf("TA_CHECK_RESULT_CONSISTENT\n");
    }else if(check_result==TA_CHECK_RESULT_MODIFIED){
        printf("TA_CHECK_RESULT_MODIFIED\n");
        uint32_t check_flag=op.params[2].value.b;
        if ((check_flag&CHECK_PERM_INCONSIS)>0) printf("perm differs\n");
        if ((check_flag&CHECK_UID_INCONSIS)>0) printf("uid differs\n");
        if ((check_flag&CHECK_GID_INCONSIS)>0) printf("gid differs\n");
        if ((check_flag&CHECK_ATIME_INCONSIS)>0) printf("atime differs\n");
        if ((check_flag&CHECK_CTIME_INCONSIS)>0) printf("ctime differs\n");
        if ((check_flag&CHECK_MTIME_INCONSIS)>0) printf("mtime differs\n");
        if ((check_flag&CHECK_INODE_INCONSIS)>0) printf("inode differs\n");
        if ((check_flag&CHECK_NLINK_INCONSIS)>0) printf("nlink differs\n");
        if ((check_flag&CHECK_SIZE_INCONSIS)>0) printf("size differs\n");
        if ((check_flag&CHECK_BCOUNT_INCONSIS)>0) printf("bcount differs\n");
        if ((check_flag&CHECK_SHA256_INCONSIS)>0) printf("hash_sha256 differs\n");
        if ((check_flag&CHECK_WHIRLPOOL_INCONSIS)>0) printf("hash_whirlpool differs\n");

    }else if(TA_CHECK_RESULT_NO_MATCH_FILE){
        printf("TA_CHECK_RESULT_NO_MATCH_FILE\n");
    }else{
        printf("GET BAD check_result\n");
        return TEEC_ERROR_GENERIC;
    }

    end=clock();
    cpu_time=((double)(end-start))/CLOCKS_PER_SEC;
    alltime+=cpu_time;
    printf("Time of checking one file is %f seconds.\n", alltime);

    return TEEC_SUCCESS;
}

int main(int argc,char**argv){
    //umask(0177); //仅本用户可读写
    //check_result=TA_CHECK_RESULT_NO_MATCH_FILE;

    conf=(idtt_config*)checked_malloc(sizeof(idtt_config));
    setdefaults_idtt_config();

    read_param(argc, argv);


    //get hostname 
    conf->hostname = checked_malloc(sizeof(char) * MAXHOSTNAMELEN + 1);
    if (gethostname(conf->hostname,MAXHOSTNAMELEN) == -1) {
        printf("gethostname failed");
        free(conf->hostname);
        conf->hostname = NULL;
    } else {
        printf( "hostname: '%s'", conf->hostname);
    }

    //读config文件
    printf("read configure file: %s\n", conf->config_file);
    readconf(conf->config_file, conf);
    /*
    node* it=conf->filelist;
    while(it!=NULL){
        printf("%s\n", it->data);
        it=it->next;
    }
    */
    if(conf->action==NO_ACTION){
        printf("NO ACTION\n");
        exit(1);
    }else if (conf->action!=DO_INIT && conf->action!=DO_CHECK && conf->action!=DO_CHECKALL){
        printf("BAD ACTION\n");
        exit(1);
    }

    //INITAIL TEE
    TEEC_Result res;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_UUID uuid = IDTT_TA_UUID; // TODO:您的TA的UUID
    uint32_t err_origin;

    // 初始化与TEE的连接
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext ERROR\n");
        exit(1);
    }

    // 打开会话
    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    //TEEC_LOGIN_USER可以定义用户登录
    if (res != TEEC_SUCCESS) {
        printf("TEEC_OpenSession ERROR\n");
        exit(1);
    }

    if(conf->action==DO_INIT){
        //TEE初始化一个DB
        res=TEEC_InvokeCommand(&sess, TA_CMD_INITDB, NULL, &err_origin);
        if (res != TEEC_SUCCESS) {
            printf("TEEC_InvokeCommand TA_CMD_INITDB ERROR\n");
            exit(1);
        }

        //存储每一个db_line
        node* it=conf->filelist;
        while(it!=NULL){
            db_line *line=gen_file_to_db_line(it->data);
           // line->
            //to TEE
            TEEC_Operation op;
            memset(&op, 0, sizeof(op));
            op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE);
            op.params[0].tmpref.buffer = it->data;
            op.params[0].tmpref.size = strlen(it->data)+1;
            op.params[1].tmpref.buffer = line;
            op.params[1].tmpref.size = sizeof(db_line);

            res=TEEC_InvokeCommand(&sess, TA_CMD_STORE, &op, &err_origin);
            if (res != TEEC_SUCCESS) {
                printf("TEEC_InvokeCommand TA_CMD_STORE ERROR\n");
            }
            it=it->next;
        }

    }else if(conf->action==DO_CHECK){
        if(filename==NULL){
            printf("No file path arg\n");
            exit(1);
        }else{
            //full path filename
            char* fullfilepath=checked_malloc(PATH_MAX);

            realpath(filename, fullfilepath);
            free(filename);
            filename=fullfilepath;
            res=do_check_one(&sess, filename);
            if(res!=TEEC_SUCCESS){
                printf("Something went wrong while checking %s\n",filename);
                return 1;
            }
        }
    }else if(conf->action==DO_CHECKALL){
        node* it=conf->filelist;
        while(it!=NULL){
            res=do_check_one(&sess, it->data);
            if(res!=TEEC_SUCCESS){
                printf("Something went wrong while checking %s\n",filename);
            }
            it=it->next;
        }
    }else{
        printf("BAD ACTION\n");
        exit(1);
    }



    // Clean up TEE
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);

    return 0;

}
// vi: ts=8 sw=8
