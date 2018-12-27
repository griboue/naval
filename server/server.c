#include <stdlib.h> 
#include <stdio.h> 
#include <string.h>

#include <unistd.h> // disable close() warning
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netinet/in.h> 

#include <pthread.h>
#include <semaphore.h>

#include "server.h" 
#define MAX_CONNECTIONS 5
#define PORT 9002

sem_t lock;

static int client_counter = 0;
int client_sockets[2];

void* place_boats_worker(void* context)
{
    char client_response[256]; 
    int sock = *(int*)context;
    char message[256]  = "instruction: make board";

    memset(client_response, 0, sizeof(client_response)); // clean string
    if (send(sock, message ,strlen(message) , 0) < 0)
    {
        printf("ERROR while sending response to client from worker \n");
    }
    recv(sock, &client_response, sizeof(client_response), 0);
    
    return NULL;
}



int main() 
{ 

    printf("Waiting for incoming connections ...\n");
    sem_init(&lock, 0, 1);

    // socket creation 
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    // dserver address9002
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to IP and port
    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    listen(server_socket, MAX_CONNECTIONS);

    int client_socket;
    int i = 0;
    while(client_counter < 2  && (client_socket = accept(server_socket, NULL ,NULL)))
    {
        client_counter++; 
        client_sockets[i] = client_socket;
        i++;
        printf("new client ! \n");
    }

    printf("\n WE COULD LAUNCH THE GAME BECAUSE 2 PLAYERS ! \n");

    pthread_t thread_id[2];
    for (int i = 0; i < 2; i++)
    {
        pthread_create(&(thread_id[i]), NULL, place_boats_worker, &client_sockets[i]);
    }

    pthread_join(thread_id[0], NULL);
    printf("\n WORKERS 1 TERMINATED \n");
    pthread_join(thread_id[1], NULL);
    printf("\n WORKERS 2 TERMINATED \n");
    
    // system pause to wait for other thread to terminate
    int read;
    scanf("%d", &read);

    close(server_socket);

    return 0; 
} 