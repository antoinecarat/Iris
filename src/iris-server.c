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
    FILE * file;
    file = fopen("iris-server/.projects", "w+");
    create_dir("iris-server");
    create_dir("iris-server/projects");
    fclose(file);
}

void create_project(char* project_name)
{
    char* path = malloc(DATASIZE);
    strcpy(path, "iris-server/projects/");
    strcat(path, project_name);
    
    DIR *directory;

    if((directory = opendir(path)) == NULL)
    {
        create_dir(path);
        char * r_path = malloc(DATASIZE);
        strcpy(r_path,path);
        strcat(r_path,"/r0");
        create_dir(r_path);
        //Add project_name to .projects
        FILE * file;
        file = fopen("iris-server/.projects", "a");
        strcat(project_name, "\n");
        fwrite(project_name, strlen(project_name), 1, file);
        fclose(file);
    } else {
        closedir(directory);
    }

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
    char * current_file = malloc(DATASIZE);
    datagram_t ** tab = malloc(999*sizeof(datagram_t));
    unsigned int i = 0;
    
    while(recv(client_socket, serial, 12 * sizeof(char) + 4*DATASIZE, 0) > 0)
    {
        //printf("Received: %s\n", serial);
        datagram_t * datagram = unserialize(serial);
        switch(datagram->transaction) {
            case CLONE:
                printf("Clone request...\n");
                break;
            case CREATE:
                printf("Create request...\n");
                printf("%s\n", datagram->file_path);
                create_project(datagram->project_name);
                if (datagram->datagram_number == 1)
                {
                    i=0;
                    strcpy(current_file, datagram->file_path);
                    tab[i] = datagram;
                } else
                {
                    tab[++i] = datagram;
                }
                if (datagram->datagram_number == datagram->datagram_total)
                {
                    datagram_t ** final_tab = malloc(i * sizeof(datagram_t));
                    memcpy(final_tab, tab, i * sizeof(datagram_t));
                    rebuild_file(datagram->project_name, current_file, datagram->version, tab);
                }
                break;
            case PULL:
                printf("Pull request...\n");
                //Construct path and send_dir
                break;

            case PUSH:
                //TODO: Use mutex to avoid collisions between several clients.
                printf("Push request...\n");
                FILE* file;
                char * file_path = malloc(DATASIZE);
                strcpy(file_path,"iris-server/");
                strcat(file_path, datagram->file_path);
                file = fopen(file_path, "a");
                char * real_data = malloc(datagram->data_length);
                memcpy(real_data, datagram->data, datagram->data_length);
                printf("Data: %s\n", real_data);
                fwrite(datagram->data, datagram->data_length, 1, file);
                //Create a new dir (version+1)
                //Receive whole repo into it
                break;

            case REBASE:
                printf("Rebase request...\n");
                break;

            case MKDIR:
                printf("mkdir request...\n");
                printf("%s\n", datagram->file_path);
                char* version = malloc(3);
                sprintf(version, "%d", datagram->version);
                char * real_path = malloc(21 + strlen(datagram->project_name) + 2 + 3 + 1 + strlen(datagram->file_path));
                strcpy(real_path, "iris-server/projects/");
                strcat(real_path, datagram->project_name);
                strcat(real_path,"/r");
                strcat(real_path, version);
                
                if (strcmp(datagram->file_path, " ") != 0)
                {
                    strcat(real_path, "/");
                    strcat(real_path, datagram->file_path);
                }

                create_dir(real_path);
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
            
            init();
            wait_for_client();
        }
    } else
    {
        perror("Usage : iris-server <command> [<args>].\n");
        perror("Enter iris-server help to know more.\n");
        exit(1);
    }
    exit(0);
}
