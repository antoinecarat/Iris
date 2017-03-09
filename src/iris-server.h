/**
 * @file iris-server.h
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief Iris server
**/
#ifndef __IRISSERVER_H__
#define __IRISSERVER_H__

#include <pthread.h>
#include <string.h>

 #include "fileManager.h"
 #include "networkManager.h"

/**
 * @brief Create all server architecture.
*/
void init();

/**
 * @brief Create new project.
 * 
 * @param projet Name of the project
*/
void create_project(char* project_name);

/**
 * @brief Listen to network, waiting for client
*/
void wait_for_client();

/**
 * @brief Treat client request.
 * 
 * @param client_socket client socket id.
*/
void treat(int client_socket);

/**
 * @brief Create a new thread for client request.
 * 
 * @param arg client socket id.
*/
void *thread_client(void *arg);

/**
 * @brief Main
*/
//void main(int argc, char **argv);

#endif //__IRISSERVER_H__

