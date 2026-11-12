/*
 * Experiment 5: Client-Server Communication Using TCP - Matrix Type Identification
 * Course: PCCSL507 - Networks Lab
 * File: server.c
 * Description: TCP Server that receives a square matrix of order N from the client,
 *              analyzes its elements, classifies the matrix type, and returns the result.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX_N 50
#define BUFFER_SIZE 1024

/* Classifies whether a square matrix is Diagonal, Upper Triangular, Lower Triangular, or General */
const char* classify_matrix(int matrix[MAX_N][MAX_N], int n) {
    int is_upper = 1;
    int is_lower = 1;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            // Elements below the main diagonal must be 0 for Upper Triangular
            if (i > j && matrix[i][j] != 0) {
                is_upper = 0;
            }
            // Elements above the main diagonal must be 0 for Lower Triangular
            if (i < j && matrix[i][j] != 0) {
                is_lower = 0;
            }
        }
    }

    if (is_upper && is_lower) {
        return "Diagonal Matrix (all non-diagonal elements are 0)";
    } else if (is_upper) {
        return "Upper Triangular Matrix (all elements below main diagonal are 0)";
    } else if (is_lower) {
        return "Lower Triangular Matrix (all elements above main diagonal are 0)";
    } else {
        return "General Matrix (Neither Upper Triangular, Lower Triangular, nor Diagonal)";
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int opt = 1;

    // 1. Create TCP socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[ERROR] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Reuse address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[ERROR] setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 2. Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 3. Bind
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[ERROR] Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 4. Listen
    if (listen(server_fd, 5) < 0) {
        perror("[ERROR] Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("=====================================================\n");
    printf("   TCP MATRIX CLASSIFICATION SERVER (Port: %d)       \n", PORT);
    printf("=====================================================\n");
    printf("[INFO] Server is listening for incoming client connections...\n\n");

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("[ERROR] Accept failed");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("[CONNECTED] Client connected from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // Session loop for client: allow testing multiple matrices
        while (1) {
            int n = 0;
            ssize_t bytes_recv = recv(client_fd, &n, sizeof(n), 0);
            if (bytes_recv <= 0 || n <= 0 || n > MAX_N) {
                break; // Client closed connection or sent invalid size
            }

            printf("\n[INFO] Received request for matrix order N = %d\n", n);

            // Receive matrix elements
            int matrix[MAX_N][MAX_N];
            int total_bytes = n * n * sizeof(int);
            int received_bytes = 0;
            char *ptr = (char*)matrix;

            while (received_bytes < total_bytes) {
                ssize_t r = recv(client_fd, ptr + received_bytes, total_bytes - received_bytes, 0);
                if (r <= 0) break;
                received_bytes += r;
            }

            if (received_bytes < total_bytes) {
                printf("[WARNING] Incomplete matrix received.\n");
                break;
            }

            // Display received matrix
            printf("[INFO] Matrix elements received:\n");
            for (int i = 0; i < n; i++) {
                printf("   | ");
                for (int j = 0; j < n; j++) {
                    printf("%3d ", matrix[i][j]);
                }
                printf("|\n");
            }

            // Classify matrix
            const char* classification = classify_matrix(matrix, n);
            printf("[INFO] Classification: %s\n", classification);

            // Send result back to client
            send(client_fd, classification, strlen(classification) + 1, 0);
        }

        printf("[DISCONNECTED] Client %s:%d disconnected.\n", client_ip, ntohs(client_addr.sin_port));
        printf("-----------------------------------------------------\n");
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
