/*
 * Experiment 9: Concurrent File Server Using TCP (Fork-Based)
 * Course: PCCSL507 - Networks Lab
 * File: server.c
 * Description: Concurrent TCP File Server that forks a child process for each incoming
 *              client connection. The child process runs an interactive service loop supporting
 *              commands: LIST, GET <filename>, INFO <filename>, HELP, and EXIT, transmitting
 *              file contents along with its own Process ID (PID) to the client.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 8084
#define BUFFER_SIZE 4096

/* SIGCHLD handler to reap terminated zombie child processes */
void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

/* Routine executed by child process to handle client file requests */
void handle_client(int connfd, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    pid_t my_pid = getpid();
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    int client_port = ntohs(client_addr.sin_port);

    printf("[CHILD PID %d] Handling connection from %s:%d\n", my_pid, client_ip, client_port);

    // Send initial welcome greeting with child worker PID
    snprintf(buffer, sizeof(buffer),
             "[SERVER PID: %d] Connected to Concurrent File Server. Type HELP for commands.\n"
             "[SERVER] End of command output.\n",
             my_pid);
    send(connfd, buffer, strlen(buffer), 0);

    // Interactive request loop
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_recv = recv(connfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_recv <= 0) {
            // Client closed connection or disconnected
            break;
        }
        buffer[bytes_recv] = '\0';
        buffer[strcspn(buffer, "\r\n")] = '\0';

        // Trim leading whitespace
        char *cmd = buffer;
        while (*cmd == ' ') cmd++;
        if (strlen(cmd) == 0) continue;

        printf("[CHILD PID %d] Request from %s:%d: \"%s\"\n", my_pid, client_ip, client_port, cmd);

        // Command: EXIT / QUIT
        if (strcasecmp(cmd, "QUIT") == 0 || strcasecmp(cmd, "EXIT") == 0) {
            snprintf(buffer, sizeof(buffer),
                     "[SERVER PID: %d] Goodbye! Closing session.\n"
                     "[SERVER] End of command output.\n",
                     my_pid);
            send(connfd, buffer, strlen(buffer), 0);
            break;
        }
        // Command: HELP
        else if (strcasecmp(cmd, "HELP") == 0) {
            snprintf(buffer, sizeof(buffer),
                     "[SERVER PID: %d] Available Commands:\n"
                     "  LIST             - List files hosted on server\n"
                     "  GET <filename>   - Display contents of requested file\n"
                     "  INFO <filename>  - View file size, metadata, and statistics\n"
                     "  HELP             - Display this command manual\n"
                     "  EXIT / QUIT      - Terminate connection with server\n"
                     "[SERVER] End of command output.\n",
                     my_pid);
            send(connfd, buffer, strlen(buffer), 0);
        }
        // Command: LIST / DIR
        else if (strcasecmp(cmd, "LIST") == 0 || strcasecmp(cmd, "DIR") == 0) {
            DIR *d = opendir(".");
            char list_buf[BUFFER_SIZE];
            snprintf(list_buf, sizeof(list_buf),
                     "[SERVER PID: %d] Files hosted on server:\n"
                     "-------------------------------------------------------------\n",
                     my_pid);
            send(connfd, list_buf, strlen(list_buf), 0);

            if (d) {
                struct dirent *dir;
                int count = 0;
                while ((dir = readdir(d)) != NULL) {
                    if (dir->d_name[0] == '.') continue;
                    struct stat st;
                    if (stat(dir->d_name, &st) == 0 && S_ISREG(st.st_mode)) {
                        count++;
                        snprintf(list_buf, sizeof(list_buf),
                                 "  %2d. %-24s (%ld bytes)\n",
                                 count, dir->d_name, (long)st.st_size);
                        send(connfd, list_buf, strlen(list_buf), 0);
                    }
                }
                closedir(d);
                if (count == 0) {
                    const char *none = "  (No regular files found)\n";
                    send(connfd, none, strlen(none), 0);
                }
            }
            const char *footer = "-------------------------------------------------------------\n"
                                 "[SERVER] End of command output.\n";
            send(connfd, footer, strlen(footer), 0);
        }
        // Command: INFO <filename>
        else if (strncasecmp(cmd, "INFO ", 5) == 0) {
            char target_file[256];
            char out_buf[BUFFER_SIZE];
            char *p = cmd + 5;
            while (*p == ' ') p++;
            strncpy(target_file, p, sizeof(target_file) - 1);
            target_file[sizeof(target_file) - 1] = '\0';

            struct stat st;
            if (stat(target_file, &st) == 0 && S_ISREG(st.st_mode)) {
                char time_buf[64];
                struct tm *tm_info = localtime(&st.st_mtime);
                strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);
                snprintf(out_buf, sizeof(out_buf),
                         "[SERVER PID: %d] File Metadata for '%s':\n"
                         "  Size:          %ld bytes\n"
                         "  Last Modified: %s\n"
                         "  Permissions:   %04o\n"
                         "[SERVER] End of command output.\n",
                         my_pid, target_file, (long)st.st_size, time_buf, (unsigned int)(st.st_mode & 0777));
                send(connfd, out_buf, strlen(out_buf), 0);
            } else {
                snprintf(out_buf, sizeof(out_buf),
                         "[SERVER PID: %d] ERROR: File '%s' not found on server.\n"
                         "[SERVER] End of command output.\n",
                         my_pid, target_file);
                send(connfd, out_buf, strlen(out_buf), 0);
            }
        }
        // Command: GET <filename> or direct <filename>
        else {
            char target_file[256];
            char out_buf[BUFFER_SIZE];
            char *p = cmd;
            if (strncasecmp(cmd, "GET ", 4) == 0) {
                p = cmd + 4;
            } else if (strncasecmp(cmd, "DOWNLOAD ", 9) == 0) {
                p = cmd + 9;
            }
            while (*p == ' ') p++;
            strncpy(target_file, p, sizeof(target_file) - 1);
            target_file[sizeof(target_file) - 1] = '\0';

            // Security check: reject paths with '..' or slashes
            if (strstr(target_file, "..") != NULL || strchr(target_file, '/') != NULL || strchr(target_file, '\\') != NULL) {
                snprintf(out_buf, sizeof(out_buf),
                         "[SERVER PID: %d] ERROR: Invalid or prohibited filename '%s'.\n"
                         "[SERVER] End of file reached.\n",
                         my_pid, target_file);
                send(connfd, out_buf, strlen(out_buf), 0);
                continue;
            }

            FILE *fp = fopen(target_file, "rb");
            if (fp == NULL) {
                snprintf(out_buf, sizeof(out_buf),
                         "[SERVER PID: %d] ERROR: File '%s' not found on server.\n"
                         "[SERVER] End of file reached.\n",
                         my_pid, target_file);
                printf("[CHILD PID %d] File '%s' not found. Sent error response.\n", my_pid, target_file);
                send(connfd, out_buf, strlen(out_buf), 0);
            } else {
                fseek(fp, 0, SEEK_END);
                long fsize = ftell(fp);
                fseek(fp, 0, SEEK_SET);

                snprintf(out_buf, sizeof(out_buf),
                         "[SERVER PID: %d] SUCCESS: File '%s' found (%ld bytes). Transmitting contents below:\n"
                         "-------------------------------------------------------------\n",
                         my_pid, target_file, fsize);
                send(connfd, out_buf, strlen(out_buf), 0);

                size_t bytes_read;
                while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
                    if (send(connfd, buffer, bytes_read, 0) < 0) {
                        perror("[ERROR] send failed during file transfer");
                        break;
                    }
                }
                fclose(fp);

                const char *footer = "\n-------------------------------------------------------------\n"
                                     "[SERVER] End of file reached.\n";
                send(connfd, footer, strlen(footer), 0);
                printf("[CHILD PID %d] File '%s' transmitted successfully.\n", my_pid, target_file);
            }
        }
    }

    close(connfd);
    printf("[CHILD PID %d] Finished servicing client %s:%d. Exiting child process.\n", my_pid, client_ip, client_port);
    exit(EXIT_SUCCESS);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int opt = 1;

    // Register SIGCHLD handler to prevent zombie processes
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("[ERROR] sigaction failed");
        exit(EXIT_FAILURE);
    }

    // 1. Create TCP Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[ERROR] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set SO_REUSEADDR
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[ERROR] setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 2. Configure Address
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
    if (listen(server_fd, 10) < 0) {
        perror("[ERROR] Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("=====================================================\n");
    printf("   CONCURRENT TCP FILE SERVER (Port: %d)             \n", PORT);
    printf("=====================================================\n");
    printf("[INFO] Parent Server listening with PID: %d\n\n", getpid());

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("[ERROR] Accept failed");
            continue;
        }

        // Fork child process for concurrent execution
        pid_t pid = fork();

        if (pid < 0) {
            perror("[ERROR] fork() failed");
            close(client_fd);
        } else if (pid == 0) {
            // --- Child Process ---
            close(server_fd); // Close listening socket in child
            handle_client(client_fd, client_addr);
        } else {
            // --- Parent Process ---
            close(client_fd); // Close connected socket in parent
            printf("[PARENT PID %d] Forked child process with PID %d to handle client.\n", getpid(), pid);
        }
    }

    close(server_fd);
    return 0;
}
