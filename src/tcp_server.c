// Afton Lawver
// 851463587

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


#define SIZE 1024
#define PORT 12587

void *connection_handler(void *p_client_socket) {
    int n, client_socket;
    client_socket = *((int*)p_client_socket);
    free(p_client_socket);

    char buffer[SIZE];

    while(1){
        bzero(buffer, SIZE);
        char *msg = "Server: Enter your name:\n";
        send(client_socket, msg, SIZE,0);
        int recv_value = recv(client_socket, buffer, SIZE, 0);
        if (recv_value == 0) {
            printf("Server Exit...\n");
            exit(0);
        }

        char *msg2 = " named ";
        printf("%s %s", msg2, buffer);
        if (recv_value == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    struct sockaddr_in newAddr;
    socklen_t addr_size;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket creation failed...\n");
        exit(0);
    }
    bzero(&server_address, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    int bind_socket = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if (bind_socket != 0) {
        close(server_socket);
        perror("socket bind failed...\n");
        exit(0);
    }

    int server_listening = listen(server_socket, 5);
    if (server_listening != 0 ) {
        perror("Server listening failed..\n");
        exit(0);
    }
    printf("Server listening on port %d\n", PORT);
    while(1) {
        int thread_number;
        client_socket = accept(server_socket, (struct sockaddr*)&newAddr, &addr_size);
        if (client_socket < 0) {
            perror("Error on accept\n");
            exit(1);
        }
        pthread_t t;
        int *pclient = malloc(sizeof(int));
        *pclient = client_socket;
        thread_number = pthread_create(&t, NULL, connection_handler, pclient);
        if(thread_number == 0) {
            printf("Accepted request from IP: %s\n", inet_ntoa(newAddr.sin_addr));
        }
        else {
            printf("ERROR return code from the pthread_create() is %d\n", thread_number);
            break;
        }
    }
    close(client_socket);
    printf("closing connection\n");
}

