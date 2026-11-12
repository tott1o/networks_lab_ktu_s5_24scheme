/*
 * Experiment 6: Client-Server Communication Using UDP - New-Gen Slang Translator
 * Course: PCCSL507 - Networks Lab
 * File: server.c
 * Description: UDP Server that receives informal English sentences containing abbreviations
 *              (slang), translates them into formal English, and returns the translated
 *              sentence to the client. Also supports DICT, SAMPLE, and PING queries.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8081
#define BUFFER_SIZE 4096

// Mapping structure for slang abbreviations to formal English
typedef struct {
    const char *abbreviation;
    const char *translation;
} SlangEntry;

static const SlangEntry SLANG_DICT[] = {
    {"tbh",  "to be honest"},
    {"ig",   "I guess"},
    {"tbf",  "to be fair"},
    {"atm",  "at the moment"},
    {"irl",  "in real life"},
    {"lol",  "laughing out loud"},
    {"asap", "as soon as possible"},
    {"omg",  "oh my god"},
    {"ttyl", "talk to you later"},
    {"idk",  "I don't know"},
    {"nvm",  "never mind"},
    {"idc",  "I don't care"}  // included in the syllabus sample text
};

static const int NUM_SLANG = sizeof(SLANG_DICT) / sizeof(SLANG_DICT[0]);

/* Function to lookup abbreviation (case-insensitive) */
const char* lookup_slang(const char *word) {
    char lower_word[128];
    size_t len = strlen(word);
    if (len >= sizeof(lower_word)) return NULL;

    for (size_t i = 0; i < len; i++) {
        lower_word[i] = (char)tolower((unsigned char)word[i]);
    }
    lower_word[len] = '\0';

    for (int i = 0; i < NUM_SLANG; i++) {
        if (strcmp(lower_word, SLANG_DICT[i].abbreviation) == 0) {
            return SLANG_DICT[i].translation;
        }
    }
    return NULL;
}

/* Function to translate sentence preserving punctuation and whitespaces */
void translate_sentence(const char *input, char *output, size_t out_size) {
    size_t in_len = strlen(input);
    size_t in_idx = 0;
    size_t out_idx = 0;

    output[0] = '\0';

    while (in_idx < in_len) {
        if (!isalpha((unsigned char)input[in_idx])) {
            if (out_idx + 1 < out_size) {
                output[out_idx++] = input[in_idx++];
                output[out_idx] = '\0';
            } else {
                break;
            }
            continue;
        }

        char word[128];
        size_t w_len = 0;
        while (in_idx < in_len && isalpha((unsigned char)input[in_idx]) && w_len < sizeof(word) - 1) {
            word[w_len++] = input[in_idx++];
        }
        word[w_len] = '\0';

        const char *replacement = lookup_slang(word);
        const char *to_append = (replacement != NULL) ? replacement : word;

        size_t rep_len = strlen(to_append);
        if (out_idx + rep_len < out_size) {
            strcpy(&output[out_idx], to_append);
            out_idx += rep_len;
        }
    }
    output[out_idx] = '\0';
}

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE * 2];

    // 1. Create UDP Socket (SOCK_DGRAM)
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("[ERROR] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Configure Server Address Structure
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
    printf("   UDP NEW-GEN SLANG TRANSLATOR SERVER (Port: %d)    \n", PORT);
    printf("=====================================================\n");
    printf("[INFO] Server is listening for incoming UDP datagrams...\n\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_recv = recvfrom(server_fd, buffer, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr*)&client_addr, &client_len);
        if (bytes_recv < 0) {
            perror("[ERROR] recvfrom failed");
            continue;
        }
        buffer[bytes_recv] = '\0';
        buffer[strcspn(buffer, "\r\n")] = '\0';

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        int client_port = ntohs(client_addr.sin_port);

        printf("[DATAGRAM RECEIVED] From %s:%d: \"%s\"\n", client_ip, client_port, buffer);

        // Command: DICT / LIST
        if (strcasecmp(buffer, "DICT") == 0 || strcasecmp(buffer, "CMD:DICT") == 0 || strcasecmp(buffer, "LIST") == 0) {
            char dict_buf[BUFFER_SIZE];
            snprintf(dict_buf, sizeof(dict_buf),
                     "Supported Slang Abbreviations Dictionary (%d entries):\n"
                     "-------------------------------------------------------------\n",
                     NUM_SLANG);
            for (int i = 0; i < NUM_SLANG; i++) {
                char entry[128];
                snprintf(entry, sizeof(entry), "  %-8s -> %s\n", SLANG_DICT[i].abbreviation, SLANG_DICT[i].translation);
                strcat(dict_buf, entry);
            }
            strcat(dict_buf, "-------------------------------------------------------------");
            sendto(server_fd, dict_buf, strlen(dict_buf), 0, (struct sockaddr*)&client_addr, client_len);
        }
        // Command: SAMPLE
        else if (strcasecmp(buffer, "SAMPLE") == 0 || strcasecmp(buffer, "CMD:SAMPLE") == 0) {
            const char *syllabus_sample = "Really idc about this stupid server as it is of no use irl but atm, I will design one, tbf to the professor.";
            char translated[BUFFER_SIZE];
            translate_sentence(syllabus_sample, translated, sizeof(translated));
            snprintf(response, sizeof(response),
                     "[ORIGINAL]  : %s\n"
                     "[TRANSLATED]: %s",
                     syllabus_sample, translated);
            sendto(server_fd, response, strlen(response), 0, (struct sockaddr*)&client_addr, client_len);
        }
        // Command: PING
        else if (strcasecmp(buffer, "PING") == 0) {
            const char *pong = "PONG from Slang Translator Server (Port 8081)";
            sendto(server_fd, pong, strlen(pong), 0, (struct sockaddr*)&client_addr, client_len);
        }
        // General Sentence Translation
        else {
            char translated[BUFFER_SIZE];
            translate_sentence(buffer, translated, sizeof(translated));
            snprintf(response, sizeof(response), "%s", translated);

            printf("[TRANSLATION]: \"%s\"\n", response);

            sendto(server_fd, response, strlen(response), 0,
                   (struct sockaddr*)&client_addr, client_len);
            printf("[INFO] Translated datagram sent back to %s:%d\n-----------------------------------------------------\n",
                   client_ip, client_port);
        }
    }

    close(server_fd);
    return 0;
}
