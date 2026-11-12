// Raw Socket Packet Sniffer
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
int main()
{
    int lfd, n, i;
    char rBuf[65536];
    struct sockaddr saddr;
    struct ethhdr *eth;
    struct iphdr *ip;
    struct in_addr src, dst;

    lfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (lfd < 0) {
        printf("\nSocket creation failed! (Run as root / sudo)\n");
        return 1;
    }
    printf("\nRaw Sniffer ready, capturing 5 packets....\n\n");

    for (i = 1; i <= 5; i++) {
        n = sizeof saddr;
        recvfrom(lfd, rBuf, sizeof rBuf, 0, &saddr, &n);

        eth = (struct ethhdr *)rBuf;
        printf("Packet #%d:\n", i);
        printf("  MAC: %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X\n",
               eth->h_source[0], eth->h_source[1], eth->h_source[2],
               eth->h_source[3], eth->h_source[4], eth->h_source[5],
               eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
               eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);

        if (ntohs(eth->h_proto) == ETH_P_IP) {
            ip = (struct iphdr *)(rBuf + sizeof(struct ethhdr));
            src.s_addr = ip->saddr;
            dst.s_addr = ip->daddr;
            printf("  IP : %s -> %s (Protocol: %d)\n",
                   inet_ntoa(src), inet_ntoa(dst), ip->protocol);
        }
        printf("\n");
    }

    close(lfd);
    return 0;
}