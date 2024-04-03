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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <tee_client_api.h>
#include "errorcodes.h"
#include "log.h"
#include "db_config.h"


#if HAVE_UNISTD_H
#include <unistd.h>
#endif

db_config* conf;


static void usage(int exitvalue)
{
  fprintf(stdout,
	  _("Usage: idtt [options] command\n\n"
	    "Commands:\n"
	    "  -i, --init\t\tInitialize the database\n"
	    "  -n, --dry-init\tTraverse the file system and match each file against rule tree\n"
	    "  -C, --check\t\tCheck the database\n"
	    "  -u, --update\t\tCheck and update the database non-interactively\n"
	    "  -E, --compare\t\tCompare two databases\n\n"
	    "Miscellaneous:\n"
	    "  -D,\t\t\t--config-check\t\t\tTest the configuration file\n"
	    "  -p file_type:path\t--path-check=file_type:path\tMatch file type and path against rule tree\n"
	    "  -h,\t\t\t--help\t\t\t\tShow this help message\n\n"
	    "Options:\n"
	    "  -c [cfgfile]\t--config=[cfgfile]\tGet config options from [cfgfile]\n"
	    "  -l [REGEX]\t--limit=[REGEX]\t\tLimit command to entries matching [REGEX]\n"
	    "  -B \"OPTION\"\t--before=\"OPTION\"\tBefore configuration file is read define OPTION\n"
	    "  -A \"OPTION\"\t--after=\"OPTION\"\tAfter configuration file is read define OPTION\n"
	    "  -L [level]\t--log-level=[level]\tSet log message level to [level]\n"
	    "\n")
	  );
  
  exit(exitvalue);
}

#define ACTION_CASE(longopt, option, _action, desc) \
      case option: { \
            if(conf->action==0){ \
                conf->action=_action; \
                log_msg(LOG_LEVEL_INFO,"(%s): %s command", longopt, desc); \
            } else { \
                INVALID_ARGUMENT(longopt, %s, "cannot have multiple commands on a single commandline") \
                exit(INVALID_ARGUMENT_ERROR); \
            } \
            break; \
        }
static void read_param(int argc,char**argv)
{
  int option = -1;
  int i=0;
  

  static struct option options[] =
  {
    { "help", no_argument, NULL, 'h' },
    { "config", required_argument, NULL, 'c'},
    { "before", required_argument, NULL, 'B'},
    { "after", required_argument, NULL, 'A'},
    { "report", no_argument, NULL, 'r'},
    { "init", no_argument, NULL, 'i'},
    { "dry-init", no_argument, NULL, 'n'},
    { "check", no_argument, NULL, 'C'},
    { "update", no_argument, NULL, 'u'},
    { "config-check", no_argument, NULL, 'D'},
    { "path-check", required_argument, NULL, 'p'},
    { "limit", required_argument, NULL, 'l'},
    { "log-level", required_argument, NULL, 'L'},
    { "compare", no_argument, NULL, 'E'},
    { NULL,0,NULL,0 }
  };

  while(1){
    option = getopt_long(argc, argv, "hL:V::vc:l:p:B:A:riCuDEn", options, &i);
    if(option==-1)
      break;
    switch(option)
      {
      case 'h':{
        usage(0);
        break;
      }
      case 'c':{
        conf->config_file=optarg;
        log_msg(LOG_LEVEL_INFO,_("(--config): set config file to '%s'"), conf->config_file);
        break;
      }
      case 'B': {
        before = append_line_to_config(before, optarg);
        log_msg(LOG_LEVEL_INFO,_("(--before): append '%s' to before config"), optarg);
	break;
      }
      case 'A': {
        after = append_line_to_config(after, optarg);
        log_msg(LOG_LEVEL_INFO,_("(--after): append '%s' to after config"), optarg);
	break;
      }
      case 'l': {
                const char* pcre_error;
                int pcre_erroffset;
                conf->limit=checked_malloc(strlen(optarg)+1);
                strcpy(conf->limit,optarg);
                if((conf->limit_crx=pcre_compile(conf->limit, PCRE_ANCHORED, &pcre_error, &pcre_erroffset, NULL)) == NULL) {
                    INVALID_ARGUMENT("--limit", error in regular expression '%s' at %i: %s, conf->limit, pcre_erroffset, pcre_error)
                }
                log_msg(LOG_LEVEL_INFO,_("(--limit): set limit to '%s'"), conf->limit);
            break;
      }
      case 'L':{
            LOG_LEVEL level = get_log_level_from_string(optarg);
            if (level == LOG_LEVEL_UNSET) {
                INVALID_ARGUMENT("--log-level", invalid log level '%s' (see man aide.conf for details), optarg)
            } else {
                set_log_level(level);
                log_msg(LOG_LEVEL_INFO,"(--log-level): set log level to '%s'", optarg);
            }
           break;
               }
      case 'p':{
            if(conf->action==0){
                conf->action=DO_DRY_RUN;
                log_msg(LOG_LEVEL_INFO,"(--path-check): path check command");

                if (strlen(optarg) >= 3 && optarg[1] == ':') {
                    RESTRICTION_TYPE file_type = get_restriction_from_char(*optarg);
                    if (file_type == FT_NULL) {
                        INVALID_ARGUMENT("--path-check", invalid file type '%c' (see man aide for details), *optarg)
                    } else {
                        conf->check_file_type = file_type;
                        if (optarg[2] != '/') {
                            INVALID_ARGUMENT("--path-check", '%s' needs to be an absolute path, optarg+2)
                        } else {
                            conf->check_path = checked_strdup(optarg+2);
                            log_msg(LOG_LEVEL_INFO,"(--path-check): set path to '%s' (filetype: %c)", optarg+2, get_restriction_char(conf->check_file_type));
                        }
                    }
                } else {
                    INVALID_ARGUMENT("--path-check", %s, "missing file type or path (see man aide for details)")
                }

            } else {
                INVALID_ARGUMENT("--path-check", %s, "cannot have multiple commands on a single commandline")
                exit(INVALID_ARGUMENT_ERROR);
            }
            break;
      }
      case 'r': {
       INVALID_ARGUMENT("--report", %s, "option no longer supported, use 'report_url' config option instead (see man aide.conf for detail)")
       break;
      }
      ACTION_CASE("--init", 'i', DO_INIT, "database init")
      ACTION_CASE("--dry-init", 'n', DO_INIT|DO_DRY_RUN, "dry init")
      ACTION_CASE("--check", 'C', DO_COMPARE, "database check")
      ACTION_CASE("--update", 'u', DO_INIT|DO_COMPARE, "database update")
      ACTION_CASE("--compare", 'E', DO_DIFF, "database compare")
      ACTION_CASE("--config-check", 'D', DO_DRY_RUN, "config check")
      default: 
	      exit(INVALID_ARGUMENT_ERROR);
      }
  }

  if(optind<argc){
    fprintf(stderr, "%s: extra parameter: '%s'\n", argv[0], argv[optind]);
    exit(INVALID_ARGUMENT_ERROR);
  }
}

int main(int argc,char**argv)
{
  int errorno=0;


  umask(0177); //近本用户可读写
  //init_sighandler();

  //setdefaults_before_config();

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

  log_msg(LOG_LEVEL_INFO, "parse configuration");
  errorno=parse_config(before, conf->config_file, after);
  if (errorno==RETFAIL){
    exit(INVALID_CONFIGURELINE_ERROR);
  }
  free (before);
  free (after);

  //setdefaults_after_config();

  log_msg(LOG_LEVEL_CONFIG, "report_urls:");
  log_report_urls(LOG_LEVEL_CONFIG);

  log_msg(LOG_LEVEL_RULE, "rule tree:");
  log_tree(LOG_LEVEL_RULE, conf->tree, 0);

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

  /* Let's do some sanity checks for the config */
  if (conf->action&(DO_DIFF|DO_COMPARE) && !(conf->database_in.url)) {
    log_msg(LOG_LEVEL_ERROR,_("missing 'database_in', config option is required"));
    exit(INVALID_ARGUMENT_ERROR);
  }
  if (conf->action&DO_INIT && !(conf->database_out.url)) {
    log_msg(LOG_LEVEL_ERROR,_("missing 'database_out', config option is required"));
    exit(INVALID_ARGUMENT_ERROR);
  }
  if(conf->database_in.url && conf->database_out.url && cmpurl(conf->database_in.url,conf->database_out.url)==RETOK){
      log_msg(LOG_LEVEL_NOTICE, "input and output database URLs are the same: '%s'", (conf->database_in.url)->value);
    if((conf->action&DO_INIT)&&(conf->action&DO_COMPARE)){
      log_msg(LOG_LEVEL_ERROR,_("input and output database urls cannot be the same "
	    "when doing database update"));
      exit(INVALID_ARGUMENT_ERROR);
    }
    if(conf->action&DO_DIFF){
      log_msg(LOG_LEVEL_ERROR,_("both input databases cannot be the same "
		"when doing database compare"));
      exit(INVALID_ARGUMENT_ERROR);
    }
  };
  if((conf->action&DO_DIFF)&&(!(conf->database_new.url)||!(conf->database_in.url))){
    log_msg(LOG_LEVEL_ERROR,_("must have both input databases defined for "
	      "database compare"));
    exit(INVALID_ARGUMENT_ERROR);
  }

  if (conf->action&DO_INIT && conf->action&DO_DRY_RUN) {
      if(db_disk_init()==RETFAIL) {
          exit(IO_ERROR);
      }
      log_msg(LOG_LEVEL_INFO, "populate tree (dry-run)");
      populate_tree(conf->tree, true);
      exit (0);
  }

  if (!(conf->action&DO_DRY_RUN)) {

  if (!init_report_urls()) {
      exit(INVALID_CONFIGURELINE_ERROR);
  }

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
	log_msg(LOG_LEVEL_ERROR,_("Error while writing database. Exiting.."));
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
      
    log_msg(LOG_LEVEL_INFO, "populate tree");
    populate_tree(conf->tree, false);

    if(conf->action&DO_INIT) {
        log_msg(LOG_LEVEL_INFO, "write new entries to database: %s:%s", get_url_type_string((conf->database_out.url)->type), (conf->database_out.url)->value);
        write_tree(conf->tree);
    }

    db_close();

    log_msg(LOG_LEVEL_INFO, "generate reports");

    int exitcode = gen_report(conf->tree);

    log_msg(LOG_LEVEL_INFO, "exit AIDE with exit code '%d'", exitcode);

    exit(exitcode);
  }
  return RETOK;
}
