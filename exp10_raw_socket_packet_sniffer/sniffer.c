/*
 * Experiment 10: Packet-Capturing Application Using Raw Sockets
 * Course: PCCSL507 - Networks Lab
 * File: sniffer.c
 * Description: Linux raw socket packet sniffer that captures link-layer Ethernet frames,
 *              dissects and displays Ethernet, IPv4, TCP, UDP, and ICMP headers,
 *              and provides a hex/ASCII payload dump.
 * Note: Requires administrative (root) privileges: run with 'sudo ./sniffer'.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

#define BUFFER_SIZE 65536

static volatile sig_atomic_t keep_running = 1;
static unsigned long total_packets = 0;
static unsigned long tcp_count = 0;
static unsigned long udp_count = 0;
static unsigned long icmp_count = 0;
static unsigned long other_count = 0;

static int filter_proto = 0; // 0: All, 1: TCP, 2: UDP, 3: ICMP

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

/* Print hex and ASCII dump of payload data */
void print_payload(const unsigned char *data, int size) {
    if (size <= 0) return;
    int limit = size > 64 ? 64 : size; // Display first 64 bytes

    printf("   Payload Preview (%d bytes, showing first %d):\n", size, limit);
    for (int i = 0; i < limit; i += 16) {
        printf("     %04X:  ", i);
        // Print Hex
        for (int j = 0; j < 16; j++) {
            if (i + j < limit) {
                printf("%02X ", data[i + j]);
            } else {
                printf("   ");
            }
        }
        printf(" |");
        // Print ASCII
        for (int j = 0; j < 16; j++) {
            if (i + j < limit) {
                unsigned char c = data[i + j];
                printf("%c", (c >= 32 && c <= 126) ? c : '.');
            }
        }
        printf("|\n");
    }
}

/* Process and dissect incoming packet */
void process_packet(const unsigned char *buffer, int size) {
    if (size < (int)sizeof(struct ethhdr)) return;

    struct ethhdr *eth = (struct ethhdr *)buffer;

    // Check if IPv4 (0x0800)
    if (ntohs(eth->h_proto) != ETH_P_IP) {
        if (filter_proto == 0) {
            total_packets++;
            other_count++;
        }
        return;
    }

    int eth_len = sizeof(struct ethhdr);
    struct iphdr *ip = (struct iphdr *)(buffer + eth_len);
    int ip_header_len = ip->ihl * 4;

    // Apply protocol filter
    if (filter_proto == 1 && ip->protocol != IPPROTO_TCP) return;
    if (filter_proto == 2 && ip->protocol != IPPROTO_UDP) return;
    if (filter_proto == 3 && ip->protocol != IPPROTO_ICMP) return;

    total_packets++;
    printf("\n===============================================================\n");
    printf("[PACKET #%lu] Captured %d bytes on wire\n", total_packets, size);
    printf("===============================================================\n");

    // 1. Ethernet Header
    printf("[ETHERNET HEADER]\n");
    printf("   Source MAC     : %02X:%02X:%02X:%02X:%02X:%02X\n",
           eth->h_source[0], eth->h_source[1], eth->h_source[2],
           eth->h_source[3], eth->h_source[4], eth->h_source[5]);
    printf("   Destination MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
           eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
           eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
    printf("   Type           : IPv4 (0x0800)\n");

    // 2. IP Header
    struct in_addr src_ip, dst_ip;
    src_ip.s_addr = ip->saddr;
    dst_ip.s_addr = ip->daddr;

    printf("[INTERNET PROTOCOL (IPv4)]\n");
    printf("   Header Length  : %d bytes (IHL: %d)\n", ip_header_len, ip->ihl);
    printf("   Total Length   : %d bytes\n", ntohs(ip->tot_len));
    printf("   TTL            : %d\n", ip->ttl);
    printf("   Source IP      : %s\n", inet_ntoa(src_ip));
    printf("   Destination IP : %s\n", inet_ntoa(dst_ip));

    const unsigned char *transport_data = buffer + eth_len + ip_header_len;
    int transport_len = size - (eth_len + ip_header_len);

    // 3. Transport Layer
    if (ip->protocol == IPPROTO_TCP) {
        tcp_count++;
        struct tcphdr *tcp = (struct tcphdr *)transport_data;
        int tcp_header_len = tcp->doff * 4;

        printf("[TRANSMISSION CONTROL PROTOCOL (TCP)]\n");
        printf("   Source Port      : %u\n", ntohs(tcp->source));
        printf("   Destination Port : %u\n", ntohs(tcp->dest));
        printf("   Sequence Number  : %u\n", ntohl(tcp->seq));
        printf("   Ack Number       : %u\n", ntohl(tcp->ack_seq));
        printf("   Header Length    : %d bytes\n", tcp_header_len);
        printf("   Flags            : SYN:%d ACK:%d FIN:%d RST:%d PSH:%d URG:%d\n",
               tcp->syn, tcp->ack, tcp->fin, tcp->rst, tcp->psh, tcp->urg);

        const unsigned char *payload = transport_data + tcp_header_len;
        int payload_len = transport_len - tcp_header_len;
        print_payload(payload, payload_len);

    } else if (ip->protocol == IPPROTO_UDP) {
        udp_count++;
        struct udphdr *udp = (struct udphdr *)transport_data;
        int udp_header_len = sizeof(struct udphdr);

        printf("[USER DATAGRAM PROTOCOL (UDP)]\n");
        printf("   Source Port      : %u\n", ntohs(udp->source));
        printf("   Destination Port : %u\n", ntohs(udp->dest));
        printf("   Datagram Length  : %u bytes\n", ntohs(udp->len));
        printf("   Checksum         : 0x%04X\n", ntohs(udp->check));

        const unsigned char *payload = transport_data + udp_header_len;
        int payload_len = transport_len - udp_header_len;
        print_payload(payload, payload_len);

    } else if (ip->protocol == IPPROTO_ICMP) {
        icmp_count++;
        struct icmphdr *icmp = (struct icmphdr *)transport_data;

        printf("[INTERNET CONTROL MESSAGE PROTOCOL (ICMP)]\n");
        printf("   Type     : %u ", icmp->type);
        if (icmp->type == ICMP_ECHOREPLY) printf("(Echo Reply)\n");
        else if (icmp->type == ICMP_ECHO) printf("(Echo Request)\n");
        else printf("(Other)\n");
        printf("   Code     : %u\n", icmp->code);
        printf("   Checksum : 0x%04X\n", ntohs(icmp->checksum));

        print_payload(transport_data + sizeof(struct icmphdr), transport_len - sizeof(struct icmphdr));

    } else {
        other_count++;
        printf("[OTHER TRANSPORT PROTOCOL: %d]\n", ip->protocol);
    }
}

int main(int argc, char *argv[]) {
    int sock_raw;
    unsigned char buffer[BUFFER_SIZE];
    int max_packets = 10;

    printf("===============================================================\n");
    printf("   RAW SOCKET PACKET CAPTURING APPLICATION - EXPERIMENT 10     \n");
    printf("===============================================================\n");

    if (argc >= 2) {
        max_packets = atoi(argv[1]);
        if (max_packets <= 0) max_packets = 10;
        if (argc >= 3) filter_proto = atoi(argv[2]);
    } else if (isatty(STDIN_FILENO)) {
        printf("Enter number of packets to capture [default 10]: ");
        char pbuf[32];
        if (fgets(pbuf, sizeof(pbuf), stdin) && pbuf[0] != '\n') {
            int val = atoi(pbuf);
            if (val > 0) max_packets = val;
        }

        printf("Select protocol filter:\n  0. All Protocols (default)\n  1. TCP only\n  2. UDP only\n  3. ICMP only\nEnter choice (0-3): ");
        if (fgets(pbuf, sizeof(pbuf), stdin) && pbuf[0] != '\n') {
            int f = atoi(pbuf);
            if (f >= 0 && f <= 3) filter_proto = f;
        }
    }

    signal(SIGINT, handle_sigint);

    // 1. Create Raw Socket for all link-layer protocols (ETH_P_ALL)
    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0) {
        perror("[ERROR] Raw socket creation failed. Ensure you run with 'sudo' privileges");
        return 1;
    }

    const char *filter_names[] = {"All Protocols", "TCP only", "UDP only", "ICMP only"};
    printf("\n[INFO] Sniffing on all interfaces (Filter: %s)...\n", filter_names[filter_proto]);
    printf("[INFO] Capturing %d packets (Press Ctrl+C to stop early)\n\n", max_packets);

    while (keep_running && (int)total_packets < max_packets) {
        int packet_size = recvfrom(sock_raw, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (packet_size < 0) {
            if (!keep_running) break;
            perror("[ERROR] Packet receive failed");
            break;
        }
        process_packet(buffer, packet_size);
    }

    printf("\n===============================================================\n");
    printf("                 PACKET CAPTURE SUMMARY                        \n");
    printf("===============================================================\n");
    printf("   Total Packets Processed : %lu\n", total_packets);
    printf("   TCP Packets             : %lu\n", tcp_count);
    printf("   UDP Packets             : %lu\n", udp_count);
    printf("   ICMP Packets            : %lu\n", icmp_count);
    printf("   Other / Non-IP Packets  : %lu\n", other_count);
    printf("===============================================================\n");

    close(sock_raw);
    return 0;
}
