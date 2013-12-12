#ifndef COUNTERS_H
#define COUNTERS_H

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

#endif //COUNTERS.H
