// Afton Lawver
// 851463587

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>


#define SIZE 1024
#define MAX_CLIENTS 5
#define PORT 12587
#define NAME_LEN 32

static _Atomic unsigned int cli_count = 0;
static int uid = 10;

// Client structure
typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[NAME_LEN];
    // insert ip address char *ip;
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

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

void queue_add(client_t *cl) {
    pthread_mutex_lock(&clients_mutex);

    for (int i=0; i<MAX_CLIENTS; i++) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void queue_remove(int uid) {
    pthread_mutex_lock(&clients_mutex);

    for (int i=0; i<MAX_CLIENTS; i++) {
        if (clients[i]) {
            if (clients[i] ->uid == uid) {
                clients[i] = NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_message(char *s, int uid) {
    pthread_mutex_lock(&clients_mutex);

    for(int i=0; i<MAX_CLIENTS; i++) {
        if(clients[i]) {
            if(clients[i]->uid != uid){
                if(write(clients[i]->sockfd, s, strlen(s)) < 0) {
                    printf("ERROR: write to descriptor failed\n");
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void print_ip_address(struct sockaddr_in addr) {
    printf("%d.%d.%d.%d",
           addr.sin_addr.s_addr & 0xff,
           (addr.sin_addr.s_addr & 0xff00) >> 8,
           (addr.sin_addr.s_addr & 0xff0000) >> 16,
           (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

void *handle_client(void *arg) {
    char buffer[SIZE];
    char name[NAME_LEN];
    // flag to tell if client is connected or not
    int leave_flag = 0;
    cli_count++;
    client_t *cli = (client_t*)arg;

    if(recv(cli->sockfd, name, NAME_LEN, 0) <= 0 || strlen(name) < 2 || strlen(name) >= NAME_LEN-1) {
        printf("Enter the name correctly\n");
        leave_flag = 1;
    }
    else {
        strcpy(cli->name, name);
        sprintf(buffer, "%s has joined\n", cli->name);
        printf("%s", buffer);
        send_message(buffer, cli->uid);
    }
    bzero(buffer, SIZE);

    while(1) {
        if(leave_flag) {
            break;
        }
        int receive = recv(cli->sockfd, buffer, SIZE, 0);
        if(receive > 0) {
            if (strlen(buffer) > 0) {
                send_message(buffer, cli->uid);
                str_trim_lf(buffer, strlen(buffer));
                printf("%s\n", buffer);
            }
        }
        else if (receive == 0 || strcmp(buffer, "exit") == 0) {
            sprintf(buffer, "%s has left\n", cli->name);
            printf("%s", buffer);
            send_message(buffer, cli->uid);
            leave_flag = 1;
        }
        else {
            printf("ERROR: -1\n");
            leave_flag = 1;
        }
        bzero(buffer, SIZE);
    }
    close(cli->sockfd);
    queue_remove(cli->uid);
    free(cli);
    cli_count--;
    pthread_detach(pthread_self());
    return NULL;
}

int main() {
    int option = 1;
    int listenfd = 0, connfd = 0;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    pthread_t tid;

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

    if (setsockopt(listenfd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&option, sizeof(option)) < 0) {
        perror("ERROR: setsockopt\n");
        return EXIT_FAILURE;
    }

    if (bind(listenfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("ERROR: bind\n");
        return EXIT_FAILURE;
    }

    if (listen(listenfd, 10) < 0) {
        printf("ERROR: listen\n");
        return EXIT_FAILURE;
    }
    printf("Server listening on port %d\n", PORT);
    printf("=== WELCOME TO THE CHAT ROOM ===\n");

    while(1) {

        socklen_t clilen = sizeof(client_address);
        connfd = accept(listenfd, (struct sockaddr *)&client_address, &clilen);

        // Check for MAX_CLIENTS
        if ((cli_count + 1) == MAX_CLIENTS) {
            printf("Maximum clients connected. Connection Rejected: ");
            print_ip_address(client_address);
            printf(":%d\n", client_address.sin_port);
            close(connfd);
            continue;
        }

        // Client Settings
        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->address = client_address;
        cli->sockfd = connfd;
        cli->uid = uid++;

        // Add client to queue
        queue_add(cli);
        pthread_create(&tid, NULL, &handle_client, (void*)cli);

        // Reduce CPU usage
        sleep(1);
    }
    return EXIT_SUCCESS;
}

