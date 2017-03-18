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


unsigned int get_latest(char * project_name)
{
    char* path = malloc(DATASIZE);
    strcpy(path, "iris-server/projects/");
    strcat(path, project_name);
    
    FILE * file_version;
    char * version_file = malloc(DATASIZE);
    strcpy(version_file, path);
    strcat(version_file, "/.version");
    file_version = fopen(version_file, "r+");
    int ver;
    fscanf(file_version, "%d", &ver);

    return ver;
}

void update_version(char * project_name, unsigned int version, char * user_name)
{
    char* path = malloc(DATASIZE);
    strcpy(path, "iris-server/projects/");
    strcat(path, project_name);

    FILE * file_version;
    char * version_file = malloc(DATASIZE);
    strcpy(version_file, path);
    strcat(version_file, "/.version");
    file_version = fopen(version_file, "w+");
    char * note = malloc(DATASIZE);
    char* v = malloc(3);
    sprintf(v, "%d", version);
    strcpy(note, v);
    strcat(note, "\n");
    strcat(note, user_name);
    fwrite(note, strlen(note), 1, file_version);
    fclose(file_version);
}

void create_project(char* project_name, char * user_name)
{
    char* path = malloc(DATASIZE);
    strcpy(path, "iris-server/projects/");
    strcat(path, project_name);
    
    DIR *directory;

    if((directory = opendir(path)) == NULL)
    {
        //Create project architecture
        create_dir(path);
        char * r_path = malloc(DATASIZE);
        strcpy(r_path,path);
        strcat(r_path,"/r0");
        create_dir(r_path);

        update_version(project_name, 0, user_name);

        // FILE * file_author;
        // char * iris_dir = malloc(DATASIZE);
        // strcpy(iris_dir, r_path);
        // strcat(iris_dir, "/.iris");
        // create_dir(iris_dir);

        // char * author_file = malloc(DATASIZE);
        // strcpy(author_file, iris_dir);
        // strcat(author_file, "/author");
        // file = freopen(author_file, "w+", file);
        // fwrite(user_name, strlen(user_name), 1, file);

        FILE * file_projects;
        file_projects = fopen("iris-server/.projects", "a");
        strcat(project_name, "\n");
        fwrite(project_name, strlen(project_name), 1, file_projects);
        fclose(file_projects);
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
        printf("Received something: %s\n", serial);
        datagram_t * datagram = unserialize(serial);
        unsigned int latest;
        
        switch(datagram->transaction) {
            case CREATE:
                printf("Create request...\n");
                if (strcmp(datagram->file_path, " ") == 0)
                {
                    create_project(datagram->project_name, datagram->user_name);
                } else 
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
                        rebuild_file(datagram->project_name, current_file, 0, tab, 1);
                    }
                }
                printf("Request treated with succes.\n");
                break;
            case CLONE:
            case PULL:
                printf("Clone/Pull request...\n");

                //TODO: check if project exists
                latest = get_latest(datagram->project_name);
                
                send_dir(client_socket, datagram->project_name, " ", datagram->transaction, latest, datagram->user_name, 1);
                break;

            case PUSH:
                //TODO: Use mutex to avoid collisions between several clients.
                printf("Push request...\n");
                latest = get_latest(datagram->project_name);

                if (strcmp(datagram->file_path, " ") == 0)
                {
                    update_version(datagram->project_name, latest + 1, datagram->user_name);
                    latest = get_latest(datagram->project_name);
                    char* r_path = malloc(DATASIZE);
                    strcpy(r_path, "iris-server/projects/");
                    strcat(r_path, datagram->project_name);
                    strcat(r_path, "/r");
                    char* revision = malloc(3);
                    sprintf(revision, "%d", latest);
                    strcat(r_path,revision);
                    create_dir(r_path);
                } else  {
 
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
                            rebuild_file(datagram->project_name, current_file, latest, tab, 1);
                        }
                    }    
                break;

            case REBASE:
                printf("Rebase request...\n");
                break;

            case MKDIR:
                printf("Mkdir request...\n");
                char* version = malloc(3);
                sprintf(version, "%d", latest);
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
    printf("End of reception\n");

}

void print_help()
{
    printf("Iris is a simple version control system. In this manual you'll find how to use it.\n");
    printf("Usage : iris-server <command> <args>\n");
    printf("\t init : Create whole server architecture.\n");
    printf("\t listen : listen to the network, waiting for client request.\n");
    printf("\t create <project-name> : Create folder and configure project.\n");
    printf("\t help : Show this.\n");
}

void list_projects()
{
    FILE *file;
    FILE *version_file;
    char *projects_path = malloc(21);
    char *version_path = malloc(50);
    strcpy(projects_path,"iris-server/.projects");
    strcpy(version_path, "iris-server/projects/");

    printf("Available projects on this server:\n");
    if((file = fopen(projects_path,"r+")) != NULL)
    {
        char * line = malloc(sizeof(DATASIZE));
        while (fscanf(file, "%s\n", line) > 0)
        {   
            //Get version in line/.version file.
            strcat(version_path, line);
            strcat(version_path, "/.version");
            int version;
            char * user = malloc(20);
            if((version_file = fopen(version_path,"r+")) != NULL)
            {
                fscanf(version_file, "%d\n%s", &version, user);
                printf("- %s: Latest version : %d by %s.\n", line, version, user);
            } else
            {
                perror("Error : Cannot open .version file");
                exit(1);
            }
        }
        fclose(file);
    } else {
        perror("Error : Cannot open .projects file");
        exit(1);
    }
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
        } else if (strcmp(command, "list-projects") == 0)
        {
            
            list_projects();
        }
    } else
    {
        perror("Usage : iris-server <command> [<args>].\n");
        perror("Enter iris-server help to know more.\n");
        exit(1);
    }
    exit(0);
}
