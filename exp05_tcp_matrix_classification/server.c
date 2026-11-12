#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int s, c, order;
    int mat[10][10];
    char sBuf[100];
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

    s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(2005);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 5);
    printf("Server waiting for client...\n");

    c = accept(s, (struct sockaddr *)&client, &client_len);
    printf("Client connected!\n");

    // Receive matrix order and elements
    recv(c, &order, sizeof(order), 0);
    recv(c, mat, sizeof(mat), 0);

    printf("Received Matrix (%dx%d):\n", order, order);
    int is_diag = 1, is_upper = 1, is_lower = 1;

    for (int i = 0; i < order; i++) {
        for (int j = 0; j < order; j++) {
            printf("%3d ", mat[i][j]);
            if (i != j && mat[i][j] != 0) is_diag = 0;
            if (i > j && mat[i][j] != 0) is_upper = 0;
            if (i < j && mat[i][j] != 0) is_lower = 0;
        }
        printf("\n");
    }

    // Determine matrix type (checking most specific type first)
    if (is_diag) {
        strcpy(sBuf, "Diagonal Matrix");
    } else if (is_upper) {
        strcpy(sBuf, "Upper Triangular Matrix");
    } else if (is_lower) {
        strcpy(sBuf, "Lower Triangular Matrix");
    } else {
        strcpy(sBuf, "Neither Upper Triangular nor Lower Triangular Matrix");
    }

    printf("Classification Result: %s\n", sBuf);
    send(c, sBuf, sizeof(sBuf), 0);

    close(c);
    close(s);
    return 0;
}