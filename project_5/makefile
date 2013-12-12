csuc_http_ashish.exe : csuc_http_ashish.o mime-types.o signal_handlers.o filesize.o error_handling.o circular_buffer.o filecontents.o validations.o time_date.o parsingheaders.o
	gcc csuc_http_ashish.o mime-types.o signal_handlers.o filesize.o error_handling.o circular_buffer.o filecontents.o  validations.o time_date.o parsingheaders.o -o csuc_http_ashish.exe -pthread -lrt

csuc_http_ashish.o : csuc_http_ashish.c
	gcc -c csuc_http_ashish.c

mime-types.o : mime-types.c
	gcc -c mime-types.c

signal_handlers.o : signal_handlers.c
	gcc -c signal_handlers.c

filesize.o : filesize.c
	gcc -c filesize.c

error_handling.o : error_handling.c
	gcc -c error_handling.c

circular_buffer.o : circular_buffer.c
	gcc -c circular_buffer.c

filecontents.o : filecontents.c
	gcc -c filecontents.c

validations.o : validations.c
	gcc -c validations.c

time_date.o : time_date.c
	gcc -c time_date.c

parsingheaders.o : parsingheaders.c
	gcc -c parsingheaders.c

clean :
	rm *.o csuc_http_ashish.exe
