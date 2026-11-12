/*
 * Experiment 5: Client-Server Communication Using TCP - Matrix Type Identification
 * Course: PCCSL507 - Networks Lab
 * File: client.c
 * Description: Interactive TCP Client that prompts user for matrix dimensions and generation
 *              modes (Diagonal, Upper Triangular, Lower Triangular, Random, Manual), sends
 *              the matrix to the server, and displays the classified matrix type.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define MAX_N 50
#define BUFFER_SIZE 1024

void display_matrix(int matrix[MAX_N][MAX_N], int n) {
    printf("\n--- Generated %d x %d Matrix ---\n", n, n);
    for (int i = 0; i < n; i++) {
        printf("   [ ");
        for (int j = 0; j < n; j++) {
            printf("%3d ", matrix[i][j]);
        }
        printf("]\n");
    }
    printf("--------------------------------\n");
}

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char response[BUFFER_SIZE];

    srand(time(NULL));

    // 1. Create TCP Socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[ERROR] Socket creation failed");
        return 1;
    }

    // 2. Configure Server Address Structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("[ERROR] Invalid server address");
        close(sock_fd);
        return 1;
    }

    // 3. Connect to Server
    printf("=====================================================\n");
    printf("     TCP MATRIX CLASSIFICATION CLIENT - EXPERIMENT 5 \n");
    printf("=====================================================\n");
    printf("[INFO] Connecting to Server at %s:%d...\n", SERVER_IP, PORT);

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[ERROR] Connection failed. Is the server running?");
        close(sock_fd);
        return 1;
    }
    printf("[INFO] Connected successfully.\n\n");

    // Interactive Loop
    while (1) {
        int n;
        printf("Enter order of the square matrix N (1 to %d, or 0 to Exit): ", MAX_N);
        if (scanf("%d", &n) != 1 || n == 0) {
            break;
        }

        if (n < 1 || n > MAX_N) {
            printf("[WARNING] Order must be between 1 and %d. Please retry.\n\n", MAX_N);
            continue;
        }

        printf("\nSelect Matrix Generation Mode:\n");
        printf("  1. General Random Matrix [1 to 50] (Syllabus standard)\n");
        printf("  2. Test Preset: Upper Triangular Matrix\n");
        printf("  3. Test Preset: Lower Triangular Matrix\n");
        printf("  4. Test Preset: Diagonal Matrix\n");
        printf("  5. Manual Entry (Input elements one-by-one)\n");
        printf("Enter choice (1-5, default 1): ");

        int choice = 1;
        if (scanf("%d", &choice) != 1 || choice < 1 || choice > 5) {
            choice = 1;
        }

        int matrix[MAX_N][MAX_N];

        // Populate Matrix
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (choice == 1) {
                    matrix[i][j] = (rand() % 50) + 1;
                } else if (choice == 2) {
                    matrix[i][j] = (i > j) ? 0 : ((rand() % 50) + 1);
                } else if (choice == 3) {
                    matrix[i][j] = (i < j) ? 0 : ((rand() % 50) + 1);
                } else if (choice == 4) {
                    matrix[i][j] = (i == j) ? ((rand() % 50) + 1) : 0;
                } else if (choice == 5) {
                    printf("  Enter element [%d][%d]: ", i, j);
                    if (scanf("%d", &matrix[i][j]) != 1) matrix[i][j] = 0;
                }
            }
        }

        display_matrix(matrix, n);

        // Send Matrix Dimension N
        send(sock_fd, &n, sizeof(n), 0);

        // Send Matrix Data
        int total_bytes = n * n * sizeof(int);
        send(sock_fd, matrix, total_bytes, 0);

        // Receive Classification Result
        memset(response, 0, sizeof(response));
        ssize_t bytes_recv = recv(sock_fd, response, sizeof(response) - 1, 0);
        if (bytes_recv > 0) {
            response[bytes_recv] = '\0';
            printf("=====================================================\n");
            printf("SERVER CLASSIFICATION RESULT:\n");
            printf(">>> %s <<<\n", response);
            printf("=====================================================\n\n");
        } else {
            printf("[ERROR] Server closed the connection.\n");
            break;
        }

        printf("Test another matrix? (1 = Yes, 0 = No / Exit): ");
        int again = 1;
        if (scanf("%d", &again) != 1 || again == 0) {
            break;
        }
        printf("\n");
    }

    printf("\n[INFO] Closing client session. Goodbye!\n");
    close(sock_fd);
    return 0;
}
