/**
 * @file fileManager.c
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief all file utilities
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileManager.h"

char* serialize(datagram_t* datagram)
{
	char* res = malloc(9 * DATASIZE);
	char* tmp = malloc(DATASIZE);
	sprintf(tmp, "%d", datagram->transaction);
	strcpy(res, tmp);
	strcat(res, "£");
	strcat(res, datagram->project_name);
	strcat(res, "£");
	strcat(res, datagram->user_name);
	strcat(res, "£");
	sprintf(tmp, "%d", datagram->version);
	strcat(res, tmp);
	strcat(res, "£");
	strcat(res, datagram->file_path);
	strcat(res, "£");
	sprintf(tmp, "%d", datagram->datagram_number);
	strcat(res, tmp);
	strcat(res, "£");
	sprintf(tmp, "%d", datagram->datagram_total);
	strcat(res, tmp);
	strcat(res, "£");
	sprintf(tmp, "%d", datagram->data_length);
	strcat(res, tmp);
	strcat(res, "£");
	strcat(res, datagram->data);

	return res;
}

datagram_t* unserialize(char * serial)
{
	printf("Received a datagram :\n");
	char ** tab = malloc(9 * DATASIZE);
	char * item = strtok (serial,"£");
	unsigned int i = 0;

  	while (item != NULL && i<10)
  	{
  		tab[i] = malloc(DATASIZE);
  		tab[i] = item;
    	item = strtok (NULL, "£");	
    	++i;
 	}

	datagram_t* datagram = malloc(sizeof(datagram_t));
	datagram->transaction = atoi(tab[0]);
	printf("Transaction: %d\n", datagram->transaction);
	strcpy(datagram->project_name,tab[1]);
	printf("Project: %s\n", datagram->project_name);
	strcpy(datagram->user_name,tab[2]);
	printf("User: %s\n", datagram->user_name);
	datagram->version = atoi(tab[3]);
	printf("Version: %d\n", datagram->version);
	strcpy(datagram->file_path,tab[4]);
	printf("File: %s\n", datagram->file_path);
	datagram->datagram_number = atoi(tab[5]);
	datagram->datagram_total = atoi(tab[6]);
	datagram->data_length = atoi(tab[7]);
	strcpy(datagram->data,tab[8]);

	return datagram;
}

datagram_t **prepare_file(char* project_name, char* file_path, 
                          transaction_t transaction, unsigned int version,
	                      char* user_name) 
{
	FILE* file;
   	datagram_t** data_tab;
    if ((file = fopen(file_path, "r+")) == NULL)
    {
        perror("Error: Cannot open file.");
    } else
    {
    	fseek(file, 0L, SEEK_END);
		unsigned int nb_datagrams = (ftell(file) / DATASIZE) + 1;
		printf("%d Datagrams\n", nb_datagrams);
		rewind(file);
		unsigned int i;
		data_tab = malloc(nb_datagrams * sizeof(datagram_t*));
		for (i=0; i<nb_datagrams; ++i)
		{
			printf("Loop %d\n", i);
		    data_tab[i] = malloc(sizeof(datagram_t*));
			fread(data_tab[i]->data, sizeof(data_tab[i]->data), 1, file);
			printf("Content: %s\n", data_tab[i]->data);
		    data_tab[i]->transaction = transaction;
		    printf("Transaction: %d\n", data_tab[i]->transaction);
		    strcpy(data_tab[i]->project_name, project_name);
		    printf("Project: %s\n", data_tab[i]->project_name);
		    strcpy(data_tab[i]->user_name, user_name);
		    printf("User: %s\n", data_tab[i]->user_name);
		    data_tab[i]->version = version;
		    printf("Version: %d\n", data_tab[i]->version);
		    strcpy(data_tab[i]->file_path, basename(file_path));		    
		    printf("File: %s\n", data_tab[i]->file_path);
		    data_tab[i]->datagram_number = i;
		    int read_bytes = (ftell(file) - (i*sizeof(data_tab[i]->data)));
		    data_tab[i]->data_length = read_bytes;
		}
        fclose(file);
    }
	return data_tab;
}


void rebuild_file(char* project_name, char* file_path, datagram_t** tab) 
{
	FILE* file;
	file = fopen(file_path, "wb+");
	unsigned int i = 0;
  	for(i=0; tab[i] != NULL; ++i) {
    	fwrite(tab[i]->data,
    	tab[i]->data_length, 1, file);
  	}
}

void create_dir(char* dir_path)
{
	//read/write/search permissions for owner and group, read/search for others. 
	mkdir(dir_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

void rename_dir(char* dir_path)
{

}

void copy_dir(char* dir_path) 
{

}

void remove_dir(char* dir_path)
{
	
}