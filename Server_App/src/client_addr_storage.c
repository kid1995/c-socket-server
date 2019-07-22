/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "client_addr_storage.h"

//Liste der Clients mit Deskriptor, IP Adresse und Port
client_info * connected_client_list[MAX_BACK_LOG];
//Ausgabe der Client Info
char client_list[MAX_CLIENT_OUTP];
//Temporäre Liste
char temp[MAX_TMP_LEN];
//Größe der Liste
int size = 0;


void saveClientAddr(int client_socket_no, char * client_host,
		char * client_port) {
	client_info * new_client = (client_info *) malloc(sizeof(client_info));
	strcpy(new_client->ip_addr, client_host);
	strcpy(new_client->port, client_port);
	new_client->descriptor = client_socket_no;
	connected_client_list[size++] = new_client;
	printf("Size %d\n", size);
}

void removeClientAddr(int client_socket_no) {
	int i = 0;
	//Client finden, welcher entfernt werden soll
	for (i = 0; i < size; i++) {
		if (client_socket_no == connected_client_list[i]->descriptor) {
			//connected_client_list[i] = NULL;
			printf("Found socket %d and removed from List \n", i);
			break;
		}
	}
	//Liste anpassen
	while (i < size) {
		connected_client_list[i] = connected_client_list[i + 1];
		i++;
	}
	size--;
}

void getListClient() {
	memset(client_list, 0,MAX_CLIENT_OUTP);
	strcpy(client_list, "List:\n");
	memset(temp, 0, MAX_TMP_LEN);
	//Liste durchlaufen und Ausgabe zusammensetze
	for (int i = 0; i < size; i++) {
		strcpy(temp, connected_client_list[i]->ip_addr);
		strcat(temp, ":");
		strncat(temp, connected_client_list[i]->port,
				strlen(connected_client_list[i]->port));
		strcat(client_list, temp);
		strcat(client_list, " ");
		if (size > 1) {
			strcat(client_list, "\n");
		}
	}
	printf("%s \n", client_list);
}

void getClient(int socket_num, char * response) {
	memset(temp, 0, MAX_TMP_LEN);
	int j = 0;
	//Gewollten Client suchen
	for (j = 0; j < size; j++) {
		if (socket_num == connected_client_list[j]->descriptor) {
			break;
		}
	}
	//IP und Port and Antwort anfügen
	strcpy(temp, connected_client_list[j]->ip_addr);
	strcat(temp, ":");
	strncat(temp, connected_client_list[j]->port,
	strlen(connected_client_list[j]->port));
	strcat(response, ", ");
	strcat(response, temp);
	printf("response after get client %s \n", response);
}

