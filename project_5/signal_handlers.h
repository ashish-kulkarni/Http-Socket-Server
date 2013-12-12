#ifndef SIGNALS_HANDLERS_H
#define SIGNALS_HANDLERS_H

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <time.h>
#include <signal.h>

extern int portno;
extern int no_of_workers;
extern int job_size;
extern char root_directory[PATH_MAX];
extern char response_strategy[PATH_MAX];
extern char log_message[PATH_MAX];
extern int total_request;
extern int total_buffer_size;
extern double servicingtime;
int uptime =0;
double totservicetime=0;
double avgservicetime=0;
extern struct timespec progtime;
struct timespec progtimeend={0,0};
extern int logcounter;
void LogWritter(FILE *stream, char *format, ...);
extern void SigUsr2Logger(int defined_level, char* message);
extern void SigUsr1Info();
extern volatile sig_atomic_t status; 
extern void graceful_shutdown(int sig);

typedef struct{
	int logcounter;
	char *log_level;
}log_level_t;

enum {
    RUNNING,
    SHUTDOWN
};

const log_level_t LOG_LEVEL_LOOKUP[] = {
	{0,"ERROR"},
	{1,"WARNING"},
	{2,"INFO"},
	{3,"DEBUG"}
};

log_level_t loggerlevel;

#endif //SIGNAL_LOOKUP
