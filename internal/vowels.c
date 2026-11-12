//TCP
//server.c=============
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int s, c, i, count = 0;
    char b[500];
    struct sockaddr_in a;

    s = socket(AF_INET, SOCK_STREAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&a, sizeof(a));
    listen(s, 5);

    c = accept(s, NULL, NULL);

    recv(c, b, sizeof(b), 0);

    for(i = 0; b[i] != '\0'; i++)
    {
        if(strchr("aeiouAEIOU", b[i]))
            count++;
    }

    send(c, &count, sizeof(count), 0);

    close(c);
    close(s);

    return 0;
}

//client.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int s, count;
    char b[500];
    struct sockaddr_in a;

    s = socket(AF_INET, SOCK_STREAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&a, sizeof(a));

    printf("Enter a string: ");
    fgets(b, sizeof(b), stdin);

    send(s, b, strlen(b) + 1, 0);

    recv(s, &count, sizeof(count), 0);

    printf("Number of vowels = %d\n", count);

    close(s);

    return 0;
}

//UDP=============================================================

//server.c


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int s, i, count = 0;
    char b[500];
    struct sockaddr_in a;
    socklen_t len = sizeof(a);

    s = socket(AF_INET, SOCK_DGRAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&a, sizeof(a));

    recvfrom(s, b, sizeof(b), 0,
             (struct sockaddr *)&a, &len);

    for(i = 0; b[i] != '\0'; i++)
    {
        if(strchr("aeiouAEIOU", b[i]))
            count++;
    }

    sendto(s, &count, sizeof(count), 0,
           (struct sockaddr *)&a, len);

    close(s);

    return 0;
}

//client.c


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int s, count;
    char b[500];
    struct sockaddr_in a;
    socklen_t len = sizeof(a);

    s = socket(AF_INET, SOCK_DGRAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter a string: ");
    fgets(b, sizeof(b), stdin);

    sendto(s, b, strlen(b) + 1, 0,
           (struct sockaddr *)&a, len);

    recvfrom(s, &count, sizeof(count), 0,
             (struct sockaddr *)&a, &len);

    printf("Number of vowels = %d\n", count);

    close(s);

    return 0;
}


