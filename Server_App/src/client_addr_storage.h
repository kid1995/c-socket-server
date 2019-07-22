/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   api.h
 * Author: kiet
 *
 * Created on May 11, 2019, 1:24 PM
 */

#ifndef API_H
#define API_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

#define MAX_BACK_LOG 10
#define MAX_PORT_LEN 10
#define MAX_CLIENT_OUTP MAX_BACK_LOG * ( INET6_ADDRSTRLEN + MAX_PORT_LEN + 2 + 7)
#define MAX_TMP_LEN INET6_ADDRSTRLEN + MAX_PORT_LEN + 1

typedef struct client_info {
	int descriptor;
	char ip_addr[INET6_ADDRSTRLEN];
	char port[MAX_PORT_LEN];
} client_info;

extern client_info * connected_client_list[];
extern char client_list[];

void saveClientAddr(int client_socket_no, char * client_host, char * client_port);
void removeClientAddr(int client_socket_nr);
void getListClient();
void getClient(int socket_num, char * response);
#endif /* API_H */

