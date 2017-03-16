/**
 * @file networkManager.c
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief all network utilities
 */

#include <stdlib.h>
#include <stdio.h>

#include "networkManager.h"

#define STRINGSIZE 128

int connect_to_server(char* name, unsigned int port) 
{
    int server_socket; 
    sockaddr_in local_adress;
    hostent* ptr_host;

    if ((ptr_host = gethostbyname(name)) == NULL)
    {
        perror("Error: Cannot find server with this adress.\n");
        exit(1);
    }

    bcopy((char*)ptr_host->h_addr, (char*)&local_adress.sin_addr, ptr_host->h_length);
    local_adress.sin_family = AF_INET;
    local_adress.sin_port = htons(port);

    //printf("Server port: %d \n", ntohs(local_adress.sin_port));
    
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: Cannot create socket.");
        exit(1);
    }

    if ((connect(server_socket, (sockaddr*)(&local_adress), sizeof(local_adress))) < 0)
    {
        perror("Error: Cannot connect to server.");
        exit(1);
    }

    return server_socket;
}

int create_server_socket()
{
    int socket_descriptor;
    sockaddr_in adresse_locale;
    hostent* ptr_host;
    char machine[STRINGSIZE+1];

    gethostname(machine,STRINGSIZE);

    if ((ptr_host = gethostbyname(machine)) == NULL)
    {
        perror("Error: Cannot find server.");
        exit(1);
    }

    bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
    adresse_locale.sin_family = ptr_host->h_addrtype; 
    adresse_locale.sin_addr.s_addr = INADDR_ANY; 
    adresse_locale.sin_port = htons(SERVER_PORT);

    printf("%d", ntohs(adresse_locale.sin_port));

    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: Cannot create server socket.");
        exit(1);
    }

    if ((bind(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0)
    {
        perror("Error: Cannot bind socket with adress.");
        exit(1);
    }

	return socket_descriptor;
}

void receive_data()
{
	
}

void send_datagram(int socket, datagram_t * datagram)
{
    char* serial = serialize(datagram);
    printf("Send: %s\n", serial);
    unsigned int size = strlen(serial) * sizeof(char);
    send(socket, serial, size, 0);
}

void send_file(int socket, char* project_name, char* file_path, 
               transaction_t transaction, unsigned int version,
               char* user_name)
{
    datagram_t **data = prepare_file(project_name, file_path, transaction, version, user_name);
	unsigned int i;
	for (i = 0; i < data[0]->datagram_total; ++i)
	{
		send_datagram(socket, data[i]);
	}

    unsigned int j=0;
    for(j=0; data[j] != NULL; ++j) {
        free_datagram(data[j]);
    }
    free(data);
}

void send_dir(int socket, char* project_name, char* dir_path, 
              transaction_t transaction, unsigned int version,
              char* user_name)
{
    char * real_path = malloc(14 + strlen(project_name) + 1 + strlen(dir_path));
    strcpy(real_path, "iris/projects/");
    strcat(real_path, project_name);
    if (strcmp(dir_path, " ") != 0)
    {
        strcat(real_path, "/");
        strcat(real_path, dir_path);
    }
    printf("Sending %s directory.\n", real_path);
    dirent *entry;
    DIR *directory;

    if((directory = opendir(real_path)) == NULL)
    {
        perror("Error: Cannot open \"");
        perror(real_path);
        perror("\" directory.");
        exit(1);
    } else {
        while((entry = readdir(directory)))
        {

            if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..") && strcmp(entry->d_name, ".iris"))
            {
                printf("%s\n", entry->d_name);
                if(entry->d_type ==  DT_DIR) {
                    printf("Folder!\n");

                    datagram_t *datagram = malloc(sizeof(datagram_t));
                    datagram->transaction = MKDIR;
                    datagram->project_name = project_name;
                    datagram->user_name = user_name;
                    datagram->file_path = dir_path;
                    datagram->data = " ";

                    send_datagram(socket, datagram);
                    char * new_dir_path = malloc(strlen(dir_path) + 1 + strlen(entry->d_name));

                    strcpy(new_dir_path, dir_path);
                    strcat(new_dir_path,"/");
                    strcat(new_dir_path, entry->d_name);
                    send_dir(socket, project_name, new_dir_path, transaction, version, user_name);

                    free_datagram(datagram);
                } else if(entry->d_type == DT_REG) {
                    printf("File!\n");
                    char * file_path = malloc(strlen(dir_path) + 1 + strlen(entry->d_name));
                    strcpy(file_path, dir_path);
                    strcat(file_path, "/");
                    strcat(file_path, entry->d_name);
                    printf("file_path: %s\n", file_path);
                    send_file(socket, project_name, file_path, transaction, version, user_name);

                    free(file_path);
                }
            }
        }
        //closedir(directory);
    }

    free(entry);
}