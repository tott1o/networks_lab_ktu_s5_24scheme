#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int s, c, n, i, flag = 0;
    struct sockaddr_in a;

    s = socket(AF_INET, SOCK_STREAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&a, sizeof(a));
    listen(s, 5);

    c = accept(s, NULL, NULL);

    read(c, &n, sizeof(n));

    if(n <= 1)
        flag = 1;
    else
        for(i = 2; i <= n/2; i++)
            if(n % i == 0)
            {
                flag = 1;
                break;
            }

    if(flag == 0)
        write(c, "Prime", 6);
    else
        write(c, "Not Prime", 10);

    close(c);
    close(s);
    return 0;
}

//=============================

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int s, n;
    char b[20];
    struct sockaddr_in a;

    s = socket(AF_INET, SOCK_STREAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&a, sizeof(a));

    printf("Enter a number: ");
    scanf("%d", &n);

    write(s, &n, sizeof(n));

    read(s, b, sizeof(b));
    printf("Result: %s\n", b);

    close(s);
    return 0;
}

//=================================

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int s, n, i, flag = 0;
    char b[20];
    struct sockaddr_in a;
    socklen_t len = sizeof(a);

    s = socket(AF_INET, SOCK_DGRAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&a, sizeof(a));

    recvfrom(s, &n, sizeof(n), 0,
             (struct sockaddr *)&a, &len);

    if(n <= 1)
        flag = 1;
    else
        for(i = 2; i <= n/2; i++)
            if(n % i == 0)
            {
                flag = 1;
                break;
            }

    if(flag == 0)
        sprintf(b, "Prime");
    else
        sprintf(b, "Not Prime");

    sendto(s, b, sizeof(b), 0,
           (struct sockaddr *)&a, len);

    close(s);
    return 0;
}
//===============================

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int s, n;
    char b[20];
    struct sockaddr_in a;
    socklen_t len = sizeof(a);

    s = socket(AF_INET, SOCK_DGRAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter a number: ");
    scanf("%d", &n);

    sendto(s, &n, sizeof(n), 0,
           (struct sockaddr *)&a, len);

    recvfrom(s, b, sizeof(b), 0,
             (struct sockaddr *)&a, &len);

    printf("Result: %s\n", b);

    close(s);
    return 0;
}