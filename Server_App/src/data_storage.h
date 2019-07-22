

#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "timer.h"

#define MAX_DATA 10
#define MAX_DATA_LEN  50
#define MAX_TIME_LEN  50

typedef struct data_info
{
	char data[MAX_DATA_LEN];
	char data_name [MAX_DATA_LEN];
	char modified_time [MAX_TIME_LEN];
	char size [MAX_DATA_LEN];
}data_info;

extern data_info * data_store [];


void saveData(char * recv_data_name, char *recv_data);
void getData(char * data_name, char * data_response);

#endif /* DATA_STORAGE_H */
