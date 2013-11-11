#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#ifndef HTTP_HEADER
#define HTTP_HEADER

#include <sys/stat.h>
#include "csuc_http.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ctype.h>
#include <pthread.h>

#define MAX_FILE_SIZE 819200
#define BUFFER_SIZE   1024
#define MID_SIZE      500
#define SMALL_SIZE    100

typedef struct counted
{
	short forkcount;
	short threadcount;
	short totalcount;
	int   dir;
	char *dir_string;
}count_t;

count_t count;

int  directory_validation(http_response_t *response);
int socket_generator(http_request_t *request,http_response_t *response,int portno);
int process_fork(http_request_t *request,http_response_t *response,int sockfd);
int process_serial(http_request_t *request,http_response_t *response,int sockfd);
void *process_thread(void *ptr_str);
int nextrequest(http_request_t *request, FILE *fp);
int parse_string(char *loc,http_request_t *request,http_response_t *response,int newfd);
int checkget(char *method,http_request_t *request,http_response_t *response,int newfd);
int passarray(http_response_t *response,http_request_t *request,int newfd);
int time_date(http_response_t *response);
int filetype(http_request_t *request,http_response_t *response);
int filesize(http_response_t *response);
int build_response(const http_request_t *request, http_response_t *response);
int send_response(http_request_t *request,http_response_t *response,int newfd);
int filecontent(FILE *newfp,http_response_t *response,int newfd);
int reset_response(http_response_t *response);

#endif //HTTP_HEADER
