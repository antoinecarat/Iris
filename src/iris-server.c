/**
 * @file iris-server.c
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief Server's implementation
 */

#include <stdlib.h>
#include <stdio.h>

#include "iris-server.h"

void init()
{
    create_dir("iris-server");
    create_dir("iris-server/projects");
    FILE * file;
    file = fopen("iris-server/.projects", "w+");
    fclose(file);
}

void create_project(char* project_name)
{
    char* path = malloc(DATASIZE);
    strcpy(path, "iris-server/projects/");
    strcat(path, project_name);
    create_dir(path);
    //Add project_name to .projects
    FILE * file;
    file = fopen("iris-server/.project", "a");
    strcat(project_name, "\n");
    fwrite(project_name, sizeof(project_name)+2, 1, file);
    fclose(file);
}

void wait_for_client()
{
    int current_adress_size, client_socket_descriptor;
    sockaddr_in current_client_adress;

    int socket_descriptor = create_server_socket();
    listen(socket_descriptor,5);

    for(;;) {
        current_adress_size = sizeof(current_client_adress);

        if ((client_socket_descriptor = accept(socket_descriptor, (sockaddr*)(&current_client_adress), (socklen_t*)(&current_adress_size))) < 0) {
            perror("Error: Cannot accept client connection.");
        }

        pthread_t thread1;

        if(pthread_create(&thread1, NULL, thread_client, (void*)client_socket_descriptor) == -1) {
            perror("Error: Cannot treat client request.");
        }

    } 
}

void *thread_client(void *arg)
{
    treat((int) arg);
    close((int) arg);

    pthread_exit(NULL);
}

void treat(int client_socket)
{
    char* serial = malloc(12 * sizeof(char) + 4*DATASIZE);
    
    while(recv(client_socket, serial, 12 * sizeof(char) + 4*DATASIZE, 0) > 0) {
        //printf("Received: %s\n", serial);
        printf("Hello.\n");
        datagram_t * datagram = unserialize(serial);
        printf("Is.\n");
        FILE* file;
        char * file_path = malloc(DATASIZE);
        printf("It.\n");
        strcpy(file_path,"iris-server/");
        printf("Me.\n");
        strcat(file_path, datagram->file_path);
        printf("You're.\n");
        file = fopen(file_path, "a");
        printf("Looking for?\n");
        switch(datagram->transaction) {
          case PULL:
            printf("Pull request...\n");
            //Construct path and send_dir
          break;

          case PUSH:
            //TODO: Use mutex to avoid collisions between several clients.
            printf("Push request...\n");
            char * real_data = malloc(datagram->data_length);
            memcpy(real_data, datagram->data, datagram->data_length);
            printf("Data: %s\n", real_data);
            fwrite(datagram->data, datagram->data_length, 1, file);
            //Create a new dir (version+1)
            //Receive whole repo into it
           break;

          case VERSION:
            printf("Version request...\n");
            break;

          default:
            perror("Error: Unknown client request.\n");
            break;
        }
    }
}

void print_help(){
    printf("Iris is a simple version control system. In this manual you'll find how to use it.\n");
    printf("Usage : iris-server <command> <args>\n");
    printf("\t init : Create whole server architecture.\n");
    printf("\t listen : listen to the network, waiting for client request.\n");
    printf("\t create <project-name> : Create folder and configure project.\n");
    printf("\t help : Show this.\n");
}

int main(int argc, char **argv) {

    char* command = argv[1];
    if (argc == 2)
    {
        if (strcmp(command, "help") == 0)
        {
            print_help();
        } else if (strcmp(command, "init") == 0)
        {
            init();
        } else if (strcmp(command, "listen") == 0)
        {
            FILE* file;
            if ((file = fopen("iris-server/.project", "r")) == NULL) //Architecture established ?
            {
                init();
            } else {
                fclose(file);
            }
            wait_for_client();
        }
    } else if (argc == 3 && (strcmp(command, "create") == 0))
    {
        char* project_name = argv[2];
        strcat(project_name, "\0");
        create_project(project_name);
    } else
    {
        perror("Usage : iris-server <command> [<args>].\n");
        perror("Enter iris-server help to know more.\n");
        exit(1);
    }
    exit(0);
}
