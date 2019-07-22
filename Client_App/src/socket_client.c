#include <sys/types.h>
#include <sys/socket.h>
#include<netdb.h>
#include<string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXMSGLEN 256
#define SRV_PORT "7777"
#define PORT_MAX_LENGTH 6
#define MAX_CONNECTION 10
#define LIST_COMMAND "List"
#define PUT_COMMAND "Put"
#define GET_COMMAND "Get"
#define QUIT_COMMAND "Quit"
#define QUIT_RESPONSE "Closed"
#define HELP_COMMAND "Help"
#define MAX_DATA_SIZE 1024 // max number of bytes we can get at once
#define MAX_DATA_NAME_SIZE 100
#define MAX_COMMAND_SIZE 5


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		printf("IPV4\n");
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}
	printf("IPV6\n");
	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int main(int argc, char **argv) {
	//hints: zur Bestimmung der Kriterien der socket adress Struktur,
	//die von getaddrinfo in res gespeichert werden
	//res: Speicher für die von getaddrinfo zurückgegeben addrinfo Strukturen
	//p: Zum iterieren über res 
	struct addrinfo hints, *res, *p;
	int client_socket;
	//Variable für die Rückgabe von send() und recv()
	int nBytes;
	//Variable für die Rückgabe von inet_pton()
	int rv;

	if(NULL == argv[1]){
		printf("U NEED GIVE AN IP TO CONNECT !!! \n");
		exit(0);
	}
	//IP zum Verbinden über Konsoleneingabe auslesen
	char *server = argv[1];
	//Variable zum Speicher der Konvertierten Adresse von inet_pton()
	struct in6_addr serveraddr;
	//Speicher für den Namen der Server Adresse, durch inet_ntop() erhalten
	char server_host[INET6_ADDRSTRLEN];


	//Speicher für die Eingaben des Kommandos
	char buffer[MAXMSGLEN];
	//delimiter für strtok(), um zu testen ob die commands korrekt sind
	char delimiter[] = "-";
	//Speicher für den Request, der an den Server gesendet wird
	char request[MAX_DATA_SIZE+ MAX_DATA_NAME_SIZE + MAX_COMMAND_SIZE];
	//Speicher für die Antwort des Servers
	char server_response[(INET6_ADDRSTRLEN +PORT_MAX_LENGTH +1)* MAX_CONNECTION +7];


	memset(&hints, 0x00, sizeof hints);
	//Das Argument service bei getaddrinfo darf nicht NULL sein
	hints.ai_flags = AI_NUMERICSERV;
	//Erlaubt IPv4 oder IPv6
	hints.ai_family = AF_UNSPEC;
	//Verbindungsorientiertes Protokoll für TCP
	hints.ai_socktype = SOCK_STREAM;

	printf("Connecting to: %s ... \n", server);


	//konvertiert Eingabe in eine IPv4 Adresse
	//AF_INET gibt an, dass es sich um eine IPv4 handeln sollte
	rv = inet_pton(AF_INET, server, &serveraddr);
	//Wenn rv != 1 dann bedeutet dies, dass es keine gültige IPv4 Adresse ist
	if (rv == 1) {
		//Es handelt sich um eine IPv4 Adresse und die Hint Flags werden entsprechend gesetzt
		hints.ai_family = AF_INET;
		//Argument node muss eine numerische Netzwerkadresse sein
		hints.ai_flags |= AI_NUMERICHOST;
	} else {
		rv = inet_pton(AF_INET6, server, &serveraddr);
		//Wenn rv != 1 dann keine gültige IPv6 Adresse
		if (rv == 1) {
			hints.ai_family = AF_INET6;
			hints.ai_flags |= AI_NUMERICHOST;
		}
	}


	//Gibt uns Informationen in Form einer oder mehrerer addrinfo Strukturen
	//Diese enthält eine Adresse an die sich der Client verbinden kann
	if ((rv = getaddrinfo(server, SRV_PORT, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	sleep(1);
	//Durch die zurückgegeben Liste iterieren und mit der ersten gültigen Adresse verbinden
	for (p = res; p != NULL; p = p->ai_next) {
		//Gibt Socket Deskriptor zurück
		if ((client_socket = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		} else {
			printf("Socket %d was created \n", client_socket);
		}
		//Verbindung aufbauen über den von socket() erhaltenen Socket
		if (connect(client_socket, p->ai_addr, p->ai_addrlen) == -1) {
			close(client_socket);
			perror("client: connect");
			continue;
		} else {
			printf("Connection was created \n");
		}
		break;
	}
	//Erster Eintrag war NULL und Verbindung ist fehlgeschlagen
	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	//Adresse in String konvertieren zur Ausgabe auf die Konsole
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *) p->ai_addr),
			server_host, sizeof server_host);
	printf("Connected to %s\n", server_host);
	//Freigeben da res nicht mehr genutzt wird
	freeaddrinfo(res);
	while (1) {
		//Buffer zurücksetzten
		memset(buffer, 0, sizeof(buffer));
		memset(request, 0, sizeof(request));
		char *data_name;
		char * data;

		//Command über Konsoleneingabe bekommen
		printf("Enter your command here: \n");
		fgets(buffer, sizeof(buffer), stdin);

		//strtok teilt einen string an dem delimiter "-"
		char *command = strtok(buffer, delimiter);

		//Dadurch bekommen wir nur das Kommando und können abfragen, ob es ein get oder put Kommando ist
		if ((strncmp(command, GET_COMMAND, 3) == 0) || (strncmp(command, PUT_COMMAND, 3) == 0)) {
			data_name = strtok(NULL, delimiter);
			if(NULL == data_name) { printf("no data name or no delimiter '-' after command \n"); continue;}
			if (strncmp(command, PUT_COMMAND, 3) == 0) {
				//Wenn es sich um put handelt muss noch der Inhalt gespeichert werden
				data = strtok(NULL, delimiter);
			}
		}
		//Sicherstellen, dass es sich um ein gültiges Kommando handelt
		int requestHasNoError = 1;
		//Zusammenstellen der Anfrage
		if (strncmp(command, PUT_COMMAND, 3) == 0) {
			if(NULL == data){ printf("data have no content or no delimiter '-' after data name \nplease enter new content \n"); continue;}
			strcpy(request, "Put");
			strcat(request, "-");
			strcat(request, data_name);
			strcat(request, "-");
			strcat(request, data);
			strcat(request, "\0");
			requestHasNoError = 1;

		} else if (strncmp(command, GET_COMMAND, 3) == 0) {
			strcpy(request, command);
			strcat(request, "-");
			strcat(request, data_name);
			strcat(request, "\0");

			requestHasNoError = 1;

		} else if (strncmp(command, LIST_COMMAND, 4) == 0) {
			strcpy(request, command);
			strcat(request, "\0");
			requestHasNoError = 1;

		} else if (strncmp(command, QUIT_COMMAND, 4) == 0) {
			strcpy(request, command);
			strcat(request, "\0");
			printf("Socket was closed \n");
		} else {
			printf("SYNTAX ERROR: this command does not exit \n");
			requestHasNoError = 0;
		}

		if (requestHasNoError) {
			memset(server_response, 0, sizeof(server_response));
			//Die Anfrage wird an den Server gesendet
			if ((nBytes = send(client_socket, request, strlen(request), 0)) == -1) {
				perror("send");
				exit(1);
			}
			//Client wartet dann auf eine Antwort des Servers
			if ((nBytes = recv(client_socket, server_response, sizeof server_response ,
					0)) == -1) {
				perror("recv");
				exit(1);
			}
			if(NULL == server_response){
				printf("ERROR: nothing received");
			}
			if(strncmp(server_response, QUIT_RESPONSE, 5) == 0){
			 close(client_socket);
			 printf("Socket closed success \n");
			 exit(0);
			 }
			printf("Server response:\n%s\n", server_response);
		}
	}

}
