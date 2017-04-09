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
	char * res = malloc(12 * sizeof(char) + 4 * DATASIZE);
	char * tmp = malloc(DATASIZE);

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
  	while (item != NULL && i < 9)
  	{
	   	tab[i] = malloc(strlen(item));
  		tab[i] = item;
    	item = strtok(NULL, "¤");
    	++i;
 	}

 	datagram_t* datagram = malloc(sizeof(datagram_t));
	datagram->project_name = malloc(DATASIZE);
	datagram->user_name = malloc(DATASIZE);
	datagram->file_path = malloc(DATASIZE);
	datagram->data = malloc(DATASIZE);

	datagram->transaction = atoi(tab[0]);
	strcpy(datagram->project_name,tab[1]);
	strcpy(datagram->user_name,tab[2]);
	datagram->version = atoi(tab[3]);
	strcpy(datagram->file_path,tab[4]);
	datagram->datagram_number = atoi(tab[5]);
	datagram->datagram_total = atoi(tab[6]);
	datagram->data_length = atoi(tab[7]);
	strcpy(datagram->data,tab[8]);


	return datagram;
}

datagram_t **prepare_file(char* project_name, char* file_path, 
                          transaction_t transaction, unsigned int version,
	                      char* user_name, int on_server) 
{
	FILE* file;
   	datagram_t** data_tab;

   	//Construct real path
   	char * real_path = malloc(DATASIZE);

   	if (on_server == 0)
    {
        strcpy(real_path, "iris/projects/");
        strcat(real_path, project_name);
    	strcat(real_path, "/");
		strcat(real_path, file_path);
    } else {
        strcpy(real_path, "iris-server/projects/");
        strcat(real_path, project_name);
        strcat(real_path, "/r");
    	char* revision = malloc(3);
    	sprintf(revision, "%d", version);
    	strcat(real_path,revision);
        strcat(real_path,"/");
		strcat(real_path, file_path);
    }

	printf(">> Preparing file for sending: %s\n", real_path);
	
    if ((file = fopen(real_path, "r+")) == NULL)
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
			data_tab[i]->project_name = malloc(DATASIZE);
			data_tab[i]->user_name = malloc(DATASIZE);
			data_tab[i]->file_path = malloc(DATASIZE);
			data_tab[i]->data = malloc(DATASIZE);
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


void rebuild_file(char* project_name, char* file_path, unsigned int version, datagram_t** tab, int on_server) 
{
	char * real_path = malloc(DATASIZE);

	if (on_server == 0)
    {
        strcpy(real_path, "iris/projects/");
        strcat(real_path, project_name);
    	strcat(real_path, "/");
		strcat(real_path, file_path);
    } else {
        strcpy(real_path, "iris-server/projects/");
        strcat(real_path, project_name);
        strcat(real_path, "/r");
        char* revision = malloc(3);
        sprintf(revision, "%d", version);
        strcat(real_path,revision);
        strcat(real_path,"/");
		strcat(real_path, file_path);
    }

	printf(">> Rebuilding file from dtagrams: %s\n", real_path);
	FILE* file;
    
    if ((file = fopen(real_path, "w+")) == NULL)
    {
        perror("Error: Cannot create file.");
    } else {
		unsigned int i = 0;
	    file = freopen(real_path,"a",file);
	  	for(i=0; i<tab[0]->datagram_total; ++i) {
		   fwrite(tab[i]->data, tab[i]->data_length, 1, file);
		}
	  	fclose(file);
    }
}



void create_dir(char* dir_path)
{
	//read/write/search permissions for owner and group, read/search for others. 
	int status = mkdir(dir_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	printf(">> MKDIR %s : %d\n", dir_path, status);
}

void clean_dir(char* dir_path)
{
	dirent *entry;
    DIR *directory;
    	
	char * real_path = malloc(DATASIZE);

	strcpy(real_path, dir_path);

	printf(">> Cleaning directory: %s\n", real_path);

    if((directory = opendir(real_path)) == NULL)
    {
        perror("Error: Cannot open \"");
        perror(real_path);
        perror("\" directory.");
        exit(1);
    } else {
        while((entry = readdir(directory)))
        {
       		if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
            {
	       		strcpy(real_path, dir_path);
				strcat(real_path, "/");
				strcat(real_path, entry->d_name);
                if(entry->d_type ==  DT_DIR)
                {
                	char * tmp = malloc(DATASIZE);
                	strcpy(tmp,real_path);
                	clean_dir(real_path);
                	rmdir(tmp);
                } else if(entry->d_type == DT_REG)
                {
		        	remove(real_path);
		        }
		    }
        }
        closedir(directory);
    }
}