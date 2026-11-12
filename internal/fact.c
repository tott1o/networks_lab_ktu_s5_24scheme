#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main(
{
    int s, c, n, i;
    long long fact = 1;
    struct sockaddr_in a;

    s = socket(AF_INET, SOCK_STREAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&a, sizeof(a));
    listen(s, 1);

    printf("Waiting for client...\n");
    c = accept(s, NULL, NULL);

    read(c, &n, sizeof(n));

    for(i = 1; i <= n; i++)
        fact = fact * i;

    write(c, &fact, sizeof(fact));

    close(c);
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
    int s, n;
    long long fact;
    struct sockaddr_in a;

    s = socket(AF_INET, SOCK_STREAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(s, (struct sockaddr *)&a, sizeof(a));

    printf("Enter a number: ");
    scanf("%d", &n);

    write(s, &n, sizeof(n));

    read(s, &fact, sizeof(fact));

    printf("Factorial = %lld\n", fact);

    close(s);

    return 0;
}

//====================================


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int s, n, i;
    long long fact = 1;
    struct sockaddr_in a, c;
    socklen_t l = sizeof(c);

    s = socket(AF_INET, SOCK_DGRAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&a, sizeof(a));

    recvfrom(s, &n, sizeof(n), 0,
             (struct sockaddr *)&c, &l);

    for(i = 1; i <= n; i++)
        fact = fact * i;

    sendto(s, &fact, sizeof(fact), 0,
           (struct sockaddr *)&c, l);

    close(s);

    return 0;
}

//================================

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main()
{
    int s, n;
    long long fact;
    struct sockaddr_in a;
    socklen_t l = sizeof(a);

    s = socket(AF_INET, SOCK_DGRAM, 0);

    a.sin_family = AF_INET;
    a.sin_port = htons(8105);
    a.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter a number: ");
    scanf("%d", &n);

    sendto(s, &n, sizeof(n), 0,
           (struct sockaddr *)&a, l);

    recvfrom(s, &fact, sizeof(fact), 0,
             (struct sockaddr *)&a, &l);

    printf("Factorial = %lld\n", fact);

    close(s);

    return 0;
}

