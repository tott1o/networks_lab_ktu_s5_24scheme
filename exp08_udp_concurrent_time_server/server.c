#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

int main() {
    int s;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);
    char buf[100];

    s = socket(AF_INET, SOCK_DGRAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(2005);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    printf("Time Server waiting...\n");

    while (1) {
        // Receive request from client[cite: 3]
        recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&client, &len);

        if (fork() == 0) { // Child process handles request concurrently[cite: 3]
            time_t t;
            time(&t);
            char *time_str = ctime(&t); // Get system time[cite: 3]

            // Send time back to client[cite: 3]
            sendto(s, time_str, strlen(time_str) + 1, 0, (struct sockaddr *)&client, len);
            exit(0);
        }
    }
    close(s);
    return 0;
}