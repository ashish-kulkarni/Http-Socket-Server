#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include "csuc_http.h"
#include "counters.h"

extern int logcounter;
extern char log_message[PATH_MAX];

int error_level(char *error_string)
{

	if((strcmp(error_string,"0")==0) || (strcmp(error_string,"ERROR")==0))
	{
		logcounter =0;
	}
	else if((strcmp(error_string,"1")==0) || (strcmp(error_string,"WARNING")==0))
	{
		logcounter = 1;
	}
	else if((strcmp(error_string,"2")==0) || (strcmp(error_string,"INFO")==0))
	{
		logcounter = 2;
	}
	else if((strcmp(error_string,"3")==0) || (strcmp(error_string,"DEBUG")==0))
	{
		logcounter = 3;
	}
	else
	{
		strcpy(log_message,"Error_level : Invalid Option");
		SigUsr2Logger(0,log_message);
		exit(EXIT_FAILURE);
	}
	return 0;
}

int port_validation(int portno)
{
	if(portno > 1024 && portno <65535)
		return 0;
	else
	{
		strcpy(log_message,"Invalid Port Number");
		SigUsr2Logger(0,log_message);
		exit(EXIT_FAILURE);
	}
}


int directory_validation(http_response_t *response)

{
	struct dirent *dirpent;
	DIR *dirp;
	dirp= opendir(count.dir_string);
	if(dirp)
	{
		strcpy(response->resource_path,count.dir_string);
		return 1;
	}
	else
	{
		strcpy(log_message,"Directory does not Exist");
		SigUsr2Logger(0,log_message);
		exit(EXIT_FAILURE);
	}
	int closedir(DIR *dirp); //Closing Directory
}
