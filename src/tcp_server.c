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
#include <signal.h>


#define SIZE 1024
#define MAX_CLIENTS 0
#define PORT 12587

static _Atomic unsigned int cli_count = 0;
static int uid = 10;

print_ip_address(struct sockaddr_in addr) {
    printf("%d.%d.%d.%d",
           addr.sin_addr.s_addr & 0xff,
           (addr.sin_addr.s_addr & 0xff00) >> 8,
           (addr.sin_addr.s_addr & 0xff0000) >> 16,
           (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

//void *connection_handler(void *p_client_socket) {
//    int n, client_socket;
//    client_socket = *((int*)p_client_socket);
//    free(p_client_socket);
//
//    char buffer[SIZE];
//
//    while(1){
//        bzero(buffer, SIZE);
//        char *msg = "Server: Enter your name:\n";
//        send(client_socket, msg, SIZE,0);
//        int recv_value = recv(client_socket, buffer, SIZE, 0);
//        if (recv_value == 0) {
//            printf("Server Exit...\n");
//            exit(0);
//        }
//
//        char *msg2 = " named ";
//        printf("%s %s", msg2, buffer);
//        if (recv_value == 0) {
//            printf("Server Exit...\n");
//            break;
//        }
//    }
//}

int main() {
    int option = 1;
    int listenfd = 0, connfd = 0;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    pthread_t tid;
//    socklen_t addr_size;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket creation failed...\n");
        exit(0);
    }

    // Socket settings
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    signal(SIGPIPE, SIG_IGN);

    if (setsockopt(listenfd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&option, sizeof(option < 0))) {
        printf("ERROR: setsockopt\n");
        return EXIT_FAILURE;
    }

    if (bind(listenfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("ERROR: bind\n");
    }

    if (listen(listenfd, 10) < 0) {
        printf("ERROR: listen\n");
    }
    printf("Server listening on port %d\n", PORT);

    printf("=== WELCOME TO THE CHAT ROOM ===\n");

    while(1) {

        socklen_t clilen = sizeof(client_address);
        connfd = accept(listenfd, (struct sockaddr *) &client_address, clilen);

        // Check for MAX_CLIENTS
        if ((cli_count + 1) == MAX_CLIENTS) {
            printf("Maximum clients connected. Connection Rejected: ");
            print_ip_address(client_address);
            close(connfd);
            continue;
        }
    }

//        int thread_number;
//        client_socket = accept(server_socket, (struct sockaddr*)&newAddr, &addr_size);
//        if (client_socket < 0) {
//            perror("Error on accept\n");
//            exit(1);
//        }
//        pthread_t t;
//        int *pclient = malloc(sizeof(int));
//        *pclient = client_socket;
//        thread_number = pthread_create(&t, NULL, connection_handler, pclient);
//        if(thread_number == 0) {
//            printf("Accepted request from IP: %s\n", inet_ntoa(newAddr.sin_addr));
//        }
//        else {
//            printf("ERROR return code from the pthread_create() is %d\n", thread_number);
//            break;
//        }
//    }
//    close(client_socket);
//    printf("closing connection\n");
}

