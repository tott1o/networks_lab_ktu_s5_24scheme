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
    char buf[256], res[512];

    s = socket(AF_INET, SOCK_DGRAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(2005);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter sentence with abbreviations (e.g., TBH IDK why LOL): ");
    fgets(buf, sizeof(buf), stdin);
    buf[strcspn(buf, "\n")] = 0;

    sendto(s, buf, strlen(buf) + 1, 0, (struct sockaddr *)&server, len);

    int n = recvfrom(s, res, sizeof(res), 0, (struct sockaddr *)&server, &len);
    res[n] = '\0';

    printf("Translated (Server -> Client): %s\n", res);

    close(s);
    return 0;
}