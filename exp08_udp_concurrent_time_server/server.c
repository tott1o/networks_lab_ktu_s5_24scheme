/*
 * Experiment 8: Concurrent Time Server Application Using UDP
 * Course: PCCSL507 - Networks Lab
 * File: server.c
 * Description: Concurrent UDP Time Server that handles multiple client time queries
 *              concurrently using POSIX threads. Supports queries:
 *              TIME (Local), UTC (GMT), DATE, EPOCH, UPTIME, PING, and HELP.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8083
#define BUFFER_SIZE 1024

// Structure to pass request context to worker thread
typedef struct {
    int sockfd;
    struct sockaddr_in client_addr;
    socklen_t addr_len;
    char request_msg[BUFFER_SIZE];
    int request_id;
} request_context_t;

static int request_counter = 0;
static time_t server_start_time = 0;
static pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Worker thread routine to process request concurrently */
void *process_time_request(void *arg) {
    request_context_t *ctx = (request_context_t *)arg;
    char time_str[128];
    char formatted_response[BUFFER_SIZE];

    time_t raw_time;
    time(&raw_time);

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ctx->client_addr.sin_addr, client_ip, sizeof(client_ip));
    int client_port = ntohs(ctx->client_addr.sin_port);

    // Clean request string
    char req[64];
    strncpy(req, ctx->request_msg, sizeof(req) - 1);
    req[sizeof(req) - 1] = '\0';
    req[strcspn(req, "\r\n")] = '\0';
    char *p = req;
    while (*p == ' ') p++;

    printf("[WORKER THREAD %lu] Servicing Request #%d (\"%s\") from %s:%d\n",
           (unsigned long)pthread_self(), ctx->request_id, p, client_ip, client_port);

    // Command: UTC / GMT
    if (strcasecmp(p, "UTC") == 0 || strcasecmp(p, "GMT") == 0 || strcmp(p, "2") == 0) {
        struct tm *gm_info = gmtime(&raw_time);
        strftime(time_str, sizeof(time_str), "%a %b %d %H:%M:%S UTC %Y", gm_info);
        snprintf(formatted_response, sizeof(formatted_response),
                 "SERVER UTC TIME: %s\n"
                 "[Server PID: %d | Worker Thread: %lu | Request ID: %d]",
                 time_str, getpid(), (unsigned long)pthread_self(), ctx->request_id);
    }
    // Command: DATE
    else if (strcasecmp(p, "DATE") == 0 || strcmp(p, "3") == 0) {
        struct tm *time_info = localtime(&raw_time);
        strftime(time_str, sizeof(time_str), "%A, %B %d, %Y (Day %j of year %Y)", time_info);
        snprintf(formatted_response, sizeof(formatted_response),
                 "SERVER CALENDAR DATE: %s\n"
                 "[Server PID: %d | Worker Thread: %lu | Request ID: %d]",
                 time_str, getpid(), (unsigned long)pthread_self(), ctx->request_id);
    }
    // Command: EPOCH
    else if (strcasecmp(p, "EPOCH") == 0 || strcmp(p, "4") == 0) {
        snprintf(formatted_response, sizeof(formatted_response),
                 "SERVER UNIX EPOCH: %ld seconds since Jan 01 1970 00:00:00 UTC\n"
                 "[Server PID: %d | Worker Thread: %lu | Request ID: %d]",
                 (long)raw_time, getpid(), (unsigned long)pthread_self(), ctx->request_id);
    }
    // Command: UPTIME
    else if (strcasecmp(p, "UPTIME") == 0 || strcmp(p, "5") == 0) {
        long uptime = (long)(raw_time - server_start_time);
        long hours = uptime / 3600;
        long minutes = (uptime % 3600) / 60;
        long seconds = uptime % 60;
        snprintf(formatted_response, sizeof(formatted_response),
                 "SERVER UPTIME: %ldh %ldm %lds (Total requests processed: %d)\n"
                 "[Server PID: %d | Worker Thread: %lu | Request ID: %d]",
                 hours, minutes, seconds, ctx->request_id,
                 getpid(), (unsigned long)pthread_self(), ctx->request_id);
    }
    // Command: PING
    else if (strcasecmp(p, "PING") == 0) {
        snprintf(formatted_response, sizeof(formatted_response),
                 "PONG from Server (PID: %d | Worker Thread: %lu)",
                 getpid(), (unsigned long)pthread_self());
    }
    // Command: HELP
    else if (strcasecmp(p, "HELP") == 0 || strcmp(p, "7") == 0) {
        snprintf(formatted_response, sizeof(formatted_response),
                 "Available Time Server Queries:\n"
                 "  TIME   - Local server time\n"
                 "  UTC    - Coordinated Universal Time (GMT)\n"
                 "  DATE   - Full calendar date and day of week\n"
                 "  EPOCH  - Unix epoch seconds timestamp\n"
                 "  UPTIME - Server uptime and statistics\n"
                 "  PING   - Connection test latency");
    }
    // Default: Local Time (matches syllabus specification)
    else {
        struct tm *time_info = localtime(&raw_time);
        strftime(time_str, sizeof(time_str), "%a %b %d %H:%M:%S %Z %Y", time_info);
        snprintf(formatted_response, sizeof(formatted_response),
                 "SERVER SYSTEM TIME: %s\n"
                 "[Server PID: %d | Worker Thread: %lu | Request ID: %d]",
                 time_str, getpid(), (unsigned long)pthread_self(), ctx->request_id);
    }

    // Send response back to remote client over UDP
    ssize_t bytes_sent = sendto(ctx->sockfd, formatted_response, strlen(formatted_response), 0,
                                (struct sockaddr*)&ctx->client_addr, ctx->addr_len);
    if (bytes_sent < 0) {
        perror("[ERROR] sendto failed in worker thread");
    } else {
        printf("[WORKER THREAD %lu] Response sent to %s:%d\n",
               (unsigned long)pthread_self(), client_ip, client_port);
    }

    free(ctx);
    pthread_detach(pthread_self());
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr;

    time(&server_start_time);

    // 1. Create UDP Socket (SOCK_DGRAM)
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("[ERROR] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Configure Server Address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 3. Bind
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("[ERROR] Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("=====================================================\n");
    printf("     CONCURRENT UDP TIME SERVER (Port: %d)           \n", PORT);
    printf("=====================================================\n");
    printf("[INFO] Server is running on PID %d\n", getpid());
    printf("[INFO] Multi-threaded worker model initialized.\n");
    printf("[INFO] Ready to accept concurrent client UDP queries...\n\n");

    while (1) {
        request_context_t *ctx = (request_context_t *)malloc(sizeof(request_context_t));
        if (!ctx) {
            perror("[ERROR] malloc failed");
            continue;
        }

        ctx->sockfd = server_fd;
        ctx->addr_len = sizeof(ctx->client_addr);
        memset(ctx->request_msg, 0, BUFFER_SIZE);

        ssize_t bytes_recv = recvfrom(server_fd, ctx->request_msg, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr*)&ctx->client_addr, &ctx->addr_len);
        if (bytes_recv < 0) {
            perror("[ERROR] recvfrom failed");
            free(ctx);
            continue;
        }
        ctx->request_msg[bytes_recv] = '\0';

        pthread_mutex_lock(&count_mutex);
        ctx->request_id = ++request_counter;
        pthread_mutex_unlock(&count_mutex);

        // Spawn a concurrent worker thread for each incoming datagram
        pthread_t worker_tid;
        if (pthread_create(&worker_tid, NULL, process_time_request, (void *)ctx) != 0) {
            perror("[ERROR] pthread_create failed");
            free(ctx);
        }
    }

    close(server_fd);
    return 0;
}
