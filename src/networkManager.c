/**
 * @file networkManager.c
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief all network utilities
 */

#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include "networkManager.h"

#define STRINGSIZE 256

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
    hostent* ptr_hote;
    char machine[STRINGSIZE+1];

    gethostname(machine,STRINGSIZE);

    if ((ptr_hote = gethostbyname(machine)) == NULL)
    {
        perror("Error: Cannot find server.");
        exit(1);
    }

    bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale.sin_addr, ptr_hote->h_length);
    adresse_locale.sin_family = ptr_hote->h_addrtype; 
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

void send_datagram(int socket, char * datagram)
{

}

void send_file(char* project_name, char* file_path, 
               transaction_t transaction, unsigned int version,
               char* user_name)
{

}

void send_dir(char* project_name, char* dir_path, 
              transaction_t transaction, unsigned int version,
              char* user_name)
{
	
}