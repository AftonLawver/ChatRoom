// Afton Lawver
// 851463587

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#define SIZE 1024
#define PORT 12587

void connection_handler(int sockfd) {
    char buffer[SIZE];
    int n;
    int recv_value = recv(sockfd, buffer, SIZE,0);
    if (recv_value == 0) {
        printf("Client Exit...\n");
        exit(0);
    }
    printf("%s", buffer);

    while (1) {
        bzero(buffer, sizeof(buffer));
        n=0;
        while ((buffer[n++] = getchar()) != '\n')
            ;
        send(sockfd, buffer, SIZE, 0);
        bzero(buffer, sizeof(buffer));
        n = 0;
        int recv_value = recv(sockfd, buffer, sizeof(buffer),0);
        while ((buffer[n++] = getchar()) != '\n')
            ;
        printf("%s", buffer);
        if (recv_value == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: no IP address provided\n");
        exit(1);
    }
    char *ip_address = argv[1];
    int network_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (network_socket == -1) {
        perror("socket creation failed...\n");
        exit(0);
    }
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr(ip_address);

    if (connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address))
        != 0) {
        perror("connection with the server failed...\n");
        exit(0);
    }
    else {
        printf("Connected to the server..\n");
        connection_handler(network_socket);
        close(network_socket);
        return 0;
    }

}
