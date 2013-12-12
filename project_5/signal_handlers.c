#include "signal_handlers.h"

void graceful_shutdown(int sig)
{
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
	    logcounter=((logcounter+1)%4);
	    break;
	default :
	    break;
    }

}


void LogWritter(FILE *stream, char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stream, format, args);
	va_end(args);
}

void SigUsr1Info()
{
	totservicetime = totservicetime+servicingtime;
	avgservicetime = (totservicetime/total_request);
	clock_gettime(CLOCK_MONOTONIC, &progtimeend);
	uptime=((int)progtimeend.tv_sec)-((int)progtime.tv_sec);
	loggerlevel.logcounter=LOG_LEVEL_LOOKUP[logcounter].logcounter;
	loggerlevel.log_level=LOG_LEVEL_LOOKUP[logcounter].log_level;
	//	strcpy(log_level,loggerlevel.log_level);
	LogWritter(stdout,"----------------------Server Information------------------------------\n");
	LogWritter(stdout,"Port Number       =         %d\n",portno);
	LogWritter(stdout,"Root Directory    =         %s\n",root_directory);
	if((strcmp(response_strategy,"Thread Pool")==0))
	{
		LogWritter(stdout,"No. of Workers    =         %d\n",no_of_workers);
		LogWritter(stdout,"Job Size          =         %d\n",job_size);
	}
	LogWritter(stdout,"Response Strategy =         %s\n",response_strategy);
	LogWritter(stdout,"Log Level         =         %s\n",loggerlevel.log_level);
	LogWritter(stdout,"----------------------------------------------------------------------\n");
	LogWritter(stdout,"Total Buffer Size =         %d\n",total_buffer_size);
	LogWritter(stdout,"Total Request     =         %d\n",total_request); 
	LogWritter(stdout,"Service time      =         %.5e Seconds\n",servicingtime);
	LogWritter(stdout,"Ave. Service time =         %.5e Seconds\n",avgservicetime);
	LogWritter(stdout,"Total Uptime      =         %d Hrs %d Mins %d Secs\n",(uptime/3600),(uptime/60),(uptime%60));

	LogWritter(stdout,"----------------------------------------------------------------------\n");
}

void SigUsr2Logger(int defined_level, char* log_message)
{
	if(defined_level <= logcounter)
	{	
		LogWritter(stderr,"------------------Error Block---------------------\n");
		if(defined_level == 0)
			LogWritter(stderr,"Error: %s\n",log_message);
		LogWritter(stderr,"---------------------------------------------------\n");
		if(logcounter == 0)
			return;
		LogWritter(stderr,"------------------Warning Block-------------------\n");
		if(defined_level == 1)
			LogWritter(stderr,"Warning: %s\n",log_message);
		LogWritter(stderr,"Warning: Do not use port number %d for other applications.\n",portno);
		LogWritter(stderr,"Warning: PATH_MAX has maximum size of 4096 on linux systems.\n");
		LogWritter(stderr,"Warning: Buffer of maximum size 8192000 initialized.\n");
                LogWritter(stderr,"Warning: Check for 404.html,400.html or /errors.\n");
		LogWritter(stderr,"--------------------------------------------------\n");
		if(logcounter == 1)
			return;
		LogWritter(stderr,"------------------Info Block----------------------\n");
		if(defined_level == 2)
			LogWritter(stderr,"%s\n",log_message);
		SigUsr1Info();
		LogWritter(stderr,"--------------------------------------------------\n");
		if(logcounter == 2)
			return;
		LogWritter(stderr,"------------------Debug Block---------------------\n");
		if(defined_level == 3)
			LogWritter(stderr,"Debug: %s\n",log_message);
		LogWritter(stderr,"--------------------------------------------------\n");
		//	break;
		//logcounter = ((logcounter+1)%4);
	}
}

