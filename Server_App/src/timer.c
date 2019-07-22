#include "timer.h"

void now(char * current_time) {
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strcpy(current_time,asctime (timeinfo));
}
