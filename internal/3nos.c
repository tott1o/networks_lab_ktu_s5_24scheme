// TCP

//server.c

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int s, c, a, b, d, max;
    struct sockaddr_in addr;

    s = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8105);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&addr, sizeof(addr));
    listen(s, 5);

    printf("Waiting for client...\n");
    c = accept(s, NULL, NULL);

    recv(c, &a, sizeof(a), 0);
    recv(c, &b, sizeof(b), 0);
    recv(c, &d, sizeof(d), 0);

    if(a > b && a > d)
        max = a;
    else if(b > d)
        max = b;
    else
        max = d;

    send(c, &max, sizeof(max), 0);

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
    int s, a, b, c, max;
    struct sockaddr_in addr;

    s = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8105);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&addr, sizeof(addr));

    printf("Enter three numbers: ");
    scanf("%d%d%d", &a, &b, &c);

    send(s, &a, sizeof(a), 0);
    send(s, &b, sizeof(b), 0);
    send(s, &c, sizeof(c), 0);

    recv(s, &max, sizeof(max), 0);

    printf("Largest number = %d\n", max);

    close(s);

    return 0;
}
