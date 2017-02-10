#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

void print_help(){
	printf("Iris is a simple version control system. In this manual you'll find how to use it.\n");
	printf("Usage : iris-client <command> <args>\n");
	printf("Commands\n");
}

hostent* getHost(){
	char host[1000];
	hostent * ptr_host; 
	FILE* fichier;
    if ((fichier = fopen(".iris", "r")) != NULL)
    {
        fgets(host, 1000, fichier);
        printf("adresse du serveur  : %s \n", host);
        fclose(fichier);
    } else {
        perror("erreur : Impossible d'ouvrir le fichier de configuration .iris");
    }

	if ((ptr_host = gethostbyname(host)) == NULL) {
        perror("erreur : impossible de trouver le serveur a partir de son adresse.");
        exit(1);
    }

    return ptr_host;
}

void say(char* mesg){
	// descripteur de socket, longueur d'un buffer utilisé
    int socket_descriptor, longueur; 

    /* adresse de socket local */
    sockaddr_in adresse_locale; 
    /* info sur une machine hote */
    hostent * ptr_host; 
    /* info sur service */
    servent * ptr_service; 
    /* nom du programme */
    char buffer[256];
    char * prog; 
    /* nom de la machine distante */



    ptr_host = getHost();
    /* copie caractere par caractere des infos de ptr_host vers adresse_locale */
    bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
    adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */
    adresse_locale.sin_port = htons(5000);
    /*-----------------------------------------------------------*/
    printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));
    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("erreur : impossible de creer la socket de connexion avec le serveur.");
        exit(1);
    }
    /* tentative de connexion au serveur dont les infos sont dans adresse_locale */
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
        perror("erreur : impossible de se connecter au serveur.");
        exit(1);
    }
    printf("connexion etablie avec le serveur. \n");
    printf("envoi d'un message au serveur. \n");
    printf("message envoye      : %s \n", mesg);
    
    /* envoi du message vers le serveur */
    if ((write(socket_descriptor, mesg, strlen(mesg))) < 0) {
        perror("erreur : impossible d'ecrire le message destine au serveur.");
        exit(1);
    }
    /* mise en attente du prgramme pour simuler un delai de transmission */
    sleep(3);
    printf("message envoye au serveur. \n");

    /* lecture de la reponse en provenance du serveur */
    while((longueur = read(socket_descriptor, buffer, sizeof(buffer))) > 0) {
        printf("reponse du serveur : \n");
        write(1,buffer,longueur);
    }
    printf("\nfin de la reception.\n");
    close(socket_descriptor);
    printf("connexion avec le serveur fermee, fin du programme.\n");
}

int main(int argc, char **argv) {
	if (argc < 2) {
        perror("Usage : iris-client <command> <args>");
        exit(1);
    }
    char* command = argv[1];
    if (strcmp(command, "init") == 0){
    	say("Init");
    	//clone ?
    } else if (strcmp(command, "pull") == 0){
		say("Pull");
		//reicv
   	} else if (strcmp(command, "push") == 0){
    	say("Push");
    	//send
   	} else {
   		printf("%s : unknown command, please read the following :\n", command);
    	print_help();
    }

    exit(0);  
}