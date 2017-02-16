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

/*void sendFile(char * filePath){
    char *fileBuffer = (char *) malloc(256);
    FILE * file;

    if ((file = fopen(filePath, "r")) != NULL)
    {
        //Read file contents into buffer
        //size_t nbytes = 0;
        //int offset = 0;
        //int sent;

        int i=0;
        char c;
        while ((c = fgetc(file)) != EOF && i<255)
        {
            fileBuffer[i] = (char) c;
            ++i;
        }

        // don't forget to terminate with the null character
        fileBuffer[255] = '\0';  

        say(fileBuffer);
    } else {
        perror("erreur : Impossible d'ouvrir le fichier de configuration .iris");
    }

    // while ((sent = send(client, fileBuffer + offset, nbytes, 0)) > 0) {
    //         offset += sent;
    //         nbytes -= sent;
    //         printf("%s\n", fileBuffer);
    // }
}*/

/*void say(char* mesg){
	// descripteur de socket, longueur d'un buffer utilisé
    int socket_descriptor, longueur; 

    // adresse de socket local
    sockaddr_in adresse_locale; 
    // info sur une machine hote
    hostent * ptr_host; 
    // info sur service
    servent * ptr_service; 
    // nom du programme
    char buffer[256];
    char * prog; 
    // nom de la machine distante



    ptr_host = getHost();
    // copie caractere par caractere des infos de ptr_host vers adresse_locale
    bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
    adresse_locale.sin_family = AF_INET;
    adresse_locale.sin_port = htons(5000);
    
    printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));
    // creation de la socket
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("erreur : impossible de creer la socket de connexion avec le serveur.");
        exit(1);
    }
    // tentative de connexion au serveur dont les infos sont dans adresse_locale
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
        perror("erreur : impossible de se connecter au serveur.");
        exit(1);
    }
    printf("connexion etablie avec le serveur. \n");
    printf("envoi d'un message au serveur. \n");
    printf("message envoye      : %s \n", mesg);
    
    // envoi du message vers le serveur
    if ((write(socket_descriptor, mesg, strlen(mesg))) < 0) {
        perror("erreur : impossible d'ecrire le message destine au serveur.");
        exit(1);
    }
    // mise en attente du prgramme pour simuler un delai de transmission
    sleep(3);
    printf("message envoye au serveur. \n");

    // lecture de la reponse en provenance du serveur 
    while((longueur = read(socket_descriptor, buffer, sizeof(buffer))) > 0) {
        printf("reponse du serveur : \n");
        write(1,buffer,longueur);
    }
    printf("\nfin de la reception.\n");
    close(socket_descriptor);
    printf("connexion avec le serveur fermee, fin du programme.\n");
}*/


hostent* getHost(){
    char host[16];
    hostent * ptr_host; 
    FILE* conf_file;
    if ((conf_file = fopen(".iris", "r")) != NULL)
    {
        fgets(host, 16, conf_file);
        printf("Server adress: %s .\n", host);
        fclose(conf_file);
    } else {
        perror("Error: Cannot open .iris configuration file.\n");
        //Ask user server adress?
        exit(1);
    }

    if ((ptr_host = gethostbyname(host)) == NULL) {
        perror("Error: Cannot find server with this adress.\n");
        exit(1);
    }

    return ptr_host;
}

int connect_to_serv()
{
    //server socket
    int server_socket; 
    //local socket adress
    sockaddr_in local_adress;

    hostent* ptr_host = getHost();

    //copy ptr_host informations into local_adress
    bcopy((char*)ptr_host->h_addr, (char*)&local_adress.sin_addr, ptr_host->h_length);
    local_adress.sin_family = AF_INET;
    local_adress.sin_port = htons(5000);

    printf("Server port: %d \n", ntohs(local_adress.sin_port));
    
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error: Cannot create socket.");
        exit(1);
    }
    if ((connect(server_socket, (sockaddr*)(&local_adress), sizeof(local_adress))) < 0) {
        perror("Error: Cannot connect to server.");
        exit(1);
    }
    printf("Connected. \n");
    return server_socket;
}

void send_message(int server_socket, char * message)
{
    printf("Sending a message to server:\n%s \n", message);
    
    if ((write(server_socket, message, strlen(message))) < 0) {
        perror("Error: Cannot send message to server.");
        exit(1);
    }
    //Transmission delay
    sleep(3);
    printf("Sent. \n");
}

void send_file(int server_socket, char * file_path)
{
    char *fileBuffer = (char *) malloc(256);
    FILE * file;

    if ((file = fopen(file_path, "r")) != NULL)
    {
        //Read file contents into buffer
        //size_t nbytes = 0;
        //int offset = 0;
        //int sent;
        int i;
        char c;
        for (i = 0; i < 255; ++i)
        {
            c = fgetc(file);
            fileBuffer[i] = (char) c;
        }
        // don't forget to terminate with the null character
        fileBuffer[255] = '\0';  

        send_message(server_socket, fileBuffer);
        fclose(file);
    } else {
        perror("Error: Cannot open file.");
    }
}

char * receive_message(int server_socket)
{
    char * answer;
    int a_length;

    while((a_length = read(server_socket, answer, sizeof(answer))) > 0) {
        printf("Answer: %s \n", answer);
        write(1,answer,a_length);
    }

    return answer;
}

int main(int argc, char **argv)
{
	if (argc < 2)
    {
        perror("Usage : iris-client <command> <args>");
        exit(1);
    }
    //connect to server via .iris file
    int server_socket = connect_to_serv();
    //read and execute command
    char* command = argv[1];
    if (strcmp(command, "help") == 0)
    {
       print_help(); 
    } else if (strcmp(command, "pull") == 0)
    {
        send_message(server_socket, "pull");
        char * answer = receive_message(server_socket);
        if (strcmp(answer, "OK")){
            //Receive files.
        } else {
            perror("Server refused command.");
            exit(1);
        }
   	} else if (strcmp(command, "push") == 0)
    {
    	send_message(server_socket, "push");
        char * answer = receive_message(server_socket);
        printf("%s\n", answer);
        if (strcmp(answer, "OK") == 0){
            send_file(server_socket, "test.txt");
        } else {
            perror("Server refused command.");
            exit(1);
        }
   	} else
    {
   		printf("%s : unknown command, please read the following :\n", command);
    	print_help();
    }
    close(server_socket);
    printf("Connection closed.\n");
    exit(0);  
}