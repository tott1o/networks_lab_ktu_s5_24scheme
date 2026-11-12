#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int s;
    struct sockaddr_in addr;
    char buf[256], filename[100];
    FILE *fp;

    s = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&addr, sizeof(addr));
    printf("Connected to server.\n");

    // Send filename
    printf("Enter filename: ");
    scanf("%s", filename);
    write(s, filename, strlen(filename));

    // Check response
    read(s, buf, sizeof(buf));
    if (strncmp(buf, "SUCCESS", 7) == 0) {
        fp = fopen(filename, "w");
        int n;
        // Read until server closes connection
        while ((n = read(s, buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0';
            fprintf(fp, "%s", buf);
        }
        fclose(fp);
        printf("File downloaded successfully!\n");
    } else {
        printf("File not found on server.\n");
    }

    close(s);
    return 0;
}
