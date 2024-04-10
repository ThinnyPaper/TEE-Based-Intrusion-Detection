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

#include "log.h"

db_config* conf;

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
    int option = -1;
    int i=0;

    static struct option options[] =
    {
    { "help", no_argument, NULL, 'h' },
    { "init", no_argument, NULL, 'i'},
    { "checkall", no_argument, NULL, 'a'},
    { "check", required_argument, NULL, 'c'},
    { NULL,0,NULL,0 }
    };

    while(option=getopt_long(argc, argv, "hiac:", options, &i)!=-1){
        switch(option){
            case: 'h':{
                usage(0);
                break;
            }
            case: 'i':{
                if(conf->action==0){ 
                    conf->action=DO_INIT; 
                    log_msg(LOG_LEVEL_INFO,"command: init database"); 
                } else { 
                    INVALID_ARGUMENT(longopt, %s, "cannot have multiple commands on a single commandline") 
                    exit(INVALID_ARGUMENT_ERROR); 
                } 
            }
            case: 'a':{
                if(conf->action==0){ 
                    conf->action=DO_CHECKALL; 
                    log_msg(LOG_LEVEL_INFO,"command: check all files"); 
                } else { 
                    INVALID_ARGUMENT(longopt, %s, "cannot have multiple commands on a single commandline") 
                    exit(INVALID_ARGUMENT_ERROR); 
                } 
            }
            case: 'c':{
                if(conf->action==0){ 
                    conf->action=DO_CHECK; 
                    log_msg(LOG_LEVEL_INFO,"command: check file %s", optarg); 
                } else { 
                    INVALID_ARGUMENT(longopt, %s, "cannot have multiple commands on a single commandline") 
                    exit(INVALID_ARGUMENT_ERROR); 
                } 
            }
        }
    }

}

static void setdefaults_before_config(){
    //set db_config defalt;
    conf->config_file=
#ifdef CONFIG_FILE
        CONFIG_FILE
#else
        NULL
#endif


}

int main(int argc,char**argv){
    int errorno=0;
    umask(0177); //仅本用户可读写

    conf=(db_config*)checked_malloc(sizeof(db_config));
    
    read_param(argc, argv);

    setdefaults_before_config();

    //读config文件
    log_msg(LOG_LEVEL_INFO, "read configure file: %s", conf->config_file);
    errorno=parse_config(conf->config_file);
    if (errorno==RETFAIL){
        exit(INVALID_CONFIGURELINE_ERROR);
    }

    readconfig


  log_msg(LOG_LEVEL_INFO, "read command line parameters");
  read_param(argc,argv);

  /* get hostname */
  conf->hostname = checked_malloc(sizeof(char) * MAXHOSTNAMELEN + 1);
  if (gethostname(conf->hostname,MAXHOSTNAMELEN) == -1) {
      log_msg(LOG_LEVEL_WARNING,"gethostname failed: %s", strerror(errno));
      free(conf->hostname);
      conf->hostname = NULL;
  } else {
      log_msg(LOG_LEVEL_DEBUG, "hostname: '%s'", conf->hostname);
  }




  log_msg(LOG_LEVEL_CONFIG, "report_urls:");
  log_report_urls(LOG_LEVEL_CONFIG); //what？

  log_msg(LOG_LEVEL_RULE, "rule tree:");
  log_tree(LOG_LEVEL_RULE, conf->tree, 0);

    //我们要不要检查模型路径可访问性？
    /*
  if (conf->check_path) {
      rx_rule* rule = NULL;
      int match = check_rxtree(conf->check_path, conf->tree, &rule, conf->check_file_type, true);
      if (match < 0) {
        fprintf(stdout, "[ ] %c '%s': outside of limit '%s'\n", get_restriction_char(conf->check_file_type), conf->check_path, conf->limit);
        exit(2);
      } else {
        exit(match?0:1);
      }
  }
  */


// tp



// init

// check -a
// 逐个检查config中的文件

// check -f file 
// 发一个node过去 


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
}
// vi: ts=8 sw=8
