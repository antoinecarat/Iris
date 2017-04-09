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
    
    char * project_name_bis = malloc(DATASIZE);
    strcpy(project_name_bis, project_name);
    strcat(project_name_bis, "\n");
    fwrite(project_name_bis, strlen(project_name_bis), 1, file);
    fclose(file);

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
    char* serial = malloc(12 * sizeof(char) + 4*DATASIZE);
    char * current_file = malloc(DATASIZE);
    datagram_t ** tab = malloc(999*sizeof(datagram_t));
    unsigned int i = 0;
    
    while(recv(server_socket, serial, 12 * sizeof(char) + 4*DATASIZE, 0) > 0)
    {
        printf(">> Receiving: %s\n", serial);
        datagram_t * datagram = unserialize(serial);

        if(datagram->transaction != ACK)
        {
            datagram_t *ack = malloc(sizeof(datagram_t));
            ack->transaction = ACK;
            ack->project_name = " ";
            ack->user_name = " ";
            ack->file_path = " ";
            ack->data = " ";
            send_datagram(server_socket, ack);
        }

        if (datagram->transaction == CLONE)
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
            char * real_path = malloc(DATASIZE);
            strcpy(real_path, "iris/projects/");
            strcat(real_path, datagram->project_name);
            strcat(real_path, "/");
            strcat(real_path, datagram->file_path);
            create_dir(real_path);
        }
    }

    //Creating notification files
    char * added_file = malloc(DATASIZE);
    char * modified_file = malloc(DATASIZE);
    char * removed_file = malloc(DATASIZE);
    strcpy(added_file, path);
    strcat(added_file, "/.iris/added");
    strcpy(modified_file, path);
    strcat(modified_file, "/.iris/modified");
    strcpy(removed_file, path);
    strcat(removed_file, "/.iris/removed");
    FILE* added = fopen(added_file, "w+");
    FILE* modified = fopen(modified_file, "w+");
    FILE* removed = fopen(removed_file, "w+");

    fwrite("\n",strlen("\n"),1,added);
    fwrite("\n",strlen("\n"),1,modified);
    fwrite("\n",strlen("\n"),1,removed);
    
    fclose(added);
    fclose(modified);
    fclose(removed);

}

void pull(char* project_name, char* server_adress, unsigned int server_port, char* user_name)
{
    char* path = malloc(DATASIZE);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    strcat(path, "/.iris/added");
    FILE * file;
    if ((file = fopen(path, "a")) == NULL)
    {
        printf("Error: \"%s\" not found. Please clone project before trying to push anything.\n", path);
    } else
    {
        fclose(file);

        int server_socket = connect_to_server(server_adress, server_port);

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
        
        while(recv(server_socket, serial, 12 * sizeof(char) + 4*DATASIZE, 0) > 0)
        {
            printf(">> Receiving: %s\n", serial);
            datagram_t * datagram = unserialize(serial);

            if(datagram->transaction != ACK)
            {
                datagram_t *ack = malloc(sizeof(datagram_t));
                ack->transaction = ACK;
                ack->project_name = " ";
                ack->user_name = " ";
                ack->file_path = " ";
                ack->data = " ";
                send_datagram(server_socket, ack);
            }


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
                char * real_path = malloc(DATASIZE);
                strcpy(real_path, "iris/projects/");
                strcat(real_path, datagram->project_name);
                strcat(real_path, "/");
                strcat(real_path, datagram->file_path);
                create_dir(real_path);
            }
        }

        //Creating notification files
        char * added_file = malloc(DATASIZE);
        char * modified_file = malloc(DATASIZE);
        char * removed_file = malloc(DATASIZE);
        strcpy(added_file, path);
        strcat(added_file, "/.iris/added");
        strcpy(modified_file, path);
        strcat(modified_file, "/.iris/modified");
        strcpy(removed_file, path);
        strcat(removed_file, "/.iris/removed");
        FILE* added = fopen(added_file, "w+");
        FILE* modified = fopen(modified_file, "w+");
        FILE* removed = fopen(removed_file, "w+");

        fwrite("\n",strlen("\n"),1,added);
        fwrite("\n",strlen("\n"),1,modified);
        fwrite("\n",strlen("\n"),1,removed);
        
        fclose(added);
        fclose(modified);
        fclose(removed);
    }    
}

void push(char* project_name, char* server_adress, unsigned int server_port, char* user_name)
{
    char* path = malloc(DATASIZE);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    strcat(path, "/.iris/added");
    FILE * file;
    if ((file = fopen(path, "a")) == NULL)
    {
        printf("Error: \"%s\" not found. Please clone project before pushing anything.\n", path);
    } else
    {
        fclose(file);

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
        
        char * path = malloc(DATASIZE);
        strcpy(path,"iris/projects/");
        strcat(path,datagram->project_name);
      
        char * added_file = malloc(DATASIZE);
        char * modified_file = malloc(DATASIZE);
        char * removed_file = malloc(DATASIZE);
        strcpy(added_file, path);
        strcat(added_file, "/.iris/added");
        strcpy(modified_file, path);
        strcat(modified_file, "/.iris/modified");
        strcpy(removed_file, path);
        strcat(removed_file, "/.iris/removed");
      
        remove(added_file);
        remove(modified_file);
        remove(removed_file);

        FILE* added = fopen(added_file, "w+");
        FILE* modified = fopen(modified_file, "w+");
        FILE* removed = fopen(removed_file, "w+");
        
        fclose(added);
        fclose(modified);
        fclose(removed);    
    }


}

void add(char* project_name, char* file_path)
{
    char* path = malloc(DATASIZE);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    strcat(path, "/.iris/added");
    FILE * file;
    if ((file = fopen(path, "a")) == NULL)
    {
        printf("Error: \"%s\" not found. Please clone project before notifying anything.\n", path);
    } else
    {
        strcat(file_path, "\n");
        fwrite(file_path, strlen(file_path), 1, file);
        fclose(file);
    }
}

void mod(char* project_name, char* file_path)
{
    char* path = malloc(DATASIZE);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    strcat(path, "/.iris/modified");
    FILE * file;
    if ((file = fopen(path, "a")) == NULL)
    {
        printf("Error: \"%s\" not found. Please clone project before notifying anything.\n", path);
    } else
    {
        strcat(file_path, "\n");
        fwrite(file_path, strlen(file_path), 1, file);
        fclose(file);
    }
}

void del(char* project_name, char* file_path)
{
    char* path = malloc(DATASIZE);
    strcpy(path, "iris/projects/");
    strcat(path, project_name);
    strcat(path, "/.iris/removed");
    FILE * file;
    if ((file = fopen(path, "a")) == NULL)
    {
        printf("Error: \"%s\" not found. Please clone project before notifying anything.\n", path);
    } else
    {
        strcat(file_path, "\n");
        fwrite(file_path, strlen(file_path), 1, file);
        fclose(file);
    }
}

void status(char* project_name)
{
    printf(">> Status: %s\n", project_name);

    FILE *file;
    int i = 0;
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
            char * line = malloc(DATASIZE);
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
    printf("\t help : Show this.\n");
}


int main(int argc, char **argv)
{
    char* command = argv[1];

    if (argc == 2 && strcmp(command, "help") == 0)
    {   //init or help
        print_help();
    } else if (argc == 2 && strcmp(command, "init") == 0)
    {
        init();
    } else if (argc == 3  && strcmp(command, "status") == 0)
    {   //status
        char * project_name = argv[2];
        status(project_name);
    } else if (argc == 4 && strcmp(command, "add") == 0)
    {   //add mod del
        char * project_name = argv[2];
        char * file_path = argv[3];
        add(project_name, file_path);
    } else if (argc == 4 && strcmp(command, "mod") == 0)
    {   
        char * project_name = argv[2];
        char * file_path = argv[3];
        mod(project_name, file_path);
    } else if (argc == 4 && strcmp(command, "del") == 0)
    {
        char * project_name = argv[2];
        char * file_path = argv[3];
        del(project_name, file_path);
    } else if (argc == 6 && strcmp(command, "create") == 0)
    {   //push pull clone create
        char * project_name = argv[2];
        char * server_adress = argv[3];
        char * server_port = argv[4];
        char * user_name = argv[5];
        create(project_name, server_adress, atoi(server_port), user_name);
    } else if (argc == 6 && strcmp(command, "clone") == 0)
    {
        char * project_name = argv[2];
        char * server_adress = argv[3];
        char * server_port = argv[4];
        char * user_name = argv[5];
        clone(project_name, server_adress, atoi(server_port), user_name);
    } else if (argc == 6 && strcmp(command, "pull") == 0)
    {
        char * project_name = argv[2];
        char * server_adress = argv[3];
        char * server_port = argv[4];
        char * user_name = argv[5];
        pull(project_name, server_adress, atoi(server_port), user_name);
    } else if (argc == 6 && strcmp(command, "push") == 0)
    {
        char * project_name = argv[2];
        char * server_adress = argv[3];
        char * server_port = argv[4];
        char * user_name = argv[5];
        push(project_name, server_adress, atoi(server_port), user_name);
    } else
    {
        printf("Usage : iris <command> <args>.\n");
        printf("Enter iris help to know more.\n");
        exit(1);
    }
    return(0);  
}