#include "http_header.h"
char root_directory[PATH_MAX] = ".";


int main(int argc, char *argv[])
{

	/*constructing structure objects*/
	http_response_t *response=(http_response_t *)malloc(sizeof(http_response_t) );
	http_request_t *request=(http_request_t *)malloc(sizeof(http_request_t) );
	int c,index,portno = 9000;char *port_string = NULL; count.dir_string = NULL;
	count.forkcount = 0;count.threadcount = 0;count.totalcount = 0;count.dir = 0;
	char default_dir[5] = ".";
	strcpy(response->resource_path,default_dir);
	while ((c = getopt (argc, argv, "ftp:d:")) != -1) //iterate till all arguments are covered
		switch (c) 
		{   
			case 'f':
				count.forkcount++; //fork flag set
				count.totalcount++; //Total Flag counter
				break;
			case 't':
				count.threadcount++; //thread flag set
				count.totalcount++; //total flag counter
				break;
			case 'p':
				port_string = strdup(optarg);
				portno = atoi(port_string);
				free(port_string);
				port_validation(portno);
				break;
			case 'd':
				count.dir_string = strdup(optarg);
				strcpy(root_directory,optarg);
				directory_validation(response);
				count.dir++;
				break;
			case '?':
				if (optopt == 'd')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr,
							"Unknown option character `\\x%x'.\n",
							optopt);
				return 1;
			default:
				abort (); 
		}
	//Handling Invalid Arguments
	for (index = optind; index < argc; index++)
		printf ("Non-option argument %s\n", argv[index]);
	//generate socket
	socket_generator(request,response,portno);
	free(request);
	free(response); 
	return 0;
}

int port_validation(int portno)
{
	if(portno > 1024 && portno <65535)
		return 0;
	else
	{
		perror("Invalid Port number");
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
		perror("Directory does not Exists");
		exit(EXIT_FAILURE);
	}
	int closedir(DIR *dirp); //Closing Directory


}


/*Generating socket connections*/
int socket_generator(http_request_t *request,http_response_t *response,int portno)
{
	struct sockaddr_in sockaddr;
	int sockfd =0;
	FILE *fwrite;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	memset(&sockaddr,0,sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(portno);
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd, ( struct sockaddr*) &sockaddr,sizeof(sockaddr)) == -1)
	{
		perror("bind");
		return 0;
	}

	if(listen(sockfd,50) == -1)
		perror("listen");
	switch(count.totalcount)
	{
		case 0:
			process_serial(request,response,sockfd);
			break;
		case 1:
			if(count.forkcount == 1)
				process_fork(request,response,sockfd);
			else if(count.threadcount ==1)
			{
				main_thread(sockfd);				//	pthread_exit((void *)NULL);
			}
			else perror("Unknown");
			break;
		case 2:
			perror("Not allowed Fork + Thread");
			exit(EXIT_FAILURE);
		default:
			abort();
	}
	return 0;
}

int process_fork(http_request_t *request,http_response_t *response,int sockfd)
{
	int newfd,pid;char sendbuff[PATH_MAX];FILE * fp;
	memset(&sendbuff,0,sizeof(sendbuff));



	while(1)
	{
		newfd=accept(sockfd,(struct sockaddr *) NULL,NULL);
		if (newfd == -1)
		{
			perror("Accept error:");
		}
		else
		{	

			fp = fdopen(dup(newfd),"r");
			if(fp)
			{
				pid =fork();

				if(pid == 0)
				{
					fgets(sendbuff,PATH_MAX,fp);
					nextrequest(request,fp);
					parse_string(sendbuff,request,response,newfd);
					reset_response(response);
					close(newfd);
					return 0;
				}


				else
				{
					while((waitpid(-1,NULL,WNOHANG)>0));
					close(newfd);

				}

			}
			else
			{
				perror("fp");
			}
			fclose(fp);	
		}

	}

	close(sockfd);
	return 0;

}


int process_serial(http_request_t *request,http_response_t *response,int sockfd)
{
	int newfd=0;char sendbuff[PATH_MAX];FILE * fp;
	memset(&sendbuff,0,sizeof(sendbuff));

	while(1)
	{
		newfd=accept(sockfd,(struct sockaddr *) NULL,NULL);
		if (newfd == -1)
		{
			perror("Accept error:");
		}
		else
		{
			fp = fdopen(dup(newfd),"r");
			if(fp)
			{
				fgets(sendbuff,PATH_MAX,fp);
				nextrequest(request,fp);
				parse_string(sendbuff,request,response,newfd);
				reset_response(response);
				close(newfd);

			}
			else
			{
				perror("fp");
			}
			fclose(fp);	
		}

	}

	close(sockfd);
	return 0;

}

int main_thread(int sockfd)
{
	int *ptr_str;
	pthread_t thread1,thread2;


	while(1)
	{
		ptr_str = malloc(sizeof(int));

		*ptr_str=accept(sockfd,(struct sockaddr *) NULL,NULL);
		//		if (*ptr_str == 0)
		//                   *ptr_str=accept(sockfd,(struct sockaddr *) NULL,NULL);


		if (*ptr_str == -1)
		{
			perror("Accept error:");
			return -1;
		}
		else
		{	
			pthread_create(&thread1,NULL,process_thread,ptr_str);
			pthread_join(thread1,NULL);
			pthread_create(&thread2,NULL,process_thread,ptr_str);
			pthread_join(thread2,NULL);

			pthread_detach(thread1);
			pthread_detach(thread2);
		}

	}


}

void *process_thread(void *ptr_str)
{
	char sendbuff[PATH_MAX];FILE * fp=NULL;
	memset(&sendbuff,0,sizeof(sendbuff));
	http_response_t *response=(http_response_t *)malloc(sizeof(http_response_t) );
	http_request_t *request=(http_request_t *)malloc(sizeof(http_request_t) );
	char default_dir[5] = ".";
	strcpy(response->resource_path,default_dir);
	if(count.dir == 1)
		strcpy(response->resource_path,count.dir_string);

	int newfd = *((int*)ptr_str);

	fp = fdopen(dup(newfd),"rw");
	if(fp)
	{
		fgets(sendbuff,PATH_MAX,fp);
		nextrequest(request,fp);
		fclose(fp);

		parse_string(sendbuff,request,response,newfd);

		reset_response(response);

	}
	else
	{
		perror("fp -threads");
	}	

	close(newfd);
}



/*getting more headers */ 
int nextrequest(http_request_t *request, FILE *fp )
{
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

/*tokenizing the strings*/
int parse_string(char *loc,http_request_t *request,http_response_t *response,int newfd)
{
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




/* Buffer Function to arrange function parameters*/
int passarray(http_response_t *response,http_request_t *request,int newfd)
{
	time_date(response);
	filetype(request,response);
	filesize(response);
	build_response(request, response);
	send_response(request,response,newfd);
	return 0;
}



/* Displaying time and date */
int time_date(http_response_t *response)
{
	char *s = malloc(sizeof (char)*SMALL_SIZE);
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
	else if(strncmp(pch,".mp4",4) == 0)
	{
		strcpy(response->headers[2].field_value,"video/mp4");
	}
	else if(strncmp(pch,".mp3",4) == 0)
	{
		strcpy(response->headers[2].field_value,"audio/mpeg");
	}
	else if(strncmp(pch,".mpg",4) == 0)
	{
		strcpy(response->headers[2].field_value,"video/mpeg");
	}
	else if(strncmp(pch,".mpeg",5) == 0)
	{
		strcpy(response->headers[2].field_value,"video/mpeg");
	}
	else if(strncmp(pch,".mov",4) == 0)
	{
		strcpy(response->headers[2].field_value,"video/quicktime");
	}
	else if(strncmp(pch,".json",5) == 0)
	{
		strcpy(response->headers[2].field_value,"application/json");
	}
	else if(strncmp(pch,".xpi",4) == 0)
	{
		strcpy(response->headers[2].field_value,"application/x-xpinstall");
	}
	else if(strncmp(pch,".webm",5) == 0)
	{
		strcpy(response->headers[2].field_value,"video/webm");
	}
	else if(strncmp(pch,".flv",4) == 0)
	{
		strcpy(response->headers[2].field_value,"video/x-flv");
	}
	else if(strncmp(pch,".pdf",4) == 0)
	{
		strcpy(response->headers[2].field_value,"application/pdf");
	}
	else if(strncmp(pch,".rss",4) == 0)
	{
		strcpy(response->headers[2].field_value,"application/rss+xml");
	}
	else if(strncmp(pch,".jquery",5) == 0)
	{
		strcpy(response->headers[2].field_value,"application/x-jquery-tmpl");
	}
	else
	{
		strcpy(response->headers[2].field_value,"text/plain");
	}
	return 0;
}


/*Getting File Size*/
int filesize(http_response_t *response)
{
	char t[18] = "Content Length: ";
	strcpy(response->headers[3].field_name,t);


	if(access(response->resource_path,F_OK)==0)
	{
		int size;
		struct stat st;
		stat(response->resource_path, &st);
		size = st.st_size;	
		sprintf(response->headers[3].field_value,"%d", size);
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


	int handle_errors(http_response_t *response)
	{
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
						perror("Error handling");

					}
				}
			}		
		}
		return 0;


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


	int send_response(http_request_t *request,http_response_t *response, int newfd)
	{
		FILE *newfp, *file;
		if (newfd == 0)
			perror("newfd is 0");
		newfp = fdopen(dup(newfd), "w");
		//newfp = fopen("log.txt", "a+");
		if(newfp== NULL)
		{		
			perror("creating file");
			exit(EXIT_SUCCESS);
		}
		int i =0;

		if(response->status.code == 404 && (strstr(response->resource_path,"favicon.ico")==NULL))
		{
			request->method=HTTP_STATUS_OK;
			response->status.code=HTTP_STATUS_LOOKUP[request->method].code;
			response->status.reason=HTTP_STATUS_LOOKUP[request->method].reason;
			strcpy(response->headers[2].field_value,"text/html");
			filesize(response);

		}
		fprintf(newfp,"HTTP/%d.%d %d %s\r\n",response->major_version,response->minor_version,response->status.code,response->status.reason);



		for(i=0;i<=3;i++)
		{
			fprintf(newfp,"%s%s\r\n",response->headers[i].field_name,response->headers[i].field_value);
		}
		fprintf(newfp,"\n");

		filecontent(newfp,response,newfd);
		fclose(newfp);
		return 0;
	}


	/* Getting file's contents */

	int filecontent(FILE *newfp,http_response_t *response,int newfd)
	{

		FILE *file; int fd;
		int f_size;
		char *buf = malloc(sizeof(char) * MAX_FILE_SIZE);
		memset(buf,0, sizeof(buf));
		if(access(response->resource_path,F_OK)==0)
		{
			file = fopen(response->resource_path, "r");
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
		{
			perror("favicon.ico");
		}
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
		count.forkcount = 0;
		count.threadcount = 0;
		count.totalcount = 0;
		memset(response->resource_path,0, sizeof(response->resource_path));
		if(count.dir == 1)
			strcpy(response->resource_path,count.dir_string);

		return 0;
	}
