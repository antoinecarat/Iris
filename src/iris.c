/**
 * @file iris-server.c
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief Server's implementation
 */

#include <stdlib.h>
#include <stdio.h>

#include "iris.h"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

void init()
{
    create_dir("iris");
    create_dir("iris/projects");
    FILE * file;
    file = fopen("iris/.projects", "w+");
    fclose(file);
}

void create(char* project_name, char* server_adress, unsigned int server_port, char * user_name)
{
    int server_socket = connect_to_server(server_adress, server_port);

    datagram_t *datagram = malloc(sizeof(datagram_t));
    datagram->transaction = CREATE;
    datagram->project_name = project_name;
    datagram->user_name = user_name;
    datagram->file_path = " ";
    datagram->data = " ";

    send_datagram(server_socket, datagram);

    send_dir(server_socket, project_name, " ", CREATE, 0, user_name, 0);
}

void clone(char* project_name, char* server_adress, unsigned int server_port, char* user_name)
{
    int server_socket = connect_to_server(server_adress, server_port);
    FILE* file;
    file = fopen("iris/.projects", "a");
    //char * project_name_bis = malloc(strlen(project_name) + 1);
    char * project_name_bis = malloc(DATASIZE);
    strcpy(project_name_bis, project_name);
    strcat(project_name_bis, "\n");
    fwrite(project_name_bis, strlen(project_name), 1, file);
    fclose(file);

    //char * path = malloc(strlen(project_name) + 14);
    char* path = malloc(DATASIZE);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    create_dir(path);
    
    datagram_t *datagram = malloc(sizeof(datagram_t));
    datagram->transaction = CLONE;
    datagram->project_name = project_name;
    datagram->user_name = user_name;
    datagram->file_path = " ";
    datagram->data = " ";

    send_datagram(server_socket, datagram);

    //receive all the files.


    //free(project_name_bis);
    //free(path);
}

void pull(char* project_name, char* server_adress, unsigned int server_port, char* user_name)
{
    int server_socket = connect_to_server(server_adress, server_port);

    //char * path = malloc(strlen(project_name) + 14);
    char * path = malloc(DATASIZE);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    
    datagram_t *datagram = malloc(sizeof(datagram_t));
    datagram->transaction = PULL;
    datagram->project_name = project_name;
    datagram->user_name = user_name;
    datagram->file_path = " ";
    datagram->data = " ";

    send_datagram(server_socket, datagram);

    clean_dir(path);

    //receive all the files.
    char* serial = malloc(12 * sizeof(char) + 4*DATASIZE);
    char * current_file = malloc(DATASIZE);
    datagram_t ** tab = malloc(999*sizeof(datagram_t));
    unsigned int i = 0;
    
    while(recv(server_socket, serial, 12 * sizeof(char) + 2*DATASIZE, 0) > 0)
    {
        printf(">> Receiving: %s\n", serial);
        datagram_t * datagram = unserialize(serial);
        if (datagram->transaction == PULL)
        {
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
                rebuild_file(datagram->project_name, current_file, datagram->version, tab, 0);
            }
        } else if (datagram->transaction == MKDIR)
        {
            char* version = malloc(3);
            //char * real_path = malloc(21 + strlen(datagram->project_name) + 2 + 3 + 1 + strlen(datagram->file_path));
            char * real_path = malloc(DATASIZE);
            strcpy(real_path, "iris/projects/");
            strcat(real_path, datagram->project_name);
            strcat(real_path, "/");
            strcat(real_path, datagram->file_path);
            create_dir(real_path);
        }
    }

    //free_datagram(datagram);
    //free(path);
}

void push(char* project_name, char* server_adress, unsigned int server_port, char* user_name)
{
    int version = 0;
    int server_socket = connect_to_server(server_adress, server_port);

    datagram_t *datagram = malloc(sizeof(datagram_t));
    datagram->transaction = PUSH;
    datagram->project_name = project_name;
    datagram->user_name = user_name;
    datagram->file_path = " ";
    datagram->data = " ";
    datagram->version = version;
    
    send_datagram(server_socket, datagram);

    send_dir(server_socket, project_name, " ", PUSH, version, user_name, 0);
}

void rebase(char* project_name, unsigned int version, char* server_adress, unsigned int server_port, char* user_name)
{
    int server_socket = connect_to_server(server_adress, server_port);

    //char * path = malloc(strlen(project_name) + 14);
    char * path = malloc(DATASIZE);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    create_dir(path);
    
    datagram_t *datagram = malloc(sizeof(datagram_t));
    datagram->transaction = REBASE;
    datagram->project_name = project_name;
    datagram->user_name = user_name;
    datagram->version = version;
    datagram->file_path = " ";
    datagram->data = " ";

    send_datagram(server_socket, datagram);
    //receive all the files.

    //free_datagram(datagram);
    //free(path);
}

void add(char* project_name, char* file_path)
{
    //char* path = malloc(strlen(project_name) + 25);
    char* path = malloc(DATASIZE);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    strcat(path, "/.iris/added");
    FILE * file;
    if ((file = fopen(path, "a")) == NULL)
    {
        //char * msg = malloc(strlen(project_name) + 61);
        char * msg = malloc(DATASIZE);
        strcpy(msg, "Error: \"");
        strcat(msg, project_name);
        strcat(msg, "\" not found. Please clone project before notifying anything.\n");
        perror(msg);
        //free(msg);
    } else
    {
        strcat(file_path, "\n");
        fwrite(file_path, strlen(file_path), 1, file);
        fclose(file);
    }

    //free(path);
}

void mod(char* project_name, char* file_path)
{
    //char* path = malloc(strlen(project_name) + 28);
    char* path = malloc(DATASIZE);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    strcat(path, "/.iris/modified");
    FILE * file;
    if ((file = fopen(path, "a")) == NULL)
    {
        //char * msg = malloc(strlen(project_name) + 61);
        char * msg = malloc(DATASIZE);
        strcpy(msg, "Error: \"");
        strcat(msg, project_name);
        strcat(msg, "\" not found. Please clone project before notifying anything.\n");
        perror(msg);
        free(msg);
    } else
    {
        strcat(file_path, "\n");
        fwrite(file_path, strlen(file_path), 1, file);
        fclose(file);
    }

    //free(path);
}

void del(char* project_name, char* file_path)
{
    //char* path = malloc(strlen(project_name) + 27);
    char* path = malloc(DATASIZE);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    strcat(path, "/.iris/removed");
    FILE * file;
    if ((file = fopen(path, "a")) == NULL)
    {
        char* msg = malloc(DATASIZE);
//        char * msg = malloc(strlen(project_name) + 61);
        strcpy(msg, "Error: \"");
        strcat(msg, project_name);
        strcat(msg, "\" not found. Please clone project before notifying anything.\n");
        perror(msg);
        //free(msg);
    } else
    {
        strcat(file_path, "\n");
        fwrite(file_path, strlen(file_path), 1, file);
        fclose(file);
    }
    
    //free(path);
}

void status(char* project_name)
{
    printf(">> Status: %s\n", project_name);

    FILE *file;
    int i = 0;
    //char *path = malloc(strlen(project_name) + 29);
    char *path = malloc(DATASIZE);
    char *status = malloc(10*DATASIZE);

    for (i = 0; i < 3; ++i)
    {
        strcpy(path,"iris/projects/");
        strcat(path, project_name);
        if (i == 0)
        {
            strcpy(status, "A");
            strcat(path, "/.iris/added");
        } else if (i == 1)
        {
            strcpy(status, "M");
            strcat(path, "/.iris/modified");
        } else {
            strcpy(status, "D");
            strcat(path, "/.iris/removed");
        }

        if((file = fopen(path,"r+")) != NULL)
        {
            char * line = malloc(sizeof(DATASIZE));
            while (fscanf(file, "%s\n", line) > 0)
            {   
                printf("%s\t", status);
                printf("%s\n", line);
            }
            fclose(file);
        } else {
            perror("Error : Cannot open file");
            exit(1);
        }
    }
    
    //free(path);
    //free(status);
}



void print_help(){
    printf("Iris is a simple version control system. In this manual you'll find how to use it.\n");
    printf("Usage : iris <command> <args>\n");
    printf("\t add <project_name> <file-name> : Notify that a file has been added to project.\n");
    printf("\t mod <project_name> <file-name> : Notify that a file has been modified to project.\n");    
    printf("\t del <project_name> <file-name> : Notify that a file has been deleted to project.\n");
    printf("\t create <project-name> <server-adress> <server-port> <user-name>: Create a new project on server.\n");
    printf("\t clone <project-name> <server-adress> <server-port> <user-name>: Retrieve a project from server for the first time.\n");
    printf("\t pull <project-name> <server-adress> <server-port> <user-name>: Retrieve latest version of the project from the server.\n");
    printf("\t push <project-name> <server-adress> <server-port> <user-name>: Push a new version of the project onto the server.\n");
    printf("\t rebase <project-name> <version> <server-adress> <server-port> <user-name>: Retrieve a specific version of the project from the project.\n");
    printf("\t help : Show this.\n");
}


int main(int argc, char **argv)
{
    char* command = argv[1];

    if (argc == 2)
    {   //init ou help
        if (strcmp(command, "help") == 0)
        {
            print_help();
        } else if (strcmp(command, "init") == 0)
        {
            init();
        }
    } else if (argc == 3)
    {   //status
        char * project_name = argv[2];
        if (strcmp(command, "status") == 0)
        {
            status(project_name);
        }
    } else if (argc == 4)
    {   //add mod del

        char * project_name = argv[2];
        char * file_path = argv[3];
        
        if (strcmp(command, "add") == 0)
        {
            add(project_name, file_path);
        } else if (strcmp(command, "mod") == 0)
        {
            mod(project_name, file_path);
        } else if (strcmp(command, "del") == 0)
        {
            del(project_name, file_path);
        }
    } else if (argc == 6)
    {   //push pull clone create
        char * project_name = argv[2];
        char * server_adress = argv[3];
        char * server_port = argv[4];
        char * user_name = argv[5];

        if (strcmp(command, "create") == 0)
        {
            create(project_name, server_adress, atoi(server_port), user_name);
        } else if (strcmp(command, "clone") == 0)
        {
            clone(project_name, server_adress, atoi(server_port), user_name);
        } else if (strcmp(command, "pull") == 0)
        {
            pull(project_name, server_adress, atoi(server_port), user_name);
        } else if (strcmp(command, "push") == 0)
        {
            push(project_name, server_adress, atoi(server_port), user_name);
        }
    }  else if (argc == 7)
    {   //rebase
        char * project_name = argv[2];
        char * version = argv[3];
        char * server_adress = argv[4];
        char * server_port = argv[5];
        char * user_name = argv[6];

        if (strcmp(command, "rebase") == 0)
        {
            rebase(project_name, atoi(version), server_adress, atoi(server_port), user_name);
        }
    } else {
        perror("Usage : iris <command> <args>.\n");
        perror("Enter iris help to know more.\n");
        exit(1);
    }
    exit(0);  
}