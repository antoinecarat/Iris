/**
 * @file iris-server.c
 * @author M.Cherrueau & A.Carat
 * @since 05/03/2017
 * @brief Server's implementation
 */

#include <stdlib.h>
#include <stdio.h>

#include "iris-server.h"

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void init()
{
    create_dir("iris-server");
    FILE * file;
    file = fopen("iris-server/.projects", "w+");
 
    if (file == NULL){
        perror("Error: Cannot open file.");
    } else {
        create_dir("iris-server/projects");
        fclose(file);
    }
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

        char * iris_dir = malloc(DATASIZE);
        strcpy(iris_dir,r_path);
        strcat(iris_dir,"/.iris");
        create_dir(iris_dir);

        
        char * version_file = malloc(DATASIZE);
        strcpy(version_file,iris_dir);
        strcat(version_file,"/version");

        FILE * file_version = fopen(version_file, "w+");
        
        if (file_version == NULL){
            perror("Error: Cannot open file.");
        } else {
            char * note = malloc(10*DATASIZE);
            strcat(note,"0\n");
            strcat(note,user_name);
            strcat(note,"\n");
            fwrite(note,strlen(note),1,file_version);
            fclose(file_version);
        }

        FILE * file_projects;
        file_projects = fopen("iris-server/.projects", "a");
        strcat(project_name, "\n");
        printf("project_name : %s\n",project_name );
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
        
        if(datagram->transaction != ACK)
        {
            datagram_t *ack;
            ack = malloc(sizeof(datagram_t));
            ack->transaction = ACK;
            ack->project_name = " ";
            ack->user_name = " ";
            ack->file_path = " ";
            ack->data = " ";
            send_datagram(client_socket, ack);
        }

        switch(datagram->transaction) {
            case CREATE:
                printf("Create request...\n");
                latest = 0;
                if (strcmp(datagram->file_path, " ") == 0)
                {    
                    create_project(datagram->project_name, datagram->user_name);
                } else 
                {
                    char * r_path = malloc(DATASIZE);
                    strcpy(r_path,"iris-server/projects/");
                    strcat(r_path,datagram->project_name);
                    strcat(r_path,"/r0/.iris/version");

                    FILE * file_version = fopen(r_path, "a");
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
                        char * note_file = malloc(DATASIZE);
                        strcpy(note_file, "A \t");
                        strcat(note_file, current_file);
                        strcat(note_file, "\n");
                        fwrite(note_file,strlen(note_file),1,file_version);   
                        rebuild_file(datagram->project_name, current_file, 0, tab, 1);
                    }
                    fclose(file_version);
                }
                printf("Request treated with succes.\n");
                break;
            case CLONE:
            case PULL:
                printf("Clone/Pull request...\n");

                latest = get_latest(datagram->project_name);
                
                send_dir(client_socket, datagram->project_name, " ", datagram->transaction, latest, datagram->user_name, 1);
                close(client_socket);
                break;

            case PUSH:
                //Avoid collisions between several clients.
                pthread_mutex_lock (&mutex);

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
                    char* version_path = malloc(DATASIZE);
                    strcpy(version_path, r_path);
                    strcat(version_path, "/.iris");
                    create_dir(r_path);

                    create_dir(version_path);
                    strcat(version_path, "/version");

                    FILE * file_version = fopen(version_path, "w+");
        
                    if (file_version == NULL){
                        perror("Error: Cannot open file.");
                    } else {
                        char * note = malloc(10*DATASIZE);
                        printf("revision : %s\n", revision);
                        strcat(note,revision);
                        strcat(note,"\n");
                        strcat(note,datagram->user_name);
                        strcat(note,"\n");
                        fwrite(note,strlen(note),1,file_version);
                        fclose(file_version);
                    }

                } else {
 
                    char* r_path = malloc(DATASIZE);
                    strcpy(r_path, "iris-server/projects/");
                    strcat(r_path, datagram->project_name);
                    strcat(r_path, "/r");
                    char* revision = malloc(3);
                    sprintf(revision, "%d", latest);
                    strcat(r_path,revision);    
                    
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

                        if(strcmp(current_file,".iris/added") == 0 || strcmp(current_file,".iris/modified") == 0 || strcmp(current_file,".iris/removed") == 0)
                        {

                            char * iris_path = malloc(DATASIZE);
                            strcpy(iris_path,r_path);
                            strcat(iris_path,"/");
                            strcat(iris_path,current_file);                            

                            char* version_path = malloc(DATASIZE);
                            strcpy(version_path, r_path);
                            strcat(version_path, "/.iris/version");
                            
                            char* status = malloc(DATASIZE);
                            if(strcmp(current_file,".iris/added") == 0)
                            {
                                strcpy(status, "A");
                            } else if(strcmp(current_file,".iris/modified") == 0)
                            {
                                strcpy(status, "M");
                            } else if(strcmp(current_file,".iris/removed") == 0)
                            {
                                strcpy(status, "D");
                            }

                            FILE * file_version;
                            if((file_version = fopen(version_path,"a")) != NULL)
                            {
                                FILE * file_iris;
                                if((file_iris = fopen(iris_path,"r+")) != NULL)
                                {
                                    char * line = malloc(DATASIZE);
                                    while (fscanf(file_iris, "%s\n", line) > 0)
                                    {
                                        char* new_line = malloc(DATASIZE);
                                        strcpy(new_line, status);
                                        strcat(new_line, "\t");
                                        strcat(new_line, line);
                                        strcat(new_line, "\n");
                                        fwrite(new_line ,strlen(new_line),1,file_version);
                                    }
                                    fclose(file_iris);
                                    remove(iris_path);
                                } else
                                {
                                    perror("Cannot open file.");
                                }
                                fclose(file_version);
                            } else
                            {
                                perror("Cannot open version file.");
                            }   
                        }                           
                    }
                }
                pthread_mutex_unlock (&mutex);     
                break;

            case REBASE:
                printf("Rebase request...\n");
                break;

            case MKDIR:
                printf("Mkdir request...\n");
                char* version = malloc(3);
                sprintf(version, "%d", latest);
                
                char * real_path = malloc(DATASIZE);
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

    char *projects_path = malloc(DATASIZE);
    char *version_path = malloc(DATASIZE);
    strcpy(projects_path,"iris-server/.projects");

    printf("Available projects on this server:\n");
    if((file = fopen(projects_path,"r+")) != NULL)
    {
        char * line = malloc(DATASIZE);
        while (fscanf(file, "%s\n", line) > 0)
        {   
            strcpy(version_path, "iris-server/projects/");

            //Get version in line/.version file.
            strcat(version_path, line);
            strcat(version_path, "/.version");
            
            int version;
            
            char * user = malloc(DATASIZE);
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
    if (argc == 2 && strcmp(command, "help") == 0)
    {
        print_help();
    } else if (argc == 2 && strcmp(command, "init") == 0)
    {
        init();
    } else if (argc == 2 && strcmp(command, "listen") == 0)
    {
        wait_for_client();
    } else if (argc == 2 && strcmp(command, "list-projects") == 0)
    {    
        list_projects();
    } else
    {
        printf("Usage : iris-server <command> [<args>].\n");
        printf("Enter iris-server help to know more.\n");
        exit(1);
    }
    exit(0);
}
