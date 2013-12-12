#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include "csuc_http.h"

extern char log_message[PATH_MAX];

/* Finding File Type */
int filetype(http_request_t *request,http_response_t *response)
{
	strcpy(log_message,"Checking for mime-types");
	SigUsr2Logger(3,log_message);

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

