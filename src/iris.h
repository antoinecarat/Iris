/**
 * @file iris.h
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief Iris client
**/
#ifndef __IRIS_H__
#define __IRIS_H__


#include <linux/types.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>


/**
 * @brief Retrieving data from remote repository 
 * 
 * @param project_name Name of the project
 * @param server_name Name of the server
 * @param server_port Port of the server
*/
void pull(char* project_name, char* server_name, unsigned int server_port);

/**
 * @brief Retrieving data from remote repository 
 * 
 * @param project_name Name of the project
 * @param user_name Name of the user
 * @param server_name Name of the server
 * @param server_port Port of the server
*/
void push(char* project_name, char* user_name, char* server_name, unsigned int server_port);

/**
 * @brief Add a file to the tracking  
 * 
 * @param project_name Name of the project
 * @param file_path Path of the file to be added 
*/
void add(char* project_name, char* file_path);

/**
 * @brief Remove a file to the tracking
 *
 * @param project_name Name of the project
 * @param file_path Pathe of the file to be removed
*/
void del(char* project_name, char* file_path);


/**
 * @brief Main
*/
//void main(int argc, char **argv);

#endif //__IRIS_H__

