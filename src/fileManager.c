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

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

char* serialize(datagram_t* datagram)
{
	char* res = malloc(12 * sizeof(char) + strlen(datagram->project_name)
										 + strlen(datagram->user_name)
										 + strlen(datagram->file_path)
										 + strlen(datagram->data));
	char* tmp = malloc(3 * sizeof(char));
	


	sprintf(tmp, "%d", datagram->transaction);
	strcpy(res, tmp);
	strcat(res, "¤");
	strcat(res, datagram->project_name);
	strcat(res, "¤");
	strcat(res, datagram->user_name);
	strcat(res, "¤");
	sprintf(tmp, "%d", datagram->version);
	strcat(res, tmp);
	strcat(res, "¤");
	strcat(res, datagram->file_path);
	strcat(res, "¤");
	sprintf(tmp, "%d", datagram->datagram_number);
	strcat(res, tmp);
	strcat(res, "¤");
	sprintf(tmp, "%d", datagram->datagram_total);
	strcat(res, tmp);
	strcat(res, "¤");
	sprintf(tmp, "%d", datagram->data_length);
	strcat(res, tmp);
	strcat(res, "¤");
	strcat(res, datagram->data);

	return res;
}

datagram_t* unserialize(char * serial)
{
	char * tab[9];

	char * item = strtok(serial,"¤");
	unsigned int i = 0;
  	while (item != NULL)
  	{
	   	tab[i] = malloc(strlen(item));
  		tab[i] = item;
    	item = strtok(NULL, "¤");
    	++i;
 	}

 	datagram_t* datagram = malloc(sizeof(datagram_t));
	datagram->project_name = malloc(strlen(tab[1]));
	datagram->user_name = malloc(strlen(tab[2]));
	datagram->file_path = malloc(strlen(tab[4]));
	datagram->data = malloc(strlen(tab[8]));

	datagram->transaction = atoi(tab[0]);
	strcpy(datagram->project_name,tab[1]);
	strcpy(datagram->user_name,tab[2]);
	datagram->version = atoi(tab[3]);
	strcpy(datagram->file_path,tab[4]);
	datagram->datagram_number = atoi(tab[5]);
	datagram->datagram_total = atoi(tab[6]);
	datagram->data_length = atoi(tab[7]);
	strcpy(datagram->data,tab[8]);


	free(item);
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
    	unsigned int file_size = ftell(file);
		unsigned int nb_datagrams = (file_size / DATASIZE) + 1;
		unsigned int already_read=0;
		rewind(file);
		unsigned int i;
		data_tab = malloc(nb_datagrams * sizeof(datagram_t));
		for (i=0; i<nb_datagrams; ++i)
		{
			unsigned int mini = MIN(DATASIZE, file_size - already_read);
			
			data_tab[i] = malloc(sizeof(datagram_t));
			data_tab[i]->project_name = malloc(strlen(project_name));
			data_tab[i]->user_name = malloc(strlen(user_name));
			data_tab[i]->file_path = malloc(strlen(file_path));
			data_tab[i]->data = malloc(mini);
			
			fread(data_tab[i]->data, mini, 1, file);
			data_tab[i]->transaction = transaction;
			strcpy(data_tab[i]->project_name, project_name);
		    strcpy(data_tab[i]->user_name, user_name);
		    data_tab[i]->version = version;
		    strcpy(data_tab[i]->file_path, file_path);
		    data_tab[i]->datagram_number = i+1;
		    data_tab[i]->datagram_total = nb_datagrams;

		    int read_bytes = (ftell(file) - already_read);
		    already_read += read_bytes;
		    data_tab[i]->data_length = read_bytes;
			
			printf("[%d] Data:%s\n", i, data_tab[i]->data);
		}
        fclose(file);
    }

    return data_tab;
}


void rebuild_file(char* project_name, char* file_path, datagram_t** tab) 
{
	FILE* file;
	file = fopen(file_path, "w+");
	unsigned int i = 0;
  	for(i=0; tab[i] != NULL; ++i) {
    	fwrite(tab[i]->data, tab[i]->data_length, 1, file);
  	}
  	fclose(file);
}

void free_datagram(datagram_t* datagram)
{
  	free(datagram);
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