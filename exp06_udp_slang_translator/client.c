/*
 * Experiment 6: Client-Server Communication Using UDP - New-Gen Slang Translator
 * Course: PCCSL507 - Networks Lab
 * File: client.c
 * Description: Interactive UDP Slang Client that connects to the translation server.
 *              Allows interactive sentence translation in a continuous REPL session,
 *              viewing server slang dictionary (dict), and running the official syllabus
 *              sample sentence (sample). Also supports batch execution via CLI args.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8081
#define BUFFER_SIZE 4096

int send_and_receive(int sock_fd, struct sockaddr_in *server_addr, const char *msg) {
    socklen_t server_len = sizeof(*server_addr);
    char response[BUFFER_SIZE];

    if (sendto(sock_fd, msg, strlen(msg), 0, (struct sockaddr*)server_addr, server_len) < 0) {
        perror("[ERROR] sendto failed");
        return -1;
    }

    memset(response, 0, BUFFER_SIZE);
    ssize_t bytes_recv = recvfrom(sock_fd, response, BUFFER_SIZE - 1, 0,
                                  (struct sockaddr*)server_addr, &server_len);
    if (bytes_recv < 0) {
        printf("[ERROR] Request timed out. Server not responding.\n");
        return -1;
    }
    response[bytes_recv] = '\0';

    printf("\n%s\n\n", response);
    return 0;
}

void print_help() {
    printf("\nAvailable Commands:\n");
    printf("  <any sentence>  - Translate informal abbreviations into formal English\n");
    printf("  sample          - Run official KTU syllabus test sentence\n");
    printf("  dict / list     - View dictionary of supported slang acronyms\n");
    printf("  help            - Display this help message\n");
    printf("  exit / quit     - Exit translator client\n\n");
}

int main(int argc, char *argv[]) {
    int sock_fd;
    struct sockaddr_in server_addr;

    // 1. Create UDP Socket
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("[ERROR] Socket creation failed");
        return 1;
    }

    // Set 3-second receive timeout
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // 2. Configure Server Address Structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("[ERROR] Invalid server IP address");
        close(sock_fd);
        return 1;
    }

    // BATCH MODE: Argument passed on command line
    if (argc >= 2) {
        printf("[INFO] Batch mode: Translating: \"%s\"\n", argv[1]);
        send_and_receive(sock_fd, &server_addr, argv[1]);
        close(sock_fd);
        return 0;
    }

    // INTERACTIVE MODE
    printf("=====================================================\n");
    printf("     UDP NEW-GEN SLANG TRANSLATOR - EXPERIMENT 6     \n");
    printf("=====================================================\n");
    printf("Connected to Slang Translation Server at %s:%d\n", SERVER_IP, PORT);
    print_help();

    char line[BUFFER_SIZE];
    while (1) {
        printf("slang-client> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        line[strcspn(line, "\r\n")] = '\0';
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (strlen(cmd) == 0) continue;

        if (strcasecmp(cmd, "EXIT") == 0 || strcasecmp(cmd, "QUIT") == 0) {
            printf("[INFO] Exiting Slang Translator Client. Goodbye!\n");
            break;
        } else if (strcasecmp(cmd, "HELP") == 0) {
            print_help();
        } else {
            send_and_receive(sock_fd, &server_addr, cmd);
        }
    }

    close(sock_fd);
    return 0;
}
