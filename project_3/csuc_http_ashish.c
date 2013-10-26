#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "csuc_http.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define MAX_FILE_SIZE 819200
#define BUFFER_SIZE   1024
#define MID_SIZE      500
#define SMALL         100

int main(int argc, char *argv[])
{
	/*check if the directory is provided by the user*/

	if (argc == 3)
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
			//	char *loc = (char*)malloc(PATH_MAX) ;
			struct sockaddr_in sockaddr;
			int sockfd =0, newfd =0, pid;
			char sendbuff[PATH_MAX];FILE * fp; FILE *fwrite;
			//	printf("first\n");
			sockfd = socket(AF_INET,SOCK_STREAM,0);
			memset(&sockaddr,0,sizeof(sockaddr));
			memset(&sendbuff,0,sizeof(sendbuff));

			sockaddr.sin_family = AF_INET;
			sockaddr.sin_port = htons(atoi(argv[2]));
			sockaddr.sin_addr.s_addr = INADDR_ANY;
			if(bind(sockfd, ( struct sockaddr*) &sockaddr,sizeof(sockaddr)) == -1)
				perror("bind");

			if(listen(sockfd,10) == -1)
				perror("listen");
			while(1)
			{
				newfd=accept(sockfd,(struct sockaddr *) NULL,NULL);
				if (newfd == -1)
				{
					perror("Accept error:");
				}
				else
				{
					fp = fdopen(newfd,"r");
					if(fp)
					{
						pid =fork();

						if(pid == 0)
						{
							fgets(sendbuff,PATH_MAX,fp);
							nextrequest(request,fp);
							strcpy(response->resource_path,argv[1]);
							parse_string(sendbuff,request,response,newfd);
							reset_response(response);
							close(newfd);
							free(request);
							free(response);
							return 0;
						}


						else
						{
							if(waitpid(-1,NULL,WNOHANG)>0);
							close(newfd);

						}


					}
					else
					{
						perror("fp");
					}
					fclose(fp);	
				}
				//   close(newfd);
				sleep(1);

			}

			close(sockfd);
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
		printf("Please ONLY  give the Directory location and port number\n");

	}
	int closedir(DIR *dirp); // Closing Directory
	return 0;
}

/*getting more headers */ 
int nextrequest(http_request_t *request, FILE *fp )
{
	char *extraheaders = malloc(sizeof(char)*BUFFER_SIZE);
	char head_name[62][SMALL], head_value[62][MID_SIZE];
	int i =0,counter = 0 ;char *token;char temp[BUFFER_SIZE];
	for (i=0; i<62;i++)
	{
		fgets(extraheaders,BUFFER_SIZE,fp);
		if(extraheaders[0] == '\n')
		{
			break;
		}
		else
		{
			if(strstr(extraheaders,":") == NULL)
			{
				break;
				i--;
			}
			else
			{
				token=strtok(extraheaders, ":");
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

	return 0;
}

/*tokenizing the strings*/
int parse_string(char *loc,http_request_t *request,http_response_t *response,int newfd)
{
	struct stat st;
	char *token = malloc(sizeof(char)*SMALL);
	char s[3] = " ";char *lasts; char *uri;
	char *method, *httpversion;char temp[] ="HTTP";
	method = malloc(sizeof(char)*SMALL);
	httpversion = malloc(sizeof(char)*SMALL);
	uri =malloc(sizeof(char)*SMALL);
	sscanf(loc,"%s %s %s",method,request->uri,httpversion);
	lstat(response->resource_path,&st);
//	if(S_ISDIR(st.st_mode))
//		strcat(request->uri,"index.html");
	if(strstr(request->uri,".")== NULL)
		strcat(request->uri,"index.html");
        if(strstr(request->uri,"?")!=NULL)
	{
//		strcpy(uri,request->uri);
//		memset(request->uri,0,sizeof(request->uri));
		strtok(request->uri,"?");
	}
	else if(strstr(request->uri,"#")!=NULL)
	{
	//	strcpy(uri,request->uri);
	//	memset(request->uri,0,sizeof(request->uri));
		strtok(request->uri,"#");
	}
	printf("uri is %s\n",request->uri);

	strtok(httpversion,"/");
	token =strtok(NULL,".");
	request->major_version=atoi(token);
	token =strtok(NULL,"\r");
	request->minor_version=atoi(token);
	checkget(method,request,response,newfd);

	return 0;
}

/* checking for GET */
int checkget(char *method,http_request_t *request,http_response_t *response,int newfd)
{ 
	char *temp = "GET";
	char path[MID_SIZE];
	memset(path,0, sizeof(path));
	strcpy(path,response->resource_path);
	strcat(path,request->uri);

	if(strcmp(method,temp)==0)
	{
		if(access(path,F_OK)==0)	
			request->method=HTTP_STATUS_OK;

		else
			request->method=HTTP_STATUS_NOT_FOUND;

	}
	else
		request->method=HTTP_STATUS_NOT_IMPLEMENTED;

	passarray(&path,response,request,newfd);
	return 0;
}


/* Buffer Function to arrange function parameters*/
int passarray(char* path,http_response_t *response,http_request_t *request,int newfd)
{
	time_date(response);
	filetype(request,response);
	filesize(&path,response,request);
	build_response(request, response);
	send_response(response,&path,newfd);
	return 0;
}



/* Displaying time and date */
int time_date(http_response_t *response)
{
	char *s = malloc(sizeof (char)*SMALL);
	char date[7] = "Date: ";
	size_t i;
	struct tm tim;
	time_t now;
	now = time(NULL);
	tim = *(gmtime(&now));
	strcpy(response->headers[0].field_name,date);
	i = strftime(s,30,"%a, %d %b %Y %H:%M:%S %Z",&tim);
	strcpy(response->headers[0].field_value,s);
	return 0;
}

/* Finding File Type*/
int filetype(http_request_t *request,http_response_t *response)
{
	size_t len= strlen(request->uri); char *pch = NULL;
	char content[16] = "Content-Type: ";
	strcpy(response->headers[2].field_name,content);
	pch = strrchr(request->uri,'.');
	if (strncmp(pch,".html",5) == 0)
	{
		strcpy(response->headers[2].field_value,"text/html");

	}
	else if(strncmp(pch,".jpeg",5) == 0 || strncmp(pch,".jpg",4) == 0)
	{
		strcpy(response->headers[2].field_value,"image/jpeg");

	}
	else if(strncmp(pch,".png",4) == 0)
	{
		strcpy(response->headers[2].field_value,"image/png");
	}
	else if(strncmp(pch,".css",4) == 0)
	{
		strcpy(response->headers[2].field_value,"text/css");
	}
	else if(strncmp(pch,".js",3) == 0)
	{
		strcpy(response->headers[2].field_value,"application/javascript");
	}
	else if(strncmp(pch,".xml",4) == 0)
	{
		strcpy(response->headers[2].field_value,"application/xml");
	}
	else
	{
		strcpy(response->headers[2].field_value,pch);
	}
	return 0;
}


/*Getting File Size*/
int filesize(char *path[],http_response_t *response, http_request_t *request)
{
	char t[18] = "Content Length: ";
	strcpy(response->headers[3].field_name,t);


	if(access(*path,F_OK)==0)
	{
		int size;
		struct stat st;
		stat(path[0], &st);
		size = st.st_size;	
		sprintf(response->headers[3].field_value,"%d", size);
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
	response->major_version=request->major_version;
	response->minor_version=request->minor_version;
	response->header_count=request->header_count;
	strcpy(response->headers[1].field_name, tempname);
	strcpy(response->headers[1].field_value, tempvalue);
	return 0;
}


int send_response(http_response_t *response,char *path[], int newfd)
{
	FILE *newfp, *file;
	newfp = fdopen(newfd, "w");
	//	newfp = fopen("tempo.txt","w+");
	if(newfp== NULL)
		perror("creating file");
	int i =0;
	fprintf(newfp,"HTTP/%d.%d %d %s\r\n",response->major_version,response->minor_version,response->status.code,response->status.reason);


	for(i=0;i<=3;i++)
	{
		fprintf(newfp,"%s%s\r\n",response->headers[i].field_name,response->headers[i].field_value);
	}
	fprintf(newfp,"\n");

	filecontent(path,newfp);
	fclose(newfp);
	return 0;
}


/* Getting file's contents */

int filecontent(char *path[], FILE *newfp)
{

	FILE *file; int fd;
	int f_size;
	char *buf = malloc(sizeof(char) * MAX_FILE_SIZE);
	memset(buf,0, sizeof(buf));
	if(access(*path,F_OK)==0)
	{
		file = fopen(path[0], "r");
		if (file)
		{
			while((f_size= fread(buf,1,sizeof(buf),file)) > 0)
				fwrite(buf,1,f_size,newfp);

			close(fd);	
		}
		else
		{
			perror("Printing");
		}
		fclose(file);
	}
	else
		perror("favicon.ico");
	free(buf);

	return 0;
}

/* Resetting the Headers*/

int reset_response(http_response_t *response)
{
	int heads, header_count =3;
	for(heads=0;heads<response->header_count;heads++)
	{
		strcpy(response->headers[heads].field_name,"");
		strcpy(response->headers[heads].field_value,"");
	}
	response->header_count = 0;
	return 0;
}
