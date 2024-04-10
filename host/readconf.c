
#include "readconf.h"

#include <string.h>
#include <pcre.h>
#include "idtt.h"
#include "log.h"
#include "conf_lex.h"

#include "conf_yacc.h"

#include "db.h"
static char* getconfig(char* name)
{

	static char info[64];
	int find=0;
	char tmp[256],fore[64],back[64],tmpcwd[MAXLINE];
	char *fs,*fe,*equal,*bs,*be,*start;

	strcpy(tmpcwd,cwd);
	strcat(tmpcwd,"/");
	FILE *fp=getfp(strcat(tmpcwd,"config.ini"));
	while(fgets(tmp,255,fp)!=NULL)
	{
		start=tmp;
		equal=strchr(tmp,'=');

		while(isblank(*start))
			++start;
		fs=start;

		if(*fs=='#')
			continue;
		while(isalpha(*start))
			++start;
		fe=start-1;

		strncpy(fore,fs,fe-fs+1);
		fore[fe-fs+1]='\0';
		if(strcmp(fore,name)!=0)
			continue;
		find=1;

		start=equal+1;
		while(isblank(*start))
			++start;
		bs=start;

		while(!isblank(*start)&&*start!='\n')
			++start;
		be=start-1;

		strncpy(back,bs,be-bs+1);
		back[be-bs+1]='\0';
		strcpy(info,back);
		break;
	}
	if(find)
		return info;
	else
		return NULL;
}


int read_file_list(char *config){
    //use lex to get a file path
    //new lsit
    filelist.tail->
}

int parse_config(char *config) {
    if(config==NULL||strcmp(config,"")==0){
      log_msg(LOG_LEVEL_ERROR,("missing configuration file"));
      return RETFAIL;
    }

    ast* config_ast = NULL;

    conf_lex_file(config);
    if(confparse(&config_ast)){
      return RETFAIL;
    }
    conf_lex_delete_buffer();
    eval_config(config_ast, 0);
    deep_free(config_ast);
    config_ast = NULL;
    
  return RETOK;
}