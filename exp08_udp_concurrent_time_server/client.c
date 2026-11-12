/*
 * Experiment 8: Concurrent Time Server Application Using UDP
 * Course: PCCSL507 - Networks Lab
 * File: client.c
 * Description: Interactive UDP Time Client that queries a remote concurrent time server.
 *              Supports interactive menu options: TIME (Local), UTC (GMT), DATE,
 *              EPOCH, UPTIME, CLOCK (Live ticking mode), HELP, and EXIT.
 *              Also supports batch mode: ./client <QUERY>
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
#define PORT 8083
#define BUFFER_SIZE 1024

/* Helper function to send query and receive server response */
int query_server(int sock_fd, struct sockaddr_in *server_addr, const char *query, char *resp_out, size_t max_out) {
    socklen_t addr_len = sizeof(*server_addr);
    if (sendto(sock_fd, query, strlen(query), 0, (struct sockaddr*)server_addr, addr_len) < 0) {
        perror("[ERROR] sendto failed");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes = recvfrom(sock_fd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)server_addr, &addr_len);
    if (bytes < 0) {
        printf("[ERROR] Request timed out. Remote server is not responding.\n");
        return -1;
    }
    buffer[bytes] = '\0';
    if (resp_out && max_out > 0) {
        strncpy(resp_out, buffer, max_out - 1);
        resp_out[max_out - 1] = '\0';
    } else {
        printf("\n=====================================================\n");
        printf("%s\n", buffer);
        printf("=====================================================\n");
    }
    return 0;
}

void print_menu() {
    printf("\nAvailable Commands:\n");
    printf("  1. TIME   - Query Current Local Server Time\n");
    printf("  2. UTC    - Query Universal Coordinated Time (UTC / GMT)\n");
    printf("  3. DATE   - Query Server Calendar Date & Day of Week\n");
    printf("  4. EPOCH  - Query Unix Epoch Seconds\n");
    printf("  5. UPTIME - Query Server Process Uptime & Request Count\n");
    printf("  6. CLOCK  - Live Ticking Digital Clock (5 synchronized updates)\n");
    printf("  7. HELP   - Display this menu\n");
    printf("  8. EXIT   - Exit Client\n\n");
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

    // 2. Configure Remote Server Address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("[ERROR] Invalid server address");
        close(sock_fd);
        return 1;
    }

    // BATCH MODE: Argument passed on command line
    if (argc >= 2) {
        printf("[INFO] Batch mode: Querying server with '%s'...\n", argv[1]);
        query_server(sock_fd, &server_addr, argv[1], NULL, 0);
        close(sock_fd);
        return 0;
    }

    // INTERACTIVE MODE
    printf("=====================================================\n");
    printf("     UDP CONCURRENT TIME CLIENT - EXPERIMENT 8       \n");
    printf("=====================================================\n");
    printf("Connected to Remote Time Server at %s:%d\n", SERVER_IP, PORT);
    print_menu();

    char line[128];
    while (1) {
        printf("time-client> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        line[strcspn(line, "\r\n")] = '\0';
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (strlen(cmd) == 0) continue;

        if (strcasecmp(cmd, "EXIT") == 0 || strcasecmp(cmd, "QUIT") == 0 || strcmp(cmd, "8") == 0) {
            printf("[INFO] Exiting Time Client. Goodbye!\n");
            break;
        } else if (strcasecmp(cmd, "HELP") == 0 || strcmp(cmd, "7") == 0) {
            print_menu();
        } else if (strcasecmp(cmd, "CLOCK") == 0 || strcmp(cmd, "6") == 0) {
            printf("[INFO] Live Ticking Clock (Synchronizing with server every 1s for 5 ticks):\n");
            for (int i = 1; i <= 5; i++) {
                char resp[BUFFER_SIZE];
                if (query_server(sock_fd, &server_addr, "TIME", resp, sizeof(resp)) == 0) {
                    char *first_line = strtok(resp, "\n");
                    printf("  [%d/5] %s\n", i, first_line ? first_line : resp);
                }
                sleep(1);
            }
            printf("[INFO] Clock sync complete.\n\n");
        } else {
            // Forward command to server (TIME, UTC, DATE, EPOCH, UPTIME, etc.)
            query_server(sock_fd, &server_addr, cmd, NULL, 0);
        }
    }

    close(sock_fd);
    return 0;
}
