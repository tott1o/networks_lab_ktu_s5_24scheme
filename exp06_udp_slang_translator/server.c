#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Server-side translation function[cite: 2]
void translate(char *src, char *dest) {
    dest[0] = '\0';
    char *token = strtok(src, " ");
    
    while (token != NULL) {
        if (strcasecmp(token, "tbh") == 0) strcat(dest, "To be honest ");
        else if (strcasecmp(token, "idk") == 0) strcat(dest, "I don't know ");
        else if (strcasecmp(token, "lol") == 0) strcat(dest, "laughing out loud ");
        else if (strcasecmp(token, "ig") == 0) strcat(dest, "I guess ");
        else if (strcasecmp(token, "atm") == 0) strcat(dest, "at the moment ");
        else if (strcasecmp(token, "omg") == 0) strcat(dest, "Oh my God ");
        else if (strcasecmp(token, "asap") == 0) strcat(dest, "as soon as possible ");
        else if (strcasecmp(token, "irl") == 0) strcat(dest, "in real life ");
        else if (strcasecmp(token, "tbf") == 0) strcat(dest, "To be fair ");
        else if (strcasecmp(token, "ttyl") == 0) strcat(dest, "Talk to you later ");
        else if (strcasecmp(token, "nvm") == 0) strcat(dest, "Never mind ");
        else {
            strcat(dest, token);
            strcat(dest, " ");
        }
        token = strtok(NULL, " ");
    }
}

int main() {
    int s;
    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);
    char buf[256], res[512];

    s = socket(AF_INET, SOCK_DGRAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(2005);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    printf("UDP Slang Server waiting...\n");

    int n = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&client, &len);
    buf[n] = '\0';
    printf("Received: %s\n", buf);

    translate(buf, res);

    sendto(s, res, strlen(res) + 1, 0, (struct sockaddr *)&client, len);

    close(s);
    return 0;
}