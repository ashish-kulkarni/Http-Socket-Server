#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<stdlib.h>
#include<time.h>
#include<errno.h>
#include<unistd.h>
#include<sys/stat.h>
#include"csuc_http.h"
/*#define ENOUGH ((CHAR_BIT * sizeof(int) - 1) / 3 + 2)*/


char **head_name[62][100];
char **head_value[62][100];
int main(int argc, char *argv[])
{
        /*check if the directory is provided by the user*/

	if (argc == 2)
	{
		struct dirent *dirpent;
		DIR *dirp;
		dirp= opendir(argv[1]);
		if(dirp)
		{
			/*constructing structure objects*/
			http_response_t *response=(http_response_t *)malloc(sizeof(http_response_t) );
			http_request_t *request=(http_request_t *)malloc(sizeof(http_request_t) );
			/*getting first string*/
			char *loc = (char*)malloc(1024) ;
			fgets(loc,1023, stdin);
			nextrequest(request);
			parse_string(loc,argv,request,response);
			free(loc);
			free(request);
			free(response); 
		}
		else
		{
			printf("Directory Does not exist.\n");
		}
	}
	else
	{
		printf("Please give the Directory location\n");

	}
	int closedir(DIR *dirp);
	return 0;
}

/*getting more headers */
int nextrequest(http_request_t *request)
{
	char *extraheaders = malloc(sizeof(char)*100);
	char head_name[62][100], head_value[62][100];

	int i =0,counter = 0 ;char *token;char temp[100];
	for (i=0; i<62;i++)
	{
		fgets(extraheaders,1023,stdin);
		if(extraheaders[0] == '\n')
		{
			break;
		}
		else
		{
			strcpy(temp,extraheaders);
			token=strtok(extraheaders, ":");
			if(strcmp(temp,token) == 0)
			{
				printf("EXPECTED ':' \n");
				i--;
			}
			else
			{
				strcpy(&head_name[i][0],token);
				token=strtok(NULL,"\0");
				strcpy(&head_value[i][0],token);
				counter++;
			}
		}
		request->header_count = counter;
	}

	for(i=0;i <= request->header_count;i++)
	{
		strcpy(request->headers[i].field_name,&head_name[i][0]);
		strcpy(request->headers[i].field_value,&head_value[i][0]);


	} 


	free(extraheaders);

	return 0;
}

/*tokenizing the strings*/
int parse_string(char *loc, char *argv[],http_request_t *request,http_response_t *response)
{
	char *token = malloc(sizeof(char)*100);
	char s[3] = " ";char *path;
	char *token1, *token3, *token2;char temp[] ="HTTP";
	token2 = malloc(sizeof(char)*100);
	token1 = malloc(sizeof(char)*100);
	token3 = malloc(sizeof(char)*100);
	token = strtok(loc,s);
	token1 = token;
	token = strtok(NULL,s);
	strcpy(request->uri,token);
	strcpy(token2,token);
	token =strtok(NULL,"/");
	if (*token != *temp)
	{
		printf("Expecting in format HTTP/1.1\n");
		return 0;
	}
	else
	{
		token3 = token;
		token =strtok(NULL,".");
		request->major_version=atoi(token);
		token =strtok(NULL,"\n");
		request->minor_version=atoi(token);
		checkget(token1,token3,argv,request,response);
	}

	return 0;
}

/* checking for GET */
int checkget(char *token1, char *token3, char *argv[],http_request_t *request,http_response_t *response)
{ 
	char *temp = "GET";
	char path[512];
	strcpy(path,argv[1]);
	strcat(path,"/");
	strcat(path,request->uri);



	if(strcmp(token1,temp)==0)
	{
		if(access(path,F_OK)==0)	
			request->method=HTTP_STATUS_OK;

		else
			request->method=HTTP_STATUS_NOT_FOUND;
	
	}
	else
		request->method=HTTP_STATUS_NOT_IMPLEMENTED;

	passarray(request->uri,&path,response,request);
	return 0;
}

/* Displaying time and date */
int time_date(http_response_t *response)
{
	char *s = malloc(sizeof (char)*100);
	char date[7] = "Date: ";
	size_t i;
	struct tm tim;
	time_t now;
	now = time(NULL);
	tim = *(localtime(&now));
	strcpy(response->headers[0].field_name,date);
	i = strftime(s,30,"%a, %d %b %Y %H:%M:%S %Z",&tim);
	strcpy(response->headers[0].field_value,s);
	return 0;
}

/* Finding File Type*/
int type(char *token2,http_response_t *response)
{
	size_t len= strlen(token2);
	char content[16] = "Content-Type: ", ftype[12];
	strcpy(response->headers[2].field_name,content);
	if (len>=4 && strncmp(token2+len-5,".html",5) == 0)
	{
		char ftype[12]="text/html";
	}
	else if(len >=4 && strncmp(token2+len-5,".jpeg",5) == 0)
	{
		char ftype[12]="image/jpeg";
	}
	else if(len>=4 && strncmp(token2+len-4,".png",4) == 0)
	{
		char ftype[12]="image/png";	
	}
	else
	{
		char ftype[12]="Unknown";
	}
	strcpy(response->headers[2].field_value,ftype);



	return 0;
}


/* getting file content*/

int filecontent(char *path[])
{

	FILE *file; int c;
	if(access(*path,F_OK)==0)
	{
	file = fopen(path[0], "r");
	if (file)
	{
		while ((c = getc(file)) != EOF)
			putchar(c);
	}
	else
	{
		perror("Error");
	}
	fclose(file);
	}
	else
		perror("Unable to retrieve");


	return 0;
}


/* Buffer Function to rearrange output*/
int passarray(char *token2,char* path,http_response_t *response,http_request_t *request)
{
	time_date(response);
	type(token2, response);
	filesize(&path,response,request);
	build_response(request, response);
	send_response(response,&path);
	return 0;
}

/*Getting File Size*/
int filesize(char *path[],http_response_t *response, http_request_t *request)
{
	char t[18] = "Content Length: ";
	strcpy(response->headers[3].field_name,t);
	/*char siz[ENOUGH];*/


	if(access(*path,F_OK)==0)
	{
		int size;
		struct stat st;
	        stat(path[0], &st);
		size = st.st_size;	
		sprintf(response->headers[3].field_value,"%d", size);
		/*convert int to string
                strcpy(response->headers[3].field_value,siz);*/

	}
	else
	{
		char siz[] = "0";
                strcpy(response->headers[3].field_value,siz);
	}

}


int build_response(const http_request_t *request, http_response_t *response)
{
	char tempname[] = "Server: ", tempvalue[]= "CSUC HTTP";
        response->status.code=HTTP_STATUS_LOOKUP[request->method].code;
	response->status.reason=HTTP_STATUS_LOOKUP[request->method].reason;
	strcpy(response->resource_path,request->uri);
	response->major_version=request->major_version;
	response->minor_version=request->minor_version;
	response->header_count=request->header_count;
	strcpy(response->headers[1].field_name, tempname);
	strcpy(response->headers[1].field_value, tempvalue);
	return 0;
}


int send_response(http_response_t *response,char *path[])
{
	int i =0;
	printf("HTTP/%d.%d ",response->major_version,response->minor_version);
	printf("%d %s\n",response->status.code,response->status.reason);

	for(i=0;i<=3;i++)
	printf("%s%s\n",response->headers[i].field_name,response->headers[i].field_value);
	filecontent(path);

}
