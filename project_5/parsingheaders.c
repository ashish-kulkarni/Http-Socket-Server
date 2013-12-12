#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include "csuc_http.h"

#define BUFFER_SIZE   1024
#define MID_SIZE      500
#define SMALL_SIZE    100
#define ULTRA_SMALL   10


extern int total_buffer_size;
extern char log_message[PATH_MAX];

/*tokenizing the strings*/
int parse_string(char *loc,http_request_t *request,http_response_t *response,int newfd)
{
	strcpy(log_message,"Entered Parse String function");
	SigUsr2Logger(3,log_message);

	struct stat st;
	char *token = malloc(sizeof(char)*SMALL_SIZE);
	char s[3] = " ";char *lasts; char *uri;
	char *method, *httpversion;char temp[] ="HTTP";
	method = malloc(sizeof(char)*SMALL_SIZE);
	httpversion = malloc(sizeof(char)*SMALL_SIZE);
	uri =malloc(sizeof(char)*SMALL_SIZE);
	sscanf(loc,"%s %s %s",method,request->uri,httpversion);
	if(strstr(request->uri,".")== NULL)
		strcat(request->uri,"index.html");
	if(strstr(request->uri,"?")!=NULL)
	{
		strtok(request->uri,"?");
	}
	else if(strstr(request->uri,"#")!=NULL)
	{
		strtok(request->uri,"#");
	}
	request->major_version=1;
	request->minor_version=1;
	checkget(method,request,response,newfd);

	return 0;
}


/* checking for GET */
int checkget(char *method,http_request_t *request,http_response_t *response,int newfd)
{ 
	strcpy(log_message,"Entering checkget function");
	SigUsr2Logger(3,log_message);

	char *temp = "GET";
	strcat(response->resource_path,request->uri);

	if(strcmp(method,temp)==0)
	{
		if(access(response->resource_path,F_OK)==0)	
			request->method=HTTP_STATUS_OK;

		else
		{
			request->method=HTTP_STATUS_NOT_FOUND;
		}

	}
	else
		request->method=HTTP_STATUS_NOT_IMPLEMENTED;

	passarray(response,request,newfd);
	return 0;
}



/*getting more headers*/  
int nextrequest(http_request_t *request, FILE *fp )
{
	strcpy(log_message,"Entered Next Request function");
	SigUsr2Logger(3,log_message);

	char *extraheaders = malloc(sizeof(char)*BUFFER_SIZE);
	char head_name[MID_SIZE], head_value[MID_SIZE];
	int i =0,counter = 0 ;char *token;char temp[BUFFER_SIZE];
	for (i=0; i<62;i++)
	{
		fgets(extraheaders,BUFFER_SIZE,fp);
		if(extraheaders[0] == '\n')
		{
			return 0;
		}
		else
		{
			if(strstr(extraheaders,":"))
			{
				sscanf(extraheaders,"%[^':']:%s",head_name,head_value);
				strcpy(request->headers[i].field_name,head_name);
				strcpy(request->headers[i].field_value,head_value);
				counter++; 
			}
			else
				break;
		}
		request->header_count = i;
	}
	return 0;
}

