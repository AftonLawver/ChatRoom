// Afton Lawver
// 851463587

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>

#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define SIZE 1024
#define PORT 12587

void func(int connfd)
{
    char buff[SIZE];
    int n;
    // infinite loop for chat
    for (;;) {
        FILE *pipe;
        int len;
        // clear the contents the in buffer object
        bzero(buff, SIZE);
        // read the message from client and copy it into the buffer
        read(connfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("From client: %s", buff);

        if (system(buff) != 0) {
            write(connfd, "system command does not exist\n", sizeof(buff));
        }
        else {
            if (strncmp("exit", buff, 4) == 0) {
                printf("Server Exit...\n");
                return;
            }
            pipe = popen(buff, "r");
            bzero(buff, SIZE);
            if (pipe == NULL) {
                write(connfd, "Something went wrong with running the command.", sizeof(buff));
            }
            else {
                int i = 0;
                int c = 0;
                while((c=getc(pipe))!=EOF) {
                    buff[i++]=c;
                    buff[i]='\0';
                }
                write(connfd, buff, sizeof(buff));
                pclose(pipe);
            }
        }
    }
}

int main() {
    int pid;
    int new_socket;
    struct sockaddr_in server_address;
    struct sockaddr_in newAddr;
    socklen_t addr_size;
    // create the server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&server_address, sizeof(server_address));
    // define the server address

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind the socket to our specified IP and port #
    int bind_socket = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if (bind_socket != 0) {
        perror("socket bind failed...\n");
        exit(0);
    }
    else
        printf("socket successfully bound to port %d\n", PORT);

    // listen for connections, and make it be able to have 5 clients connected
    int server_listening = listen(server_socket, 5);
    if (server_listening != 0 ) {
        perror("Server listening failed..\n");
        exit(0);
    }
    else
        printf("Server listening..\n");

    printf("Waiting for a connection on port %d\n", PORT);
    while(1) {
        new_socket = accept(server_socket, (struct sockaddr*)&newAddr, &addr_size);

        if (new_socket < 0) {
            perror("Error on accept\n");
            exit(1);
        }
        // Create child process
        pid = fork();
        if (pid < 0) {
            perror("Error on fork");
            exit(1);
        }
        if (pid == 0) {
            printf("Connection accepted from client\n");
            close(server_socket);
            func(new_socket);
            break;
        }
        else {
            close(new_socket);
        }
    }
    close(new_socket);
    exit(0);
    return 0;
}