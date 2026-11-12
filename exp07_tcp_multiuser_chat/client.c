/*
 * Experiment 7: Multi-User Chat Client Using TCP
 * Course: PCCSL507 - Networks Lab
 * File: client.c
 * Description: Multi-threaded TCP Chat Client that concurrently handles user input
 *              and incoming messages from other participants using POSIX threads.
 *              Displays commands: /help, /users, /msg, /exit.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8082
#define BUFFER_SIZE 2048
#define NAME_LEN 32

static volatile sig_atomic_t flag = 0;
static int sockfd = 0;
static char name[NAME_LEN];

void str_trim_lf(char *arr, int length) {
    for (int i = 0; i < length; i++) {
        if (arr[i] == '\n' || arr[i] == '\r') {
            arr[i] = '\0';
            break;
        }
    }
}

void catch_ctrl_c_and_exit(int sig) {
    (void)sig;
    flag = 1;
}

/* Worker thread to send messages to server */
void *send_msg_handler(void *arg) {
    (void)arg;
    char buffer[BUFFER_SIZE];

    while (!flag) {
        if (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
            str_trim_lf(buffer, BUFFER_SIZE);

            if (strcmp(buffer, "/exit") == 0 || strcmp(buffer, "exit") == 0) {
                send(sockfd, buffer, strlen(buffer), 0);
                flag = 1;
                break;
            } else if (strlen(buffer) > 0) {
                send(sockfd, buffer, strlen(buffer), 0);
            }
        }
    }
    catch_ctrl_c_and_exit(2);
    return NULL;
}

/* Worker thread to receive messages from server */
void *recv_msg_handler(void *arg) {
    (void)arg;
    char buffer[BUFFER_SIZE];

    while (!flag) {
        int receive = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (receive > 0) {
            buffer[receive] = '\0';
            printf("%s", buffer);
            fflush(stdout);
        } else if (receive == 0) {
            printf("\n[INFO] Server closed the connection.\n");
            flag = 1;
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }
    return NULL;
}

int main() {
    signal(SIGINT, catch_ctrl_c_and_exit);

    printf("=====================================================\n");
    printf("        TCP MULTI-USER CHAT CLIENT - EXPERIMENT 7    \n");
    printf("=====================================================\n");

    printf("Enter your username (max %d chars): ", NAME_LEN - 1);
    if (fgets(name, NAME_LEN, stdin) == NULL) {
        strcpy(name, "Anonymous");
    }
    str_trim_lf(name, strlen(name));

    if (strlen(name) < 2) {
        printf("[ERROR] Name must be at least 2 characters long.\n");
        return 1;
    }

    struct sockaddr_in server_addr;

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("[ERROR] Invalid server address");
        return 1;
    }

    // 2. Connect to Server
    printf("[INFO] Connecting to Chat Server at %s:%d...\n", SERVER_IP, PORT);
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[ERROR] Connection failed. Is the server running?");
        return 1;
    }

    // Send username
    send(sockfd, name, strlen(name), 0);

    printf("\n=== Connected to Chat Room as \"%s\" ===\n", name);
    printf("Commands: /help (show help) | /users (list users) | /msg <user> <text> | /exit (leave)\n");
    printf("-----------------------------------------------------\n\n");

    // Spawn Send and Receive Threads
    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, send_msg_handler, NULL) != 0) {
        perror("[ERROR] pthread_create failed");
        return 1;
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, recv_msg_handler, NULL) != 0) {
        perror("[ERROR] pthread_create failed");
        return 1;
    }

    while (1) {
        if (flag) {
            printf("\n[INFO] Exiting chat room. Goodbye!\n");
            break;
        }
        usleep(100000);
    }

    close(sockfd);
    return 0;
}
