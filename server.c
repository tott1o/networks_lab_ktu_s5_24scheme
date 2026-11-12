#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int s, c;
    struct sockaddr_in addr;
    char buf[256];
    FILE *fp;

    s = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8000);

    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    listen(s, 5);
    printf("Server ready...\n");

    while (1) {
        c = accept(s, NULL, NULL);

        if (fork() == 0) { // Child Process
            close(s);
            printf("Client connected! Child PID: %d\n", getpid());

            // Read filename
            read(c, buf, sizeof(buf));

            fp = fopen(buf, "r");
            if (fp == NULL) {
                write(c, "ERROR", 5);
            } else {
                write(c, "SUCCESS", 7);
                // Send file content line by line
                while (fgets(buf, sizeof(buf), fp) != NULL) {
                    write(c, buf, strlen(buf));
                }
                fclose(fp);
            }
            close(c); // Closing socket automatically signals EOF to client
            exit(0);
        }
        close(c);
    }
    return 0;
}