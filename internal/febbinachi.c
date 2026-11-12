//server.c=============================================

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main() {
    int s, c, n, a = 0, b = 1, t;
    int arr[100];

    struct sockaddr_in server;

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8105);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));
    listen(s, 5);

    c = accept(s, NULL, NULL);

    read(c, &n, sizeof(n));

    for(int i = 0; i < n; i++) {
        arr[i] = a;
        t = a + b;
        a = b;
        b = t;
    }

    write(c, arr, n * sizeof(int));

    close(c);
    close(s);

    return 0;
}

//client.c=============================================

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main() {
    int s, n, arr[100];

    struct sockaddr_in server;

    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8105);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&server, sizeof(server));

    printf("Enter number of terms: ");
    scanf("%d", &n);

    write(s, &n, sizeof(n));

    read(s, arr, n * sizeof(int));

    printf("Fibonacci Series: ");
    for(int i = 0; i < n; i++)
        printf("%d ", arr[i]);

    close(s);

    return 0;
}

//UDP=======================================================================

//server.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main() {
    int s, n, a = 0, b = 1, t;
    int arr[100];

    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8105);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&server, sizeof(server));

    recvfrom(s, &n, sizeof(n), 0,
             (struct sockaddr *)&client, &len);

    for(int i = 0; i < n; i++) {
        arr[i] = a;
        t = a + b;
        a = b;
        b = t;
    }

    sendto(s, arr, n * sizeof(int), 0,
           (struct sockaddr *)&client, len);

    close(s);

    return 0;
}

//client.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main() {
    int s, n, arr[100];

    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    s = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(8105);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter number of terms: ");
    scanf("%d", &n);

    sendto(s, &n, sizeof(n), 0,
           (struct sockaddr *)&server, len);

    recvfrom(s, arr, n * sizeof(int), 0,
             (struct sockaddr *)&server, &len);

    printf("Fibonacci Series: ");
    for(int i = 0; i < n; i++)
        printf("%d ", arr[i]);

    close(s);

    return 0;
}
