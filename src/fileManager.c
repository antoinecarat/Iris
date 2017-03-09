/**
 * @file fileManager.c
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief all file utilities
 */

#include <stdlib.h>
#include <stdio.h>

#include "fileManager.h"


datagram_t **prepare_file(char* project_name, char* file_path, 
                          transaction_t transaction, unsigned int version,
	                      char* user_name) 
{
	FILE* file;
   	datagram_t** data_tab;
    if (file = fopen(file_path, "r+") == NULL)
    {
        perror("Error: Cannot open file.");
    } else
    {
		unsigned int i = 0;
		while (fread(data_tab[i]->data, sizeof(data_tab[i]->data), 1, file) > 0)
		{
		    data_tab[i] = malloc(sizeof(datagram_t));
		    data_tab[i]->transaction = transaction;
		    strcpy(data_tab[i]->project_name, project_name);
		    strcpy(data_tab[i]->user_name, user_name);
		    strcpy(data_tab[i]->version, version);
		    strcpy(data_tab[i]->file_path, basename(file_path));
		    data_tab[i]->datagram_number = i;
		    int bytes_read = (ftell(file) - (i*sizeof(data_tab[i]->data)));
		    data_tab[i]->data_length = bytes_read;
		    ++i;
		}
		data_tab[i]->datagram_total = i;
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