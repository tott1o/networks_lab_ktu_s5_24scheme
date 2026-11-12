#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

int main() {
    int s, order;
    int mat[10][10];
    char rBuf[100];
    struct sockaddr_in server;

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(2005);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&server, sizeof(server));
    printf("Connected to server.\n");

    printf("Enter matrix order N: ");
    scanf("%d", &order);

    // Generate random matrix with values in range [1, 50] inclusive[cite: 1]
    srand(time(NULL));
    printf("Generated Matrix:\n");
    for (int i = 0; i < order; i++) {
        for (int j = 0; j < order; j++) {
            mat[i][j] = (rand() % 50) + 1;
            printf("%3d ", mat[i][j]);
        }
        printf("\n");
    }

    // Send order and matrix to server
    send(s, &order, sizeof(order), 0);
    send(s, mat, sizeof(mat), 0);

    // Receive result from server
    recv(s, rBuf, sizeof(rBuf), 0);
    printf("Server Result: %s\n", rBuf);

    close(s);
    return 0;
}