#include "data_storage.h"

//Speicher der Dateien
data_info * data_store[MAX_DATA];
//Anzahl Dateien im Speicher
int data_num = 0;
//Speicher um aktuelles Datum um Zeit zu speichern
char current_time[MAX_TIME_LEN];
int i;
//Zum Wissen, ob die Datei schon im Speicher ist
int new_data = 1;
//Zum entfernen des letzten Zeichens beim Get Kommando
//Das letzte Zeichen kommt nur bei dem Get Kommando und nicht bei Put
int LastChar  = 1;
time_t rawtime_modified;
struct tm * timeinfo_modified;

void saveData(char * recv_data_name, char *recv_data) {
	//Check, ob Datei im Speicher ist
	getData(recv_data_name,NULL);
	if(new_data){
		//Datei ist noch nicht im Speicher
		data_info * new_data = (data_info *) malloc(sizeof(data_info));
		//Datei zusammenstellen
		//Inhalt speichern
		strcpy(new_data->data, recv_data);
		//Name speichern
		strcpy(new_data->data_name, recv_data_name);
		//Jetztige Zeit speichern
		now(current_time);
		strcpy(new_data->modified_time, current_time);
		//Größe speichern
		sprintf(new_data->size,"%ld", strlen(new_data->data));
		data_store[data_num++] = new_data;
		printf("\nData was not existed\nNew Data was saved on %sData name: %s\ncontent: %s( size: %s Byte) \n",
				new_data->modified_time, new_data->data_name, new_data->data,
				new_data->size);
	} else {
		//Datei ist schon enthalten und wird überschrieben		
		strcpy(data_store[i]->data , recv_data);
		memset(current_time, 0, sizeof(current_time));			
		//Neue bearbeitete Zeit
		time(&rawtime_modified);
		timeinfo_modified = localtime(&rawtime_modified);
		strcpy(data_store[i]->modified_time,asctime (timeinfo_modified));
		sprintf(data_store[i]->size,"%ld", strlen(data_store[i]->data));
		printf("Data %s, was modified  at %s: \n  ( new size: %s Byte) \n",
			data_store[i]->data_name, data_store[i]->modified_time,
			data_store[i]->size);	
		new_data =1;
	}
}

void getData(char * data_name, char * data_response) {
	if(NULL == data_response){
		LastChar= 0;
		printf("Checking if data name < %s> was existed ... \n", data_name);
	} else{
		LastChar = 1;
		printf("Getting data with name < %s> ... \n", data_name);
	}
	//Dateispeicher durchlaufen
	for (i = 0; i < data_num; i++) {
		//Länge der Namen vergleiche		
		size_t finding_data_length = (int)strlen(data_name)-LastChar;
		if (strlen(data_store[i]->data_name) == finding_data_length) {
			//Namen vergleichen
			if (strncmp(data_store[i]->data_name, data_name, finding_data_length) == 0) {
				if(NULL != data_response){
					//String der Antwort zusammensetzen 
					strcpy(data_response,"Last modified: ");
					strncat(data_response, data_store[i]->modified_time,strlen(data_store[i]->modified_time) - 1);
					strcat(data_response, "(Size: ");
					strncat(data_response, data_store[i]->size, strlen(data_store[i]->size));
					strcat(data_response," Byte)");
					strcat(data_response,"\nContent: ");
					strcat(data_response, data_store[i]->data);
				} else{ new_data = 0;}
				break;
			}
		}
	}
	if(!new_data){
		printf("Found %s in storage \n", data_name);
	}
}
