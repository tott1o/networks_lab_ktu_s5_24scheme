#!/usr/bin/env python3
"""
Experiment 4 Helper: DNS Query Tool for Wireshark Capture
Course: PCCSL507 Networks Lab
Description: Sends a standard DNS query (Type A) for a domain (e.g. ktu.edu.in)
             over UDP port 53 and prints the raw query and response so students
             can observe exact DNS packets in Wireshark.
"""

import socket
import sys

DEFAULT_DOMAIN = "ktu.edu.in"
DNS_SERVER = "8.8.8.8" # Google Public DNS (or local resolver)
PORT = 53

def build_dns_query(domain, transaction_id=0x1A2B):
    # Transaction ID (2 bytes)
    header = transaction_id.to_bytes(2, byteorder="big")
    # Flags: Standard Query, Recursion Desired = 0x0100 (2 bytes)
    header += (0x0100).to_bytes(2, byteorder="big")
    # QDCOUNT = 1, ANCOUNT = 0, NSCOUNT = 0, ARCOUNT = 0 (8 bytes)
    header += (1).to_bytes(2, byteorder="big")
    header += (0).to_bytes(2, byteorder="big")
    header += (0).to_bytes(2, byteorder="big")
    header += (0).to_bytes(2, byteorder="big")

    # Question Section (QNAME)
    qname = b""
    for part in domain.split("."):
        qname += len(part).to_bytes(1, byteorder="big")
        qname += part.encode("ascii")
    qname += b"\x00" # Null terminator

    # QTYPE = 1 (A Record), QCLASS = 1 (IN - Internet)
    question = qname + (1).to_bytes(2, byteorder="big") + (1).to_bytes(2, byteorder="big")
    return header + question

def main():
    domain = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_DOMAIN
    dns_server = sys.argv[2] if len(sys.argv) > 2 else DNS_SERVER

    print("=" * 65)
    print("   EXPERIMENT 4: STANDALONE DNS QUERY SCRIPT")
    print("=" * 65)
    print(f"[*] Target Domain : {domain}")
    print(f"[*] DNS Resolver  : {dns_server}:{PORT}")
    print("[*] 1. Open Wireshark and listen on active interface (Wi-Fi/Ethernet)")
    print("[*] 2. Set display filter to: 'dns'")
    print("=" * 65)

    tx_id = 0x4A8F
    query_packet = build_dns_query(domain, transaction_id=tx_id)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(4.0)

    try:
        print(f"[*] Sending DNS Query (TxID: 0x{tx_id:04X}) to {dns_server}...")
        sock.sendto(query_packet, (dns_server, PORT))
        response, server_addr = sock.recvfrom(4096)
        print(f"[+] Received DNS Response ({len(response)} bytes) from {server_addr[0]}:{server_addr[1]}")

        # Parse basic response header
        resp_id = int.from_bytes(response[0:2], byteorder="big")
        flags = int.from_bytes(response[2:4], byteorder="big")
        qdcount = int.from_bytes(response[4:6], byteorder="big")
        ancount = int.from_bytes(response[6:8], byteorder="big")

        qr_bit = (flags >> 15) & 1
        rcode = flags & 0x0F

        print(f"    - Response TxID : 0x{resp_id:04X} (Matches Query: {resp_id == tx_id})")
        print(f"    - QR Bit        : {qr_bit} (1 = Response)")
        print(f"    - Questions     : {qdcount}")
        print(f"    - Answer RRs    : {ancount}")
        print(f"    - Response Code : {rcode} ({'NoError' if rcode == 0 else 'Error'})")
        print("[*] Inspect this query and response in Wireshark now!")
    except socket.timeout:
        print("[ERROR] DNS Query timed out. Check network connection.")
    finally:
        sock.close()

if __name__ == "__main__":
    main()
