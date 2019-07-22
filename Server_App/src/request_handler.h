#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <string.h>

#include "client_addr_storage.h"
#include "timer.h"
#include "data_storage.h"

#define MAX_REQUEST_LEN 100

#define LIST_COMMAND "List"
#define PUT_COMMAND "Put"
#define GET_COMMAND "Get"
#define QUIT_COMMAND "Quit"
#define MAXHOSTNAMELEN 50
#define MAXDATALEN 50
#define MAXTIMELEN 20

extern char * command;
extern char * received_data;
extern char data_response [];

void parseRequest(int client_socket, char * request, char * response, char * server_ip);

#endif
