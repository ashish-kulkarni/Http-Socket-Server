#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include <time.h>
#include "csuc_http.h"

#define SMALL_SIZE 100

extern char log_message[PATH_MAX];


/* Displaying time and date */
int time_date(http_response_t *response)
{
	strcpy(log_message,"Entered time/date function");
	SigUsr2Logger(3,log_message);

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



