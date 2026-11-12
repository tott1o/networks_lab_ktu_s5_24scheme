/*
 * Experiment 9: Concurrent File Server Using TCP (Fork-Based)
 * Course: PCCSL507 - Networks Lab
 * File: client.c
 * Description: Interactive TCP File Client that connects to the concurrent file server,
 *              presents an interactive CLI shell supporting commands:
 *              LIST, GET, DOWNLOAD, INFO, HELP, and EXIT, displaying server worker
 *              Process ID (PID) alongside file contents and metadata.
 *              Also supports batch mode via command-line arguments: ./client <filename>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8084
#define BUFFER_SIZE 4096

/* Helper function to read until delimiter "[SERVER] End of" */
void read_until_delimiter(int sock_fd, char *output, size_t max_out) {
    char temp[BUFFER_SIZE];
    size_t total = 0;
    if (output && max_out > 0) output[0] = '\0';

    while (1) {
        memset(temp, 0, sizeof(temp));
        ssize_t n = recv(sock_fd, temp, sizeof(temp) - 1, 0);
        if (n <= 0) break;
        temp[n] = '\0';

        if (output && total + n < max_out) {
            strcat(output, temp);
            total += n;
        } else if (!output) {
            printf("%s", temp);
            fflush(stdout);
        }

        if (strstr(temp, "[SERVER] End of") != NULL ||
            strstr(temp, "Goodbye! Closing session.") != NULL) {
            break;
        }
    }
}

/* Download file and save to local disk */
void handle_download(int sock_fd, const char *filename) {
    char req[512];
    snprintf(req, sizeof(req), "GET %s\n", filename);
    send(sock_fd, req, strlen(req), 0);

    char resp[BUFFER_SIZE * 16];
    read_until_delimiter(sock_fd, resp, sizeof(resp));

    if (strstr(resp, "SUCCESS:") != NULL) {
        char local_name[300];
        snprintf(local_name, sizeof(local_name), "downloaded_%s", filename);

        // Find start of file content (after separator line)
        char *sep = strstr(resp, "-------------------------------------------------------------\n");
        if (sep) {
            char *content_start = sep + strlen("-------------------------------------------------------------\n");
            char *footer = strstr(content_start, "\n-------------------------------------------------------------\n[SERVER] End of file reached.");
            size_t content_len;
            if (footer) {
                content_len = footer - content_start;
            } else {
                content_len = strlen(content_start);
            }

            FILE *fp = fopen(local_name, "wb");
            if (fp) {
                fwrite(content_start, 1, content_len, fp);
                fclose(fp);
                printf("[SUCCESS] File saved locally as '%s' (%zu bytes).\n", local_name, content_len);
            } else {
                perror("[ERROR] Failed to create local file");
            }
        }
    } else {
        printf("%s", resp);
    }
}

int main(int argc, char *argv[]) {
    int sock_fd;
    struct sockaddr_in server_addr;

    // 1. Create Socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[ERROR] Socket creation failed");
        return 1;
    }

    // 2. Configure Server Address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("[ERROR] Invalid server address");
        close(sock_fd);
        return 1;
    }

    // 3. Connect to Server
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[ERROR] Connection to server failed");
        close(sock_fd);
        return 1;
    }

    // Read initial server banner
    char banner[BUFFER_SIZE];
    read_until_delimiter(sock_fd, banner, sizeof(banner));
    // Remove trailer line
    char *del = strstr(banner, "[SERVER] End of command output.");
    if (del) *del = '\0';

    // BATCH MODE: If filename passed as command-line argument
    if (argc >= 2) {
        printf("%s", banner);
        printf("[INFO] Batch mode: Requesting '%s'...\n", argv[1]);
        char req[512];
        snprintf(req, sizeof(req), "GET %s\n", argv[1]);
        send(sock_fd, req, strlen(req), 0);
        read_until_delimiter(sock_fd, NULL, 0);
        close(sock_fd);
        return 0;
    }

    // INTERACTIVE MODE
    printf("=====================================================\n");
    printf("        CONCURRENT TCP FILE CLIENT - EXPERIMENT 9    \n");
    printf("=====================================================\n");
    printf("%s", banner);
    printf("Type 'HELP' for available commands or 'EXIT' to quit.\n\n");

    char input[512];
    while (1) {
        printf("file-client> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\r\n")] = '\0';
        char *cmd = input;
        while (*cmd == ' ') cmd++;
        if (strlen(cmd) == 0) continue;

        if (strcasecmp(cmd, "EXIT") == 0 || strcasecmp(cmd, "QUIT") == 0) {
            send(sock_fd, "EXIT\n", 5, 0);
            read_until_delimiter(sock_fd, NULL, 0);
            break;
        } else if (strncasecmp(cmd, "DOWNLOAD ", 9) == 0) {
            char *fname = cmd + 9;
            while (*fname == ' ') fname++;
            if (strlen(fname) > 0) {
                handle_download(sock_fd, fname);
            } else {
                printf("[USAGE] DOWNLOAD <filename>\n");
            }
        } else {
            // Forward command to server (LIST, GET <file>, INFO <file>, HELP, or raw filename)
            char send_cmd[600];
            if (strncasecmp(cmd, "GET ", 4) != 0 &&
                strcasecmp(cmd, "LIST") != 0 &&
                strcasecmp(cmd, "DIR") != 0 &&
                strcasecmp(cmd, "HELP") != 0 &&
                strncasecmp(cmd, "INFO ", 5) != 0) {
                // User typed just a filename like "sample.txt"
                snprintf(send_cmd, sizeof(send_cmd), "GET %s\n", cmd);
            } else {
                snprintf(send_cmd, sizeof(send_cmd), "%s\n", cmd);
            }

            send(sock_fd, send_cmd, strlen(send_cmd), 0);
            read_until_delimiter(sock_fd, NULL, 0);
        }
    }

    printf("[INFO] Session closed. Exiting.\n");
    close(sock_fd);
    return 0;
}
