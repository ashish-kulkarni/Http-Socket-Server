#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include "csuc_http.h"
#define MAX_FILE_SIZE 8192000

extern int total_buffer_size;
extern char log_message[PATH_MAX];

/* Getting file's contents */

int filecontent(FILE *newfp,http_response_t *response,int newfd)
{
	strcpy(log_message,"Checking for file contents");
	SigUsr2Logger(3,log_message);

	FILE *file; int fd;
	int f_size;
	char *buf = malloc((sizeof(char *))*(MAX_FILE_SIZE));
	memset(buf,0, MAX_FILE_SIZE);
	if(access(response->resource_path,F_OK)==0)
	{
		file = fopen(response->resource_path, "r");
		if (file)
		{
			while((f_size= fread(buf,1,MAX_FILE_SIZE,file)) > 0)
				fwrite(buf,1,f_size,newfp);

			//		close(fd);	
		}
		else
		{
			strcpy(log_message,"Opening file in file contents");
			SigUsr2Logger(0,log_message);
		}
		fclose(file);
	}
	else
	{
		strcpy(log_message,"Warning :favicon.ico detected.");
		SigUsr2Logger(1,log_message);
	}
	free(buf);

	return 0;
}


