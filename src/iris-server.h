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
 * @brief Create whole server architecture.
*/
void init();

/**
 * @brief Refresh .version file.
 * 
 * @param project_name Name of the project
 * @param version The version
 * @param user_name Creator of the project
*/
void update_version(char * project_name, unsigned int version, char * user_name);

/**
 * @brief Retrieve latest version from .version file.
 */
unsigned int get_latest();

/**
 * @brief Create new project.
 * 
 * @param project_name Name of the project
 * @param user_name Creator of the project
*/
void create_project(char* project_name, char * user_name);

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

#endif //__IRISSERVER_H__

