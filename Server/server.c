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
#define TAILLE_MAX_NOM 256
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;



void *thread_client(void *arg)
{
    printf("Nouveau client détecté !.\n");

    /* traitement du message */
    printf("reception d'un message.\n");
    renvoi((int) arg);
    close((int) arg);

    /* Pour enlever le warning */
    (void) arg;
    pthread_exit(NULL);
}


/*------------------------------------------------------*/
void renvoi (int sock) {
    char buffer[256];
    char answer[256];
    int longueur;
    if ((longueur = read(sock, buffer, sizeof(buffer))) <= 0) 
        return;
    printf("message lu : %s \n", buffer);

    if (strcmp(buffer, "init") == 0){
        //clone ?
    } else if (strcmp(buffer, "pull") == 0){
        strcpy(answer,"Sending files");
        //send
    } else if (strcmp(buffer, "push") == 0){
        strcpy(answer,"Waiting for files");
        //recv
    } else {
       strcpy(answer,"Pouet");
    }
    printf("message apres traitement : %s \n", answer);  
    printf("renvoi du message traite.\n");
    /* mise en attente du programme pour simuler un delai de transmission */
    sleep(3);
    write(sock,answer,strlen(answer)+1);    
    printf("message envoye. \n");       
    return;
}
/*------------------------------------------------------*/
/*------------------------------------------------------*/
main(int argc, char **argv) {
    // despripteur de socket, (nouveau) descripteur de socket, longueur d'adresse courante d'un client
    int socket_descriptor, nouv_socket_descriptor, longueur_adresse_courante; 
    // structeur d'adresse locale, adresse client courant
    sockaddr_in adresse_locale, adresse_client_courant; 
    // les infos recuperees sur la machine hote
    hostent* ptr_hote;
    // les infos recuperees sur le service de la machine */
    servent* ptr_service; 
    // nom de la machine locale */
    char machine[TAILLE_MAX_NOM+1]; 
    // recuperation du nom de la machine */
    gethostname(machine,TAILLE_MAX_NOM);
    /* recuperation de la structure d'adresse en utilisant le nom */
    if ((ptr_hote = gethostbyname(machine)) == NULL) {
        perror("erreur : impossible de trouver le serveur a partir de son nom.");
        exit(1);
    }    
    /* initialisation de la structure adresse_locale avec les infos recuperees */
    /* copie de ptr_hote vers adresse_locale */
    bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale.sin_addr, ptr_hote->h_length);
    adresse_locale.sin_family = ptr_hote->h_addrtype; 
/* ou AF_INET */
    adresse_locale.sin_addr.s_addr = INADDR_ANY; 
/* ou AF_INET */
    /* 2 facons de definir le service que l'on va utiliser a distance */
    /* (commenter l'une ou l'autre des solutions) */
    /*-----------------------------------------------------------*/
    /* SOLUTION 1 : utiliser un service existant, par ex. "irc" */
    /*
    if ((ptr_service = getservbyname("irc","tcp")) == NULL) {
perror("erreur : impossible de recuperer le numero de port du service desire.");
exit(1);
    }
    adresse_locale.sin_port = htons(ptr_service->s_port);
    /*-----------------------------------------------------------*/
    /* SOLUTION 2 : utiliser un nouveau numero de port */
    
    adresse_locale.sin_port = htons(5000);
    /*-----------------------------------------------------------*/
    printf("numero de port pour la connexion au serveur : %d \n", 
    ntohs(adresse_locale.sin_port) /*ntohs(ptr_service->s_port)*/);
    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("erreur : impossible de creer la socket de connexion avec le client.");
        exit(1);
    }
    /* association du socket socket_descriptor à la structure d'adresse adresse_locale */
    if ((bind(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
        perror("erreur : impossible de lier la socket a l'adresse de connexion.");
        exit(1);
    }
    /* initialisation de la file d'ecoute */
    listen(socket_descriptor,5);
    /* attente des connexions et traitement des donnees recues */
    for(;;) {
        longueur_adresse_courante = sizeof(adresse_client_courant);
        /* adresse_client_courant sera renseignée par accept via les infos du connect */
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
}
