#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include "csuc_http.h"

extern char root_directory[PATH_MAX];
extern char log_message[PATH_MAX];



int handle_errors(http_response_t *response)
{
	strcpy(log_message,"Unbinding takes time.Wait before using the same port number.");
	SigUsr2Logger(3,log_message);


	int size;
	struct stat st;
	FILE *fp;

	memset(response->resource_path,0, sizeof(response->resource_path));
	strcpy(response->resource_path, root_directory);	
	strcat(response->resource_path,"/404.html");
	if(fp=fopen(response->resource_path,"r"))
	{
		if(fp)
		{
			stat(response->resource_path, &st);
			size = st.st_size;	
			sprintf(response->headers[3].field_value,"%d", size);


		}

		fclose(fp);	
	}
	else 
	{
		memset(response->resource_path,0, sizeof(response->resource_path));
		strcpy(response->resource_path, root_directory);	
		strcat(response->resource_path,"/400.html");
		if(fp=fopen(response->resource_path,"r"))
		{
			if(fp)
			{
				stat(response->resource_path, &st);
				size = st.st_size;	
				sprintf(response->headers[3].field_value,"%d", size);

			}

			fclose(fp);	
		}
		else
		{
			memset(response->resource_path,0, sizeof(response->resource_path));
			strcpy(response->resource_path, "errors");	
			strcat(response->resource_path,"/errors.html");
			fp = fopen (response->resource_path,"r");
			{
				if(fp)
				{
					stat(response->resource_path, &st);
					size = st.st_size;	
					sprintf(response->headers[3].field_value,"%d", size);
					fclose(fp);
				}
				else
				{
					strcpy(log_message,"creating File *fp Error handling");
					SigUsr2Logger(0,log_message);
				}
			}
		}		
	}
	return 0;


}


