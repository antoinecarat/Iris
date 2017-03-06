/**
 * @file networkManager.c
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief all network utilities
**/

#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "networkManager.h"

int connect_to_server(char* name, unsigned int port) 
{
	return 0;
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