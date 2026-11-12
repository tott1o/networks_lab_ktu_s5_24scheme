#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int s;
    struct sockaddr_in server;
    socklen_t len = sizeof(server);
    char buf[100], choice;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(2005);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    do {
        printf("Request time? (y/n): ");
        scanf(" %c", &choice);

        if (choice == 'y' || choice == 'Y') {
            sendto(s, "time", 5, 0, (struct sockaddr *)&server, len);

            recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&server, &len);
            printf("Server Time: %s\n", buf);
        } else {
            break; // Terminate communication[cite: 3]
        }
    } while (1);

    close(s);
    return 0;
}