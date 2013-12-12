#include "http_header.h"
#include "circular_buffer.h"

#define MAX_HEADERS_COUNT 3

int main(int argc, char *argv[])
{
	clock_gettime(CLOCK_MONOTONIC, &progtime);
	/*constructing structure objects*/
	http_response_t *response=(http_response_t *)malloc(sizeof(http_response_t) );
	http_request_t *request=(http_request_t *)malloc(sizeof(http_request_t) );
	int c,index;char *port_string = NULL; count.dir_string = NULL;
	count.forkcount = 0;count.threadcount = 0;count.totalcount = 0;count.dir = 0;
	count.workercount =0; count.queuecount =0;
	char *worker_string;char* queue_string;char default_dir[ULTRA_SMALL] = ".";
	char *error_string;
	strcpy(response->resource_path,default_dir);
	while ((c = getopt (argc, argv, "ftp:d:w:q:v:")) != -1) //iterate till all arguments are covered
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
			case 'w':
				count.workercount++; //worker flag set
				worker_string=strdup(optarg);
				no_of_workers = atoi(worker_string);
				free(worker_string);
				break;
			case 'q':
				count.queuecount++;	//queuesize flag set
				queue_string=strdup(optarg);
				job_size = atoi(queue_string);
				free(queue_string);
				break;
			case 'v':
				error_string=strdup(optarg);
				error_level(error_string);
				free(error_string);
				break;

			default:
				exit(EXIT_FAILURE); 
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

/*Generating socket connections*/
int socket_generator(http_request_t *request,http_response_t *response,int portno)
{
	strcpy(log_message,"Entered Socket generator function");
	SigUsr2Logger(3,log_message);

	struct sockaddr_in sockaddr;
	int sockfd =0;
	FILE *fwrite;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	memset(&sockaddr,0,sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(portno);
	sockaddr.sin_addr.s_addr = INADDR_ANY;

	// signal handling
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags   = 0;
	sa.sa_handler = graceful_shutdown;
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		exit(EXIT_FAILURE);
	}
	if (sigaction(SIGTERM, &sa, NULL) == -1) {
		exit(EXIT_FAILURE);
	}
	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
		exit(EXIT_FAILURE);
	}
	if (sigaction(SIGUSR2, &sa, NULL) == -1) {
		exit(EXIT_FAILURE);
	}

	// end_signal_handling
	if(bind(sockfd, ( struct sockaddr*) &sockaddr,sizeof(sockaddr)) == -1)
	{
		strcpy(log_message,"Binding address");
		SigUsr2Logger(0,log_message);
		return 0;
	}

	if(listen(sockfd,50) == -1)
	{
		strcpy(log_message,"Listening");
		SigUsr2Logger(0,log_message);
		return 0;
	}
	switch(count.totalcount)
	{
		case 0:
			if(count.queuecount == 1)
			{
				if(count.workercount ==1)
					thread_pool(sockfd);
				else
				{
					strcpy(log_message,"No worker Specified");
					SigUsr2Logger(0,log_message);
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				if(count.workercount ==1)
					thread_pool(sockfd);
				else
					process_serial(request,response,sockfd);
			}
			break;
		case 1:
			if(count.workercount == 1 || count.queuecount ==1)
			{
				strcpy(log_message,"Invalid Option: -w -f/-w -t/-q -f/ -q -t");
				SigUsr2Logger(0,log_message);
				exit(EXIT_FAILURE);
			}
			if(count.forkcount == 1)
				process_fork(request,response,sockfd);
			else if(count.threadcount ==1)
			{
				main_thread(sockfd);				//	pthread_exit((void *)NULL);
			}
			else
			{
				strcpy(log_message,"Unknown");
				SigUsr2Logger(0,log_message);
			}
			break;
		case 2:
			strcpy(log_message,"Fork + Thread");
			SigUsr2Logger(0,log_message);
			exit(EXIT_FAILURE);
		default:
			exit(EXIT_FAILURE);
	}
	return 0;
}

int thread_pool(int sockfd)
{
	strcpy(response_strategy,"Thread Pool");
	int newsockfd,load,s,si;
	pthread_t tidgroup[no_of_workers];
	cbInit(&cb,job_size);
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);

	for(load=0;load<no_of_workers;load++)
	{
		si = pthread_sigmask(SIG_BLOCK, &set, NULL);
		if(si!=0)
		{
			strcpy(log_message,"Invalid thread blocking");
			SigUsr2Logger(0,log_message);
		}

		s=pthread_create(&tidgroup[load],0,thread_consumer,NULL);
		if(s!=0)
		{
			strcpy(log_message,"Invalid thread creation");
			SigUsr2Logger(0,log_message);
		}
		si = pthread_sigmask(SIG_UNBLOCK, &set, NULL);
		if(si!=0)
		{
			strcpy(log_message,"Invalid thread blocking");
			SigUsr2Logger(0,log_message);
		}

	}
	while(status == RUNNING)
	{
		newsockfd=accept(sockfd,(struct sockaddr *)NULL,NULL);
		clock_gettime(CLOCK_MONOTONIC, &tstart);

		if(newsockfd != -1)
			thread_producer(&newsockfd);
		//sleep(1);
	}
	for(load=0;load<no_of_workers;load++)
		pthread_detach(tidgroup[load]);
	close(sockfd);
	exit(EXIT_FAILURE);
}

void *thread_consumer()
{
	while(status == RUNNING)
	{
		pthread_mutex_lock(&the_mutex);
		while(cbIsEmpty(&cb))
		{
			pthread_cond_wait(&condc, &the_mutex);
		}
		//	pthread_mutex_lock(&the_mutex);
		//NEXT JOB FROM BUFFER
		cbRead(&cb, &elem);
		int newfd = elem.value; 

		//DECREMENT THE SIZE
		pthread_cond_broadcast(&condp);
		pthread_mutex_unlock(&the_mutex);


		//DO THe job
		char sendbuff[PATH_MAX];FILE * fp=NULL;
		memset(&sendbuff,0,sizeof(sendbuff));
		http_response_t *response=(http_response_t *)malloc(sizeof(http_response_t) );
		http_request_t *request=(http_request_t *)malloc(sizeof(http_request_t) );
		char default_dir[5] = ".";
		strcpy(response->resource_path,default_dir);
		if(count.dir == 1)
			strcpy(response->resource_path,count.dir_string);

		if(newfd != -1)
		{
			fp = fdopen(dup(newfd),"r");
			if(fp)
			{
				fgets(sendbuff,PATH_MAX,fp);
				nextrequest(request,fp);
				parse_string(sendbuff,request,response,newfd);
				reset_response(response);

			}
			else
			{
				strcpy(log_message,"creating File *fp in thread pool");
				SigUsr2Logger(0,log_message);
			}
			fclose(fp);

		}close(newfd);
	}
	exit(EXIT_FAILURE);
}
void thread_producer(int *newfdptr)
{
	pthread_mutex_lock(&the_mutex);
	while(cbIsFull(&cb))
	{
		pthread_cond_wait(&condp,&the_mutex);
	}
	//pthread_mutex_lock(&the_mutex);
	//add to buffer
	elem.value = *newfdptr;
	cbWrite(&cb,&elem);
	//increament buffer size
	pthread_cond_broadcast(&condc);
	pthread_mutex_unlock(&the_mutex);

}

int process_fork(http_request_t *request,http_response_t *response,int sockfd)
{
	strcpy(response_strategy," Process Fork");
	int newfd,pid;char sendbuff[PATH_MAX];FILE * fp;
	memset(&sendbuff,0,sizeof(sendbuff));



	while(status == RUNNING)
	{
		newfd=accept(sockfd,(struct sockaddr *) NULL,NULL);
		clock_gettime(CLOCK_MONOTONIC, &tstart);

		if (newfd != -1)
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
				strcpy(log_message,"creating File *fp in fork");
				SigUsr2Logger(0,log_message);

			}
			fclose(fp);	
		}

	}

	close(sockfd);
	return 0;

}


int process_serial(http_request_t *request,http_response_t *response,int sockfd)
{
	strcpy(response_strategy,"Serial Process");
	int newfd=0;char sendbuff[PATH_MAX];FILE * fp;
	memset(&sendbuff,0,sizeof(sendbuff));

	while(status == RUNNING)
	{
		newfd=accept(sockfd,(struct sockaddr *) NULL,NULL);
		clock_gettime(CLOCK_MONOTONIC, &tstart);
		if (newfd != -1)
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
				strcpy(log_message,"creating File *fp in serial");
				SigUsr2Logger(0,log_message);

			}
			fclose(fp);	
		}

	}

	close(sockfd);
	return 0;

}

int main_thread(int sockfd)
{
	strcpy(response_strategy,"Process Thread");
	int *ptr_str;int si;
	pthread_t thread1,thread2;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);
	while(status == RUNNING)
	{
		ptr_str = malloc(sizeof(int));
		int fd;

		fd=accept(sockfd,(struct sockaddr *) NULL,NULL);
		clock_gettime(CLOCK_MONOTONIC, &tstart);

		*ptr_str =fd;
		//		if (*ptr_str == 0)
		//                   *ptr_str=accept(sockfd,(struct sockaddr *) NULL,NULL);


		if (fd != -1)
		{	
			si = pthread_sigmask(SIG_BLOCK, &set, NULL);
			if(si!=0)
			{
				strcpy(log_message,"Invalid thread blocking");
				SigUsr2Logger(0,log_message);
			}
			pthread_create(&thread1,0,process_thread,ptr_str);
			//		pthread_join(thread1,NULL);
			//sleep(1);
			si = pthread_sigmask(SIG_UNBLOCK, &set, NULL);
			if(si!=0)
			{
				strcpy(log_message,"Invalid thread blocking");
				SigUsr2Logger(0,log_message);
			}
			pthread_detach(thread1);
		}

	}
	close(sockfd);

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

	if(newfd != -1)
	{
		fp = fdopen(dup(newfd),"r");
		if(fp)
		{
			fgets(sendbuff,PATH_MAX,fp);
			nextrequest(request,fp);
			//fclose(fp);
			parse_string(sendbuff,request,response,newfd);
			reset_response(response);
			//	close(newfd);

		}
		else
		{
			strcpy(log_message,"creating File *fp in Threads");
			SigUsr2Logger(0,log_message);

		}
		fclose(fp);

	}close(newfd);

}



/*getting more headers  
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

/*tokenizing the strings
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


/* checking for GET 
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
*/



/* Buffer Function to arrange function parameters*/
int passarray(http_response_t *response,http_request_t *request,int newfd)
{
	strcpy(log_message,"Entered buffer function");
	SigUsr2Logger(3,log_message);

	time_date(response);
	filetype(request,response);
	filesize(response);
	build_response(request, response);
	send_response(request,response,newfd);
	return 0;
}

int build_response(const http_request_t *request, http_response_t *response)
{
	strcpy(log_message,"Unbinding of Port takes ~53 seconds.");
	SigUsr2Logger(1,log_message);

	strcpy(log_message," ");
	SigUsr2Logger(2,log_message);

	total_request++;
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
	newfp = fdopen(dup(newfd), "w");
	if(newfp== NULL)
	{
		strcpy(log_message,"creating File *fp in send response");
		SigUsr2Logger(0,log_message);
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



	for(i=0;i<=MAX_HEADERS_COUNT;i++)
	{
		fprintf(newfp,"%s%s\r\n",response->headers[i].field_name,response->headers[i].field_value);
	}
	fprintf(newfp,"\n");

	filecontent(newfp,response,newfd);
	fclose(newfp);
	clock_gettime(CLOCK_MONOTONIC, &tend);
	servicingtime = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);

	return 0;
}


/* Resetting the Headers*/

int reset_response(http_response_t *response)
{
	strcpy(log_message,"resetting headers and buffers");
	SigUsr2Logger(3,log_message);

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
	count.workercount =0;
	count.queuecount =0;

	memset(response->resource_path,0, sizeof(response->resource_path));
	if(count.dir == 1)
		strcpy(response->resource_path,count.dir_string);

	return 0;
}
