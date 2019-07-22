#include "request_handler.h"

//Kommando des Clients
char * command = " ";
//Buffer für Client Request
char handler_buffer[MAX_REQUEST_LEN];
//Delimiter für strtok
char delimiter[] = "-";
//Hostname des Servers
char hostname[MAXHOSTNAMELEN];
//Antwort mit Datei
char data_response [MAXDATALEN];
//Jetztiges Datum und Zeit
char current_time [MAXTIMELEN];
char * quit_response = "Closed";


void parseRequest(int client_socket, char * request, char * response, char * server_ip) {
	char * data_name;
	char * data;
	//Kommando vom Client erhalten
	strcpy(handler_buffer, request);
	command = strtok(handler_buffer, delimiter);

	if ((strncmp(command, GET_COMMAND, 3) == 0)	|| (strncmp(command, PUT_COMMAND, 3) == 0)) {
		data_name = strtok(NULL, delimiter);

		//Ist das Kommando Get?
		if (strncmp(command, GET_COMMAND, 3) == 0) {
			//NOT FOUND anfangs, falls die Datei nicht gefunden wurde
			strcpy(data_response,"NOT FOUND");
			getData(data_name,data_response);
			//Erhaltene Datei in Antwort speichern
			strcpy(response,data_response);
		} else {
		//PUT Kommando
			//Dateiinhalt erhalten
			data = strtok(NULL, delimiter);
			//In Speicher schreiben
			saveData(data_name, data);
			strcpy(response, "Ok,");
			//Hostnamen bekommen
			if (gethostname(hostname, MAXHOSTNAMELEN) != 0) {
				perror("CANT GET HOST NAME: ");
			} else {
				//Hostnamen zur Antwort hinzufügen
				strcat(response, hostname);
			}
			//IP und Port des Clients der Antwort anfügen
			getClient(client_socket,response);
			strcat(response, ",");
			//Zuletzt modifiziert zur Antwort hinzufügen
			now(current_time);
			strncat(response, current_time, strlen(current_time));
		}
	} else if (strncmp(command, LIST_COMMAND, 4) == 0) {
		getListClient();
		strcpy(response, client_list);
	} else if (strncmp(command, QUIT_COMMAND, 4) == 0) {
		strcpy(response, quit_response);
	}
}
