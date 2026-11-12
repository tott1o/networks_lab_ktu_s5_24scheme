/*
 * Experiment 7: Multi-User Chat Server Using TCP
 * Course: PCCSL507 - Networks Lab
 * File: server.c
 * Description: Multi-threaded TCP Chat Server that manages multiple concurrent
 *              client connections, relays broadcast messages, and supports commands:
 *              /help, /users (list active users), /msg <target> <text>, and /exit.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8082
#define MAX_CLIENTS 50
#define BUFFER_SIZE 2048
#define NAME_LEN 32

typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[NAME_LEN];
} client_t;

static client_t *clients[MAX_CLIENTS];
static pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
static int uid_counter = 10;

/* Add client to global registry */
void queue_add(client_t *cl) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Remove client from global registry */
void queue_remove(int uid) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->uid == uid) {
            clients[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Broadcast message to all active clients except sender */
void broadcast_message(const char *message, int sender_uid) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->uid != sender_uid) {
            send(clients[i]->sockfd, message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Send private message to specific user */
int send_private_message(const char *target_name, const char *msg, client_t *sender) {
    char pmsg[BUFFER_SIZE + 64];
    int found = 0;

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && strcmp(clients[i]->name, target_name) == 0) {
            snprintf(pmsg, sizeof(pmsg), "[WHISPER from %s]: %s\n", sender->name, msg);
            send(clients[i]->sockfd, pmsg, strlen(pmsg), 0);
            found = 1;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    if (found) {
        snprintf(pmsg, sizeof(pmsg), "[WHISPER to %s]: %s\n", target_name, msg);
        send(sender->sockfd, pmsg, strlen(pmsg), 0);
    }
    return found;
}

/* Worker thread routine for each connected client */
void *handle_client(void *arg) {
    char buffer[BUFFER_SIZE];
    char send_buffer[BUFFER_SIZE + 64];
    int leave_flag = 0;

    client_t *cli = (client_t *)arg;

    // Receive username
    if (recv(cli->sockfd, cli->name, NAME_LEN, 0) <= 0 || strlen(cli->name) < 2) {
        printf("[WARNING] Client failed to provide valid username.\n");
        leave_flag = 1;
    } else {
        cli->name[strcspn(cli->name, "\r\n")] = '\0';
        snprintf(send_buffer, sizeof(send_buffer), ">>> [SERVER]: \"%s\" has joined the chat room <<<\n", cli->name);
        printf("[JOIN] %s", send_buffer);
        broadcast_message(send_buffer, cli->uid);

        const char *welcome = "Welcome to the chat room! Type /help for commands or /exit to quit.\n";
        send(cli->sockfd, welcome, strlen(welcome), 0);
    }

    memset(buffer, 0, BUFFER_SIZE);

    // Chat message loop
    while (!leave_flag) {
        int receive = recv(cli->sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (receive > 0) {
            buffer[receive] = '\0';
            buffer[strcspn(buffer, "\r\n")] = '\0';

            // Command: /exit
            if (strcmp(buffer, "/exit") == 0 || strcmp(buffer, "exit") == 0) {
                snprintf(send_buffer, sizeof(send_buffer), ">>> [SERVER]: \"%s\" has left the chat room <<<\n", cli->name);
                printf("[LEAVE] %s", send_buffer);
                broadcast_message(send_buffer, cli->uid);
                leave_flag = 1;
            }
            // Command: /help
            else if (strcmp(buffer, "/help") == 0) {
                const char *help_msg =
                    "--- Chat Room Commands ---\n"
                    "  /users               - List active online participants\n"
                    "  /msg <user> <text>   - Send private whisper to user\n"
                    "  /help                - Show this command guide\n"
                    "  /exit                - Disconnect from chat room\n"
                    "--------------------------\n";
                send(cli->sockfd, help_msg, strlen(help_msg), 0);
            }
            // Command: /users
            else if (strcmp(buffer, "/users") == 0 || strcmp(buffer, "/list") == 0) {
                char user_list[BUFFER_SIZE];
                snprintf(user_list, sizeof(user_list), "--- Active Online Users ---\n");
                pthread_mutex_lock(&clients_mutex);
                int count = 0;
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i]) {
                        count++;
                        char entry[64];
                        snprintf(entry, sizeof(entry), "  %d. %s%s\n", count, clients[i]->name,
                                 (clients[i]->uid == cli->uid) ? " (You)" : "");
                        strcat(user_list, entry);
                    }
                }
                pthread_mutex_unlock(&clients_mutex);
                strcat(user_list, "---------------------------\n");
                send(cli->sockfd, user_list, strlen(user_list), 0);
            }
            // Command: /msg <target> <message>
            else if (strncmp(buffer, "/msg ", 5) == 0) {
                char *target = buffer + 5;
                while (*target == ' ') target++;
                char *msg = strchr(target, ' ');
                if (msg) {
                    *msg = '\0';
                    msg++;
                    while (*msg == ' ') msg++;
                    if (!send_private_message(target, msg, cli)) {
                        snprintf(send_buffer, sizeof(send_buffer), "[SERVER ERROR]: User '%s' not found online.\n", target);
                        send(cli->sockfd, send_buffer, strlen(send_buffer), 0);
                    }
                } else {
                    const char *usage = "[USAGE]: /msg <username> <message>\n";
                    send(cli->sockfd, usage, strlen(usage), 0);
                }
            }
            // Standard Broadcast Message
            else if (strlen(buffer) > 0) {
                snprintf(send_buffer, sizeof(send_buffer), "[%s]: %s\n", cli->name, buffer);
                printf("[MESSAGE] %s", send_buffer);
                broadcast_message(send_buffer, cli->uid);
            }
        } else if (receive == 0 || receive < 0) {
            snprintf(send_buffer, sizeof(send_buffer), ">>> [SERVER]: \"%s\" disconnected <<<\n", cli->name);
            printf("[DISCONNECT] %s", send_buffer);
            broadcast_message(send_buffer, cli->uid);
            leave_flag = 1;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }

    close(cli->sockfd);
    queue_remove(cli->uid);
    free(cli);
    pthread_detach(pthread_self());

    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    pthread_t tid;
    int opt = 1;

    // 1. Create TCP Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[ERROR] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[ERROR] setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 2. Configure Address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 3. Bind
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[ERROR] Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 4. Listen
    if (listen(server_fd, 10) < 0) {
        perror("[ERROR] Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("=====================================================\n");
    printf("   TCP MULTI-USER CHAT SERVER (Port: %d)             \n", PORT);
    printf("=====================================================\n");
    printf("[INFO] Server listening on port %d...\n\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("[ERROR] Accept failed");
            continue;
        }

        // Allocate client context
        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->address = client_addr;
        cli->sockfd = client_fd;
        cli->uid = uid_counter++;

        queue_add(cli);
        pthread_create(&tid, NULL, &handle_client, (void *)cli);
    }

    close(server_fd);
    return 0;
}
