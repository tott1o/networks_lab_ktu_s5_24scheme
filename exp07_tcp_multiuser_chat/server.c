#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

int clients[10];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg) {
    int c = *((int *)arg);
    char buf[256];

    while (recv(c, buf, sizeof(buf), 0) > 0) {
        if (strncmp(buf, "exit", 4) == 0) break;

        // Broadcast to all other clients[cite: 4]
        pthread_mutex_lock(&lock);
        for (int i = 0; i < 10; i++) {
            if (clients[i] != 0 && clients[i] != c) {
                send(clients[i], buf, sizeof(buf), 0);
            }
        }
        pthread_mutex_unlock(&lock);
    }

    // Remove client on disconnect[cite: 4]
    pthread_mutex_lock(&lock);
    for (int i = 0; i < 10; i++) {
        if (clients[i] == c) clients[i] = 0;
    }
    pthread_mutex_unlock(&lock);

    close(c);
    return NULL;
}

int main() {
    int s, c;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(2005);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 5);
    printf("Chat Server started...\n");

    while (1) {
        c = accept(s, (struct sockaddr *)&client, &len);

        pthread_mutex_lock(&lock);
        for (int i = 0; i < 10; i++) {
            if (clients[i] == 0) {
                clients[i] = c;
                break;
            }
        }
        pthread_mutex_unlock(&lock);

        // Create separate thread for each client[cite: 4]
        pthread_t t;
        pthread_create(&t, NULL, handle_client, &c);
        pthread_detach(t);
    }

    close(s);
    return 0;
}