#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

#include "client_addr_storage.h"
#include "request_handler.h"

#define SRV_PORT 7777
#define MAX_CONNECTION 10
#define MAX_DATA_SIZE 1000
#define IPV4_EINBETTEN_PREFIX "::ffff:"
#define PREFIX_LEN 7
#define MAXMSGLEN 256


int main(int argc, char **argv) {
	//Set aller Socket descriptors
	fd_set master;
	//Temporärer Socket descriptor Set für select()
	fd_set read_fds;
	//Maximale Anzahl von Socket descriptor im Set
	int fdmax; 

	//Anfangs werden die beiden Sets auf 0 gesetzt
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	//Für die setsockopt() SO_REUSEADDR
	int yes = 1;

	int server_socket, client_socket;
	char server_ip[INET6_ADDRSTRLEN];

	//Client info
	struct sockaddr_in6 serveraddr;
	//Client IP Adresse
	struct sockaddr_storage remoteaddr;
	socklen_t remote_addr_len;
	//Request des Client
	char client_data[MAXMSGLEN];
	int client_data_len;
	//Hostname des Client
	char host_name[INET6_ADDRSTRLEN];
	char client_port[MAX_PORT_LEN];
	//Antwort des Servers and den Client
	char response[MAX_DATA_SIZE];
	//Socket Deskriptor des Servers bekommen
	if ((server_socket = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
		perror("TCP Socket");
		exit(1);
	}
	//Socket Option setzten, sodass socket wiederverwendbar ist(SO_REUSEADDR)
	//SPL_SOCKET um Optionen manipulieren zu können
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

	memset(&serveraddr, 0, sizeof(serveraddr));
	//Adressfamilie auf IPv6 Setzen, IPv4 wird jedoch mit eingebettet
	serveraddr.sin6_family = AF_INET6;
	//Port des Servers wird von Host Byte Reihenfolge in Netzwerk Byte Reihenfolge konvertiert 
	serveraddr.sin6_port = htons(SRV_PORT);
	//Adresse ist unspezifiziert
	serveraddr.sin6_addr = in6addr_any;
	
	//Socket mit Adresse und Portnummer in serveraddr assoziieren 
	//Da die Adresse in serveraddr auf in6addr_any gesetzt wurde, ist die Adresse unspezifiziert 
	if (bind(server_socket, (struct sockaddr *) &serveraddr, sizeof(serveraddr))
			< 0) {
		perror("bind() failed");
		exit(1);
	}

	printf("Binding successful\n");

	//Wartet auf Verbindungen auf dem Socket
	if (listen(server_socket, MAX_CONNECTION) == -1) {
		perror("listen");
		exit(3);
	}
	printf("Listening successful\n");

	//Der "hörende" Socket wird zum Set hinzugefügt
	FD_SET(server_socket, &master);

	//Größter Deskriptor ist bisher der erste
	fdmax = server_socket; 

	while (1) {
		//Set in temporäre kopieren
		read_fds = master;
		//Das Programm wartet und prüft ob die Sockets bereit sind zum lesen/schreiben
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}
		//Über sockets iterieren
		for (int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				//Socket ist enthalten im Set
				if (i == server_socket) {
					//Wenn i == server_socket bedeutet es, dass ein neuer Client sich verbunden hat
					remote_addr_len = sizeof remoteaddr;
					client_socket = accept(server_socket,
							(struct sockaddr *) &remoteaddr, &remote_addr_len);

					if (client_socket == -1) {
						perror("accept");
					} else {
						//Socket zum Set hinzufügen
						FD_SET(client_socket, &master);
						//fdmax aktualisieren
						if (client_socket > fdmax) { 
							fdmax = client_socket;
						}

						//getnameinfo nimmt eine bereits beladene sockaddr Struktur und erhält darüber den Hostnamen
						//NI_NUMERICSERV: Numerische Form der Service Adresse (Port)
						//NI_NUMERICHOST: Numerische Form des Hostnamen
						int name_res = getnameinfo(
								(struct sockaddr*) &remoteaddr, remote_addr_len,
								host_name, INET6_ADDRSTRLEN, client_port,
								MAX_PORT_LEN,
								NI_NUMERICSERV | NI_NUMERICHOST);
						//Das Einbetten von IPv4 in IPv6 führt dazu, dass die Adresse mit einem IPv6 Prefix gespeichert wurde
						//Daher wird dieser entfernt
						if(strncmp(host_name,IPV4_EINBETTEN_PREFIX, PREFIX_LEN) == 0){
							strncpy(host_name, host_name+PREFIX_LEN, INET_ADDRSTRLEN);
						}

						if (name_res == 0) {
							//Client in die Liste speichern
							saveClientAddr(client_socket, host_name,
									client_port);
							getListClient();
						} else {
							gai_strerror(name_res);
							printf("error num: %d \n ", name_res);
							perror("get client info fail: \n");
						}
					}
				} else {
					//Daten vom Client behandeln
					memset(client_data, 0, sizeof(client_data));
					if ((client_data_len = recv(i, client_data,
							sizeof client_data, 0)) <= 0) {
						// Error oder Verbindung wurde vom Client unterbrochen
						if (client_data_len == 0) {
							//Verbindung unterbrochen
							removeClientAddr(i);
							printf("Select srv_ip: socket %d hung up\n", i);
						} else {
							perror("received");
						}
						//Socket schließen und aus dem Set entfernen
						close(i);
						FD_CLR(i, &master);
					} else {
						//Daten vom Client erhalten
						printf("socket %d have sent data: %s\n", i,
								client_data);

						parseRequest(i, client_data, response, server_ip);

						for (int j = 0; j <= fdmax; j++) {
							//Antwort an den Client
							if (FD_ISSET(j, &master)) {
								if (j == i) {
									if (send(j, response, strlen(response), 0)
											== -1) {
										perror("send");
									}
								}
							}
						}
					}
				} 
			} 
		}
	}
}	

