#include <stdio.h> 
#include <stdlib.h> 

#include <unistd.h> // disable close() warning
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>


#include "client.h" 


int main(int argc, char const *argv[])
{
    
    show_menu();
    proceed_selection();
    

    return 0;
}

void show_menu()
{
    printf("Please select an option: \n\n");
    printf("1) Connect to a server \n");
    printf("2) Create a server \n");
    printf("3) Play against the machine \n\n");
}

void proceed_selection()
{
    int choice;
    scanf("%d",&choice);

    if (choice == 1)
    {
        connect_to_server();
    }
    else if (choice == 2)
    {
        printf("\n You selected option 2 \n");
    }
    else if (choice == 3)
    {
        printf("\n You selected option 2 \n");
    }
}

int socket_connection(char* ip, int port)
{
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // ajout d'une adresse à la socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);
    int connection_status = connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    // check si aucun probleme lors de la connexion
    if (connection_status == -1)
    {
        printf("Error while creating / connecting the socket");
    }

    return network_socket;
}

void connect_to_server()
{
    char ip[256];
    int port;

    // printf("\n Enter the IP address of the server please: ");
    // scanf("%s", ip);

    // printf("\n Enter the port number of the server please: ");
    // scanf("%d", &port);
    strcpy(ip, "127.0.0.1");
    port = 9002;



    // try to connect to the server with the IP and port
    int network_socket = socket_connection(ip, port);
    
    char server_reponse[256];
    char message[256];
    
  
    // clean string
    memset(server_reponse, 0, sizeof(server_reponse)); 
    memset(message, 0, sizeof(message)); 




    recv(network_socket, &server_reponse, sizeof(server_reponse), 0);
    // affiche la réponse du serveur
    if (strcmp(server_reponse, "instruction: make board") == 0)
    {
        // make_board(); TO CONTINUE ......
    }
    

}



  
int lol(int argc, char const *argv[]) 
{ 
    // creation de la socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // ajout d'une adresse à la socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connection_status = connect(network_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    // check si aucun probleme lors de la connexion
    if (connection_status == -1)
    {
        printf("There was an error making a connection to the remote socket \n\n");
    }

    // recevoir donnés depuis le serveur
    char server_reponse[256];
    char message[256];

    while(1)
    {

        printf("Enter message : \n");

		scanf("%s" , message);

        printf("you entered %s \n", message);

        //Send some data
		if(send(network_socket , message , strlen(message) , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
        memset(server_reponse, 0, sizeof(server_reponse)); // clean string
        recv(network_socket, &server_reponse, sizeof(server_reponse), 0);
        // affiche la réponse du serveur
        printf("The server sent: %s \n \n", server_reponse);
        
    }



    
    // ferme la connexion
    close(network_socket);


    return 0; 
} 