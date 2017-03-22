/**
 * @file networkManager.h
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief all network utilities
**/
#ifndef __NETWORKMANAGER_H__
#define __NETWORKMANAGER_H__


#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <dirent.h>
#include "fileManager.h"

#define SERVER_PORT 2500

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;
typedef struct dirent dirent;


/**
 * @brief Connect a client to server.
 *
 * @param name Name of the server
 * @param port Port of the server
 *
 * @return Server socket id 
*/
int connect_to_server(char* name, unsigned int port);

/**
 * @brief Create server socket.
 *
 * @return Server socket id 
*/
int create_server_socket();

/**
 * @brief Wait for acquitment
 * 
 * @param socket Socket where to send
*/
void wait_for_ack(int socket);

/**
 * @brief Send a datagram.
 * 
 * @param socket Socket where to send
 * @param datagram Datagram to be sent
*/
void send_datagram(int socket, datagram_t * datagram);


/**
 * @brief Send a file 
 *
 * @param socket Socket where to send
 * @param project_name Name of the project. pouet
 * @param file_path Path of the file to be send. prout/file.truc
 * @param transaction Transaction type.
 * @param version Version of the project.
 * @param user_name Name of the user.
 * @param on_server Where the file is rebuilt : 0=client, 1=server
*/
void send_file(int socket, char* project_name, char* file_path, 
               transaction_t transaction, unsigned int version,
               char* user_name, int on_server);

/**
 * @brief Send the content of a directory 
 *
 * @param socket Socket where to send
 * @param project_name Name of the project. pouet
 * @param file_dir Path of the directory to be send. prout/file.truc
 * @param transaction Transaction type.
 * @param version Version of the project.
 * @param user_name Name of the user.
 * @param on_server Where the file is rebuilt : 0=client, 1=server
*/ 
void send_dir(int socket, char* project_name, char* dir_path, 
              transaction_t transaction, unsigned int version,
              char* user_name, int on_server);

#endif //__NETWORKMANAGER_H__