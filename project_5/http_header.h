#ifndef HTTP_HEADER
#define HTTP_HEADER

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include "csuc_http.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ctype.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/time.h>

#define MAX_FILE_SIZE 819200
#define BUFFER_SIZE   1024
#define MID_SIZE      500
#define SMALL_SIZE    100
#define ULTRA_SMALL   10
//global struct
typedef struct counted
{
	short forkcount;
	short threadcount;
	short totalcount;
	int   dir;
	char *dir_string;
	short workercount;
	short queuecount;
}count_t;

count_t count;
//global variables
int portno =9000;
char root_directory[PATH_MAX] = ".";
int no_of_workers=0;
int job_size = 16;
char response_strategy[PATH_MAX];
char log_level[PATH_MAX];
int total_request=0;
int total_buffer_size=0;
enum {
    RUNNING,
    SHUTDOWN
};

volatile sig_atomic_t status = RUNNING;

void graceful_shutdown(int sig)
{
    printf("%s is triggered\n", strsignal(sig));
    switch(sig)
    {
	case SIGINT :
	    status = SHUTDOWN;
	    break;
	case SIGTERM :
	    status = SHUTDOWN;
	    break;
	case SIGUSR1 :
	    SigUsr1Info();
	    break;
	case SIGUSR2 :
	    break;
	default :
	    break;
    }

}

//Time functions
struct timespec tstart={0,0}, tend={0,0};
double servicingtime =0;
/* Opaque buffer element type.  This would be defined by the application. */
typedef struct { int value; } ElemType;

/* Circular buffer object */
typedef struct {
	int         size;   /* maximum number of elements           */
	int         start;  /* index of oldest element              */
	int         end;    /* index at which to write new element  */
	ElemType   *elems;  /* vector of elements                   */
} CircularBuffer;


static pthread_mutex_t the_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condc = PTHREAD_COND_INITIALIZER;
static pthread_cond_t condp = PTHREAD_COND_INITIALIZER;

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
int thread_pool(int sockfd);
void *thread_consumer();
void thread_producer();
void cbInit(CircularBuffer *cb, int size);
void cbFree(CircularBuffer *cb);
int cbIsFull(CircularBuffer *cb);
int cbIsEmpty(CircularBuffer *cb);
void cbWrite(CircularBuffer *cb, ElemType *elem);
void cbRead(CircularBuffer *cb, ElemType *elem);


#endif //HTTP_HEADER
