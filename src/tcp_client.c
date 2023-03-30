// Afton Lawver
// 851463587

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

#define MAX_CLIENTS 5
#define SIZE 1024
#define PORT 12587
#define NAME_LEN 32


volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[NAME_LEN];

void str_overwrite_stdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}

void str_trim_lf(char * arr, int length) {
    for (int i=0; i<length; i++) {
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void recv_msg_handler() {
    char message[SIZE] = {};
    while (1) {
        int receive = recv(sockfd, message, SIZE, 0);
        if (receive > 0) {
            printf("%s", message);
            str_overwrite_stdout();
        } else if (receive == 0) {
            break;
        } else {
            // -1
        }
        memset(message, 0, sizeof(message));
    }
}

void send_msg_handler() {
    char message[SIZE] = {};
    char buffer[SIZE + NAME_LEN] = {};

    while(1) {
        str_overwrite_stdout();
        fgets(message, SIZE, stdin);
        str_trim_lf(message, SIZE);

        if (strcmp(message, "exit") == 0) {
            break;
        } else {
            sprintf(buffer, "%s: %s\n", name, message);
            send(sockfd, buffer, strlen(buffer), 0);
        }

        bzero(message, SIZE);
        bzero(buffer, SIZE + NAME_LEN);
    }
    catch_ctrl_c_and_exit(2);

}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: no IP address provided\n");
        return EXIT_FAILURE;
    }

    signal(SIGINT, catch_ctrl_c_and_exit);
    printf("Please enter your name: ");
    fgets(name, NAME_LEN, stdin);
    str_trim_lf(name, strlen(name));

    if (strlen(name) > NAME_LEN || strlen(name) < 2) {
        printf("Enter the name correctly\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_address;
    // Socket settings
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    int err = connect(sockfd, (struct sockaddr*)&server_address, sizeof(server_address));
    if (err == -1) {
        printf("ERROR: connect\n");
        return EXIT_FAILURE;
    }

    // Send the name
    send(sockfd, name, NAME_LEN, 0);
    printf("=== WELCOME TO THE CHAT ROOM ===\n");

    pthread_t send_msg_thread;
    if(pthread_create(&send_msg_thread, NULL, (void*)send_msg_handler, NULL) != 0){
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    pthread_t recv_msg_thread;
    if(pthread_create(&recv_msg_thread, NULL, (void*)recv_msg_handler, NULL) != 0){
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    while(1) {
        if(flag) {
            printf("\nBye\n");
            break;
        }
    }
    close(sockfd);
    return EXIT_SUCCESS;

}
