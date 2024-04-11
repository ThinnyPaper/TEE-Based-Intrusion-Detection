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
//#include <tee_client_api.h>
#include <unistd.h>
#include <getopt.h> 
#include <limits.h>

#include "log.h"
#include "db.h"
#include "readconf.h"
#include "util.h"

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif
db_config* conf;
char* filename;

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

static void setdefaults_before_config(){
    //set db_config defalt;
    conf->filelist=NULL;
    conf->config_file=
#ifdef CONFIG_FILE
        CONFIG_FILE
#else
        NULL
#endif
;

}

int main(int argc,char**argv){
    int errorno=0;
    umask(0177); //仅本用户可读写

    conf=(db_config*)checked_malloc(sizeof(db_config));
    read_param(argc, argv);

    setdefaults_before_config();

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
    node* it=conf->filelist;
    while(it!=NULL){

        printf("%s\n", it->data);
        it=it->next;
    }

    if(conf->action==DO_INIT){
        node* it=conf->filelist;
        while(it!=NULL){
            gen_file_to_db_line(it->data);
           // line->
            //to TEE

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
            gen_file_to_db_line(filename);
            //to TEE
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
        printf("NO ACTION\n");
        exit(1);
    }






/*


    if (!init_report_urls()) {
        exit(INVALID_CONFIGURELINE_ERROR);
    }
    //试试prefix前缀能不能打开
    if (conf->action&(DO_INIT|DO_COMPARE) && conf->root_prefix_length > 0) {
        DIR *dir;
        if((dir = opendir(conf->root_prefix)) != NULL) {
            closedir(dir);
        } else {
            log_msg(LOG_LEVEL_ERROR,"opendir() for root_prefix %s failed: %s", conf->root_prefix, strerror(errno));
            exit(INVALID_CONFIGURELINE_ERROR);
        }
    }
    if(conf->action&DO_INIT){
      if(db_init(&(conf->database_out), false,
#ifdef WITH_ZLIB
        conf->gzip_dbout
#else
        false
#endif
       ) == RETFAIL) {
	exit(IO_ERROR);
      }
      if(db_writespec(conf)==RETFAIL){
	log_msg(LOG_LEVEL_ERROR,("Error while writing database. Exiting.."));
	exit(IO_ERROR);
      }
    }
    if((conf->action&DO_INIT)||(conf->action&DO_COMPARE)){
      if(db_disk_init()==RETFAIL)
	exit(IO_ERROR);
    }
    if((conf->action&DO_COMPARE)||(conf->action&DO_DIFF)){
      if(db_init(&(conf->database_in), true, false)==RETFAIL)
	exit(IO_ERROR);
    }
    if(conf->action&DO_DIFF){
      if(db_init(&(conf->database_new), true, false)==RETFAIL)
	exit(IO_ERROR);
    }
      
    //log_msg(LOG_LEVEL_INFO, "populate tree");
    //populate_tree(conf->tree, false);

    if(conf->action&DO_INIT) {
        log_msg(LOG_LEVEL_INFO, "write new entries to database: %s:%s", get_url_type_string((conf->database_out.url)->type), (conf->database_out.url)->value);
        write_tree(conf->tree);
        
    }

    db_close();

    log_msg(LOG_LEVEL_INFO, "generate reports");

    int exitcode = gen_report(conf->tree);

    log_msg(LOG_LEVEL_INFO, "exit AIDE with exit code '%d'", exitcode);

    exit(exitcode);
  
  return RETOK;
  */
}
// vi: ts=8 sw=8
