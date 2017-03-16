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

    send_dir(server_socket, project_name, " ", CREATE, 0, user_name);
}

void clone(char* project_name, char* server_adress, unsigned int server_port, char* user_name)
{
    int server_socket = connect_to_server(server_adress, server_port);
    FILE* file;
    file = fopen("iris/.projects", "a");
    char * project_name_bis = malloc(strlen(project_name) + 1);
    strcpy(project_name_bis, project_name);
    strcat(project_name_bis, "\n");
    fwrite(project_name_bis, strlen(project_name), 1, file);
    fclose(file);

    char * path = malloc(strlen(project_name) + 14);
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

    free(project_name_bis);
    free(path);
}

void pull(char* project_name, char* server_adress, unsigned int server_port, char* user_name)
{
    int server_socket = connect_to_server(server_adress, server_port);

    char * path = malloc(strlen(project_name) + 14);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    
    datagram_t *datagram = malloc(sizeof(datagram_t));
    datagram->transaction = PULL;
    datagram->project_name = project_name;
    datagram->user_name = user_name;
    datagram->file_path = " ";
    datagram->data = " ";

    send_datagram(server_socket, datagram);
    //receive all the files.

    free_datagram(datagram);
    free(path);
}

void push(char* project_name, char* server_adress, unsigned int server_port, char* user_name)
{
    int server_socket = connect_to_server(server_adress, server_port);

    char * path = malloc(strlen(project_name) + 14);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
 
    //send_dir(path).

    free(path);
}

void rebase(char* project_name, unsigned int version, char* server_adress, unsigned int server_port, char* user_name)
{
    int server_socket = connect_to_server(server_adress, server_port);

    char * path = malloc(strlen(project_name) + 14);
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

    free_datagram(datagram);
    free(path);
}

void add(char* project_name, char* file_path)
{
    char* path = malloc(strlen(project_name) + 25);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    strcat(path, "/.iris/added");
    FILE * file;
    if ((file = fopen(path, "a")) == NULL)
    {
        char * msg = malloc(strlen(project_name) + 61);
        strcpy(msg, "Error: \" ");
        strcat(msg, project_name);
        strcat(msg, "\" not found. Please clone project before notifying anything.\n");
        perror(msg);
        free(msg);
    } else
    {
        strcat(file_path, "\n");
        fwrite(file_path, sizeof(file_path)+2, 1, file);
        fclose(file);
    }

    free(path);
}

void mod(char* project_name, char* file_path)
{
    char* path = malloc(strlen(project_name) + 28);
    strcpy(path, "iris/");
    strcat(path, project_name);
    strcat(path, "/.iris/modified");
    FILE * file;
    if ((file = fopen(path, "a")) == NULL)
    {
        char * msg = malloc(strlen(project_name) + 61);
        strcpy(msg, "Error: \" ");
        strcat(msg, project_name);
        strcat(msg, "\" not found. Please clone project before notifying anything.\n");
        perror(msg);
        free(msg);
    } else
    {
        strcat(file_path, "\n");
        fwrite(file_path, sizeof(file_path)+2, 1, file);
        fclose(file);
    }

    free(path);
}

void del(char* project_name, char* file_path)
{
    char* path = malloc(strlen(project_name) + 27);
    strcpy(path, "iris/");
    strcat(path, project_name);
    strcat(path, "/.iris/removed");
    FILE * file;
    if ((file = fopen(path, "a")) == NULL)
    {
        char * msg = malloc(strlen(project_name) + 61);
        strcpy(msg, "Error: \" ");
        strcat(msg, project_name);
        strcat(msg, "\" not found. Please clone project before notifying anything.\n");
        perror(msg);
        free(msg);
    } else
    {
        strcat(file_path, "\n");
        fwrite(file_path, sizeof(file_path)+2, 1, file);
        fclose(file);
    }
    
    free(path);
}

void status(char* project_name)
{
    printf("%s status :\n", project_name);

    FILE *file;
    int i = 0;
    char *path = malloc(strlen(project_name) + 5);
    char *status = malloc(10*DATASIZE);

    for (i = 0; i < 3; ++i)
    {
        strcpy(path,"iris/");
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
            char c = fgetc(file);
            if (c != EOF)
            {
                printf("%s\t", status);
                printf("%c",c);
                while((c=fgetc(file))!=EOF){
                    printf("%c",c);
                    if (c == '\n')
                    {
                        printf("%s\t", status);
                    }
                }
            }
            fclose(file);
        } else {
            perror("Error : Cannot open file");
            exit(1);
        }
    }
    
    free(path);
    free(status);
}



void print_help(){
    printf("Iris is a simple version control system. In this manual you'll find how to use it.\n");
    printf("Usage : iris <command> <args>\n");
    printf("\t add <file-name> : Notify that a file has been added.\n");
    printf("\t mod <file-name> : Notify that a file has been modified.\n");    
    printf("\t del <file-name> : Notify that a file has been deleted.\n");
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
    {
        if (strcmp(command, "help") == 0)
        {
            print_help();
        } else if (strcmp(command, "init") == 0)
        {
            init();
        }
    } else if (argc == 4)
    {
        char * project_name = argv[2];
        char * file_path = argv[3];

        init();
        
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
    {
        char * project_name = argv[2];
        char * server_adress = argv[3];
        char * server_port = argv[4];
        char * user_name = argv[5];

        init();

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
            int server_socket = connect_to_server(server_adress, atoi(server_port));
            send_file(server_socket, project_name, "testPush.txt", PUSH , 1, user_name);
        }
    }  else if (argc == 7)
    {
        char * project_name = argv[2];
        char * version = argv[3];
        char * server_adress = argv[4];
        char * server_port = argv[5];
        char * user_name = argv[6];

        init();

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