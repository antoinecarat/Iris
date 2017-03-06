/*----------------------------------------------
Serveur à lancer avant le client
------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
//compile with -pthread 
#include <pthread.h>
/* pour les sockets */
#include <sys/socket.h>
#include <netdb.h> 
/* pour hostent, servent */
#include <string.h> 
/* pour bcopy, ... */  


#include "fileManager.h"


#define TAILLE_MAX_NOM 256
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

void init()
{
    create_dir("iris-server");
    create_dir("iris-server/projects");
    FILE * file;
    file = fopen("iris-server/.project", "w+");
    fclose(file);
}

void create_project(char* project_name)
{
    char* path;
    strcpy(path, "iris-server/projects/");
    strcat(path, project_name);
    create_dir(path);
    //Add project_name to .projects
    FILE * file;
    file = fopen("iris-server/.project", "a");
    strcat(project_name, "\n");
    fwrite(project_name, sizeof(project_name), 1, file);
    fclose(file);
}

void wait_for_client()
{

}

void *thread_client(void *arg)
{
    /* traitement du message */
    renvoi((int) arg);
    close((int) arg);

    /* Pour enlever le warning */
    (void) arg;
    pthread_exit(NULL);
}

void treat()
{

}

void print_help(){
    printf("Iris is a simple version control system. In this manual you'll find how to use it.\n");
    printf("Usage : iris-server <command> <args>\n");
    printf("\t No command : listen to the network, waiting for client request.\n");
    printf("\t init : Create whole server architecture.\n");
    printf("\t create <project-name> : Create folder and configure project.\n");
    printf("\t help : Show this.\n");
}

int main(int argc, char **argv) {

    char* command = argv[1];
    if (argc == 1)
    {
        //Check if architecture is established.
        //Yes -> wait_for_client()
        //No -> init() and wait_for_client()
    } else if (argc == 2)
    {
        if (strcmp(command, "help") == 0)
        {
            print_help();
        } else if (strcmp(command, "init") == 0)
        {
            init();
        }
    } else if (argc == 3 && (strcmp(command, "create") == 0))
    {
        char* project_name = argv[2];
        strcat(project_name, "\0");
        create_project(project_name);
        //Create project folder and config
    } else
    {
        perror("Usage : iris-server [<command>] [<args>].\n");
        perror("Enter iris-server help to know more.\n");
        exit(1);
    }
    exit(0);
}

/*-------------------------------------------

    // despripteur de socket, (nouveau) descripteur de socket, longueur d'adresse courante d'un client
    int socket_descriptor, nouv_socket_descriptor, longueur_adresse_courante; 
    // structeur d'adresse locale, adresse client courant
    sockaddr_in adresse_locale, adresse_client_courant; 
    // les infos recuperees sur la machine hote
    hostent* ptr_hote;
    // les infos recuperees sur le service de la machine
    servent* ptr_service; 
    // nom de la machine locale
    char machine[TAILLE_MAX_NOM+1]; 
    // recuperation du nom de la machine
    gethostname(machine,TAILLE_MAX_NOM);
    // recuperation de la structure d'adresse en utilisant le nom 
    if ((ptr_hote = gethostbyname(machine)) == NULL) {
        perror("erreur : impossible de trouver le serveur a partir de son nom.");
        exit(1);
    }    
    // initialisation de la structure adresse_locale avec les infos recuperees 
    // copie de ptr_hote vers adresse_locale
    bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale.sin_addr, ptr_hote->h_length);
    adresse_locale.sin_family = ptr_hote->h_addrtype; 
// ou AF_INET 
    adresse_locale.sin_addr.s_addr = INADDR_ANY; 
// ou AF_INET
    // 2 facons de definir le service que l'on va utiliser a distance
    // (commenter l'une ou l'autre des solutions)
    //-------------------------------------------------------
    // SOLUTION 1 : utiliser un service existant, par ex. "irc"
//     if ((ptr_service = getservbyname("irc","tcp")) == NULL) {
// perror("erreur : impossible de recuperer le numero de port du service desire.");
// exit(1);
//     }
//     adresse_locale.sin_port = htons(ptr_service->s_port);
    //-----------------------------------------------------------
    // SOLUTION 2 : utiliser un nouveau numero de port
    adresse_locale.sin_port = htons(5000);
    /-----------------------------------------------------------
    printf("numero de port pour la connexion au serveur : %d \n", 
    ntohs(adresse_locale.sin_port)); //ntohs(ptr_service->s_port)
    // creation de la socket
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("erreur : impossible de creer la socket de connexion avec le client.");
        exit(1);
    }
    // association du socket socket_descriptor à la structure d'adresse adresse_locale
    if ((bind(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
        perror("erreur : impossible de lier la socket a l'adresse de connexion.");
        exit(1);
    }
    // initialisation de la file d'ecoute
    listen(socket_descriptor,5);
    // attente des connexions et traitement des donnees recues
    for(;;) {
        longueur_adresse_courante = sizeof(adresse_client_courant);
        // adresse_client_courant sera renseignée par accept via les infos du connect
        if ((nouv_socket_descriptor = accept(socket_descriptor, (sockaddr*)(&adresse_client_courant), &longueur_adresse_courante)) < 0) {
            perror("erreur : impossible d'accepter la connexion avec le client.");
            exit(1);
        }

        pthread_t thread1;

        if(pthread_create(&thread1, NULL, thread_client, (void*)nouv_socket_descriptor) == -1) {
            perror("pthread_create");
            exit(1);
        }

    }    
}*/



/*------------------------------------------------------*/
void renvoi (int sock) {
    char buffer[256];
    char answer[256] = "KO";
    int longueur;

    FILE * file;
    if((file = fopen("test.txt","w+")) != NULL){
        read(sock, buffer, sizeof(buffer));
        printf("lu : %s", buffer);
        //fwrite(buffer, 1, strlen(buffer), file);
    } else {
        perror("Cannot open file");
    }
    //if ((longueur = read(sock, buffer, sizeof(buffer))) <= 0) 
    //    return;
    printf("\n");
    if (strcmp(buffer, "pull") == 0){
        strcpy(answer,"OK");
        //send
    } else if (strcmp(buffer, "push") == 0){
        strcpy(answer,"OK");
        write(sock,answer,strlen(answer)+1);    
        sleep(6);
        read(sock, buffer, sizeof(buffer));
        printf("lu2 : %s", buffer);
        //ready to receive
    } else {
        strcpy(answer,"Otherwise");
    }
    /* mise en attente du programme pour simuler un delai de transmission */
    sleep(3);
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
