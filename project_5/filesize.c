#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include "csuc_http.h"

extern int total_buffer_size;
extern char log_message[PATH_MAX];

/*Getting File Size*/
int filesize(http_response_t *response)
{
	strcpy(log_message,"Checking for file size");
	SigUsr2Logger(3,log_message);

	char t[18] = "Content Length: ";
	strcpy(response->headers[3].field_name,t);


	if(access(response->resource_path,F_OK)==0)
	{
		int size;
		struct stat st;
		stat(response->resource_path, &st);
		size = st.st_size;	
		sprintf(response->headers[3].field_value,"%d", size);
		total_buffer_size=total_buffer_size+size;
	}
	else
	{
		if(strstr(response->resource_path,"favicon.ico")==NULL)
			handle_errors(response);
		else
		{
			char siz[] = "0";
			strcpy(response->headers[3].field_value,siz);

		}

	}
}

