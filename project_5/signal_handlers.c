#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/time.h>

extern int portno;
extern int no_of_workers;
extern int job_size;
extern char root_directory[PATH_MAX];
extern char response_strategy[PATH_MAX];
extern char log_level[PATH_MAX];
extern int total_request;
extern int total_buffer_size;
extern double servicingtime;
extern double uptime;

void WriterSigUsr1(FILE *stream, char *format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stream, format, args);
	va_end(args);
}
void SigUsr1Info()
{
	WriterSigUsr1(stdout,"----------------------Server Information------------------------------\n");
	WriterSigUsr1(stdout,"Port Number       =         %d\n",portno);
	WriterSigUsr1(stdout,"Root Directory    =         %s\n",root_directory);
	if((strcmp(response_strategy,"Thread Pool")==0))
	{
		WriterSigUsr1(stdout,"No. of Workers    =         %d\n",no_of_workers);
		WriterSigUsr1(stdout,"Job Size          =         %d\n",job_size);
	}
	WriterSigUsr1(stdout,"Response Strategy =         %s\n",response_strategy);
        WriterSigUsr1(stdout,"Log Level         =         %s\n",log_level);
	WriterSigUsr1(stdout,"----------------------------------------------------------------------\n");
	WriterSigUsr1(stdout,"Total Buffer Size =         %d\n",total_buffer_size);
	WriterSigUsr1(stdout,"Total Request     =         %d\n",total_request); 
	WriterSigUsr1(stdout,"Service time      =         %.5eseconds\n",servicingtime);

	WriterSigUsr1(stdout,"----------------------------------------------------------------------\n");

}
