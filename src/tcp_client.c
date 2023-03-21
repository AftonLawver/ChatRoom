// Afton Lawver
// 851463587

#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>

#include <netinet/in.h>
#include <unistd.h>
#define SIZE 1024
#define PORT 12587

void func(int sockfd)
{
    char buff[SIZE];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("Enter the command: ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        if ((strncmp(buff, "", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
        printf("From Server: %s", buff);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: no IP address provided\n");
        exit(1);
    }
    char *ip_address = argv[1];
    // create a socket
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
    else
        printf("Connected to the server..\n");

    func(network_socket);
    close(network_socket);
    return 0;
}
