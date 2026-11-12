// TCP server.c
//=====================
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int s, c, i, n;
    char b[100], temp;
    struct sockaddr_in a;

    s = socket(AF_INET, SOCK_STREAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&a, sizeof(a));
    listen(s, 1);

    printf("Waiting for client...\n");
    c = accept(s, NULL, NULL);

    n = recv(c, b, sizeof(b) - 1, 0);
    if (n > 0) {
        b[n] = '\0';

        for(i = 0; i < n/2; i++) {
            temp = b[i];
            b[i] = b[n-i-1];
            b[n-i-1] = temp;
        }

        send(c, b, strlen(b), 0);
    }

    close(c);
    close(s);

    return 0;
}

// TCP client.c
//===================================
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int s, n;
    char b[100];
    struct sockaddr_in a;

    s = socket(AF_INET, SOCK_STREAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&a, sizeof(a));

    printf("Enter a string: ");
    fgets(b, sizeof(b), stdin);
    b[strcspn(b, "\n")] = '\0';

    send(s, b, strlen(b), 0);

    n = recv(s, b, sizeof(b) - 1, 0);
    if (n > 0) {
        b[n] = '\0';
        printf("Reversed string: %s\n", b);
    }

    close(s);

    return 0;
}

// UDP server.c
//===========================
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int s, i, n;
    char b[100], temp;
    struct sockaddr_in a;
    socklen_t len = sizeof(a);

    s = socket(AF_INET, SOCK_DGRAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&a, sizeof(a));

    printf("Waiting for client...\n");

    n = recvfrom(s, b, sizeof(b) - 1, 0, (struct sockaddr *)&a, &len);
    if (n > 0) {
        b[n] = '\0';

        for(i = 0; i < n/2; i++) {
            temp = b[i];
            b[i] = b[n-i-1];
            b[n-i-1] = temp;
        }

        sendto(s, b, strlen(b), 0, (struct sockaddr *)&a, len);
    }

    close(s);

    return 0;
}

// UDP client.c
//===========================
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int s, n;
    char b[100];
    struct sockaddr_in a;
    socklen_t len = sizeof(a);

    s = socket(AF_INET, SOCK_DGRAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter a string: ");
    fgets(b, sizeof(b), stdin);
    b[strcspn(b, "\n")] = '\0';

    sendto(s, b, strlen(b), 0, (struct sockaddr *)&a, len);

    n = recvfrom(s, b, sizeof(b) - 1, 0, (struct sockaddr *)&a, &len);
    if (n > 0) {
        b[n] = '\0';
        printf("Reversed string: %s\n", b);
    }

    close(s);

    return 0;
}
