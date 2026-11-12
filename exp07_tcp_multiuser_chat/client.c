#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

int s;

// Separate receiving thread for simultaneous messaging[cite: 4]
void *recv_msg(void *arg) {
    char buf[256];
    while (recv(s, buf, sizeof(buf), 0) > 0) {
        printf("\n%s\n> ", buf);
        fflush(stdout);
    }
    return NULL;
}

int main() {
    struct sockaddr_in server;
    char name[50], msg[200], buf[256];

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(2005);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&server, sizeof(server));
    printf("Connected to chat.\n");

    printf("Enter username: ");
    scanf("%s", name);
    getchar(); // Clear newline

    pthread_t t;
    pthread_create(&t, NULL, recv_msg, NULL);
    pthread_detach(t);

    while (1) {
        printf("> ");
        fgets(msg, sizeof(msg), stdin);
        msg[strcspn(msg, "\n")] = 0;

        sprintf(buf, "%s: %s", name, msg);
        send(s, buf, sizeof(buf), 0);

        if (strcmp(msg, "exit") == 0) { // Graceful exit[cite: 4]
            break;
        }
    }

    close(s);
    return 0;
}