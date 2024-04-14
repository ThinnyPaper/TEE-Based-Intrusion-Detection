/*
 * TIDE (Intrusion Detection Tool Based on TEE)
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
 * Portions of this program are based on AIDE(Advanced Intrusion
 * Detection Environment) by Rami Lehti, Pablo Virolainen, 
 * Mike Markley, Richard van den Berg, Hannes von Haugwitz 
 * (Copyright (C) 1999-2006, 2010-2013, 2015-2017, 2019-2021). 
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

#include "log.h"
#include "db.h"
#include "readconf.h"
#include "util.h"
#include "ta_command.h"

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif
idtt_config* conf;
char* filename;
uint32_t check_result;

static void usage(int exitvalue){
  fprintf(stdout,
	"Usage: idtt cammand [args]\n\n"
    "Cammands:\n"
    "  -i\t--init\t\tInitialize the database\n"
    "  -a\t--checkall\t\tCheck all file compare to the database"
    "  -c [file]\t--check=[file]\t\tCheck [file] compare to the database\n"
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
                    log_msg(LOG_LEVEL_INFO,"command: init database"); 
                    break;
                } else { 
                    exit(-1); 
                } 
            }
            case 'a':{
                if(conf->action==0){ 
                    conf->action=DO_CHECKALL; 
                    log_msg(LOG_LEVEL_INFO,"command: check all files"); 
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
                    log_msg(LOG_LEVEL_INFO,"command: check file %s", optarg); 
                    break;
                } else { 
                    exit(-1); 
                } 
            }
        }
    }

}

static void setdefaults_idtt_config(){
    //set db_config defalt;
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

int main(int argc,char**argv){
    int errorno=0;
    umask(0177); //仅本用户可读写
    check_result=0;

    conf=(db_config*)checked_malloc(sizeof(db_config));
    read_param(argc, argv);

    setdefaults_idtt_config();

    //get hostname 
    conf->hostname = checked_malloc(sizeof(char) * MAXHOSTNAMELEN + 1);
    if (gethostname(conf->hostname,MAXHOSTNAMELEN) == -1) {
        log_msg(LOG_LEVEL_WARNING,"gethostname failed: %s", strerror(errorno));
        free(conf->hostname);
        conf->hostname = NULL;
    } else {
        log_msg(LOG_LEVEL_DEBUG, "hostname: '%s'", conf->hostname);
    }

    //读config文件
    log_msg(LOG_LEVEL_INFO, "read configure file: %s", conf->config_file);
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
    }else (conf->action!=DO_INIT && conf->action!=DO_CHECK){
        printf("BAD ACTION\n");
        exit(1);
    }

    //INITAIL TEE
    TEEC_Result res;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_UUID uuid = TA_UUID; // 您的TA的UUID
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
            printf("TEEC_InvokeCommand TA_CMD_INITDB ERROR\n")
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
                printf("TEEC_InvokeCommand TA_CMD_STORE ERROR\n")
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
            //printf("fullpath: %s\n",fullfilepath);
            //gen filename to db_line;
            db_line *line=gen_file_to_db_line(filename);
            //to TEE
            TEEC_Operation op;
            memset(&op, 0, sizeof(op));
            op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_OUTPUT, TEEC_NONE);
            op.params[0].tmpref.buffer = filename;
            op.params[0].tmpref.size = strlen(filename)+1;
            op.params[1].tmpref.buffer = line;
            op.params[1].tmpref.size = sizeof(db_line);

            res=TEEC_InvokeCommand(&sess, TA_CMD_STORE, &op, &err_origin);
            if (res != TEEC_SUCCESS) {
                printf("TEEC_InvokeCommand TA_CMD_STORE ERROR\n")
            }
            uint32_t check_result=op.params[2].value.a;
            if(check_result==TA_CHECK_RESULT_CONSISTENT){
                printf("TA_CHECK_RESULT_CONSISTENT\n");
            }else if(check_result==TA_CHECK_RESULT_MODIFIED){
                printf("TA_CHECK_RESULT_MODIFIED\n");
            }else if(TA_CHECK_RESULT_NO_MATCH_FILE){
                printf("TA_CHECK_RESULT_NO_MATCH_FILE\n");
            }

        }
    }else if(conf->action==DO_CHECKALL){
        node* it=conf->filelist;
        while(it!=NULL){
            //gen it to db_line;
               

            db_line* line=gen_file_to_db_line(it->data);
            printf("SHA-256 Hash: ");

            for (int i = 0; i < 32; i++) {
                printf("%02x", line->hash_sha256[i]);
            }
            printf("\n");
            //to TEE


            it=it->next;
        }
    }else{
        printf("BAD ACTION\n");
        exit(1);
    }



    // Clean up TEE
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    
    return check_result;

}
// vi: ts=8 sw=8
