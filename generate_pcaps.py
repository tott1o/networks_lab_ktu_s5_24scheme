#!/usr/bin/env python3
"""
PCAP Generator for KTU Networks Lab Wireshark Experiments
Generates standard libpcap files for Experiments 2, 3, and 4
so students can immediately open and inspect them in Wireshark.
"""

import struct
import time
import socket

def ip_checksum(data):
    if len(data) % 2 != 0:
        data += b'\x00'
    s = sum(struct.unpack('!%dH' % (len(data) // 2), data))
    s = (s >> 16) + (s & 0xFFFF)
    s += (s >> 16)
    return (~s) & 0xFFFF

def tcp_checksum(src_ip, dst_ip, tcp_bytes):
    src_bytes = socket.inet_aton(src_ip)
    dst_bytes = socket.inet_aton(dst_ip)
    length = len(tcp_bytes)
    pseudo = src_bytes + dst_bytes + struct.pack('!BBH', 0, 6, length) + tcp_bytes
    return ip_checksum(pseudo)

def udp_checksum(src_ip, dst_ip, udp_bytes):
    src_bytes = socket.inet_aton(src_ip)
    dst_bytes = socket.inet_aton(dst_ip)
    length = len(udp_bytes)
    pseudo = src_bytes + dst_bytes + struct.pack('!BBH', 0, 17, length) + udp_bytes
    return ip_checksum(pseudo)

def make_eth_header(src_mac, dst_mac, eth_type=0x0800):
    src_b = bytes.fromhex(src_mac.replace(':', ''))
    dst_b = bytes.fromhex(dst_mac.replace(':', ''))
    return dst_b + src_b + struct.pack('!H', eth_type)

def make_ip_header(src_ip, dst_ip, proto, payload_len, ip_id=1000, ttl=64):
    version_ihl = 0x45 # IPv4, 5 words (20 bytes)
    tos = 0
    tot_len = 20 + payload_len
    flags_frag = 0x4000 # Don't fragment
    check = 0
    src_b = socket.inet_aton(src_ip)
    dst_b = socket.inet_aton(dst_ip)
    raw = struct.pack('!BBHHHBBH4s4s', version_ihl, tos, tot_len, ip_id, flags_frag, ttl, proto, check, src_b, dst_b)
    check = ip_checksum(raw)
    return struct.pack('!BBHHHBBH4s4s', version_ihl, tos, tot_len, ip_id, flags_frag, ttl, proto, check, src_b, dst_b)

def make_tcp_packet(src_ip, dst_ip, src_port, dst_port, seq, ack, flags, payload=b''):
    data_offset = 5 # 20 bytes
    offset_flags = (data_offset << 12) | flags
    window = 64240
    urg_ptr = 0
    check = 0
    raw_tcp = struct.pack('!HHIIHHHH', src_port, dst_port, seq, ack, offset_flags, window, check, urg_ptr) + payload
    check = tcp_checksum(src_ip, dst_ip, raw_tcp)
    tcp_hdr = struct.pack('!HHIIHHHH', src_port, dst_port, seq, ack, offset_flags, window, check, urg_ptr)
    return tcp_hdr + payload

def make_udp_packet(src_ip, dst_ip, src_port, dst_port, payload=b''):
    length = 8 + len(payload)
    check = 0
    raw_udp = struct.pack('!HHHH', src_port, dst_port, length, check) + payload
    check = udp_checksum(src_ip, dst_ip, raw_udp)
    if check == 0: check = 0xFFFF
    return struct.pack('!HHHH', src_port, dst_port, length, check) + payload

class PcapWriter:
    def __init__(self, filename):
        self.f = open(filename, 'wb')
        # Pcap Global Header: magic 0xa1b2c3d4, v2.4, thiszone 0, sigfigs 0, snaplen 65535, network 1 (Ethernet)
        self.f.write(struct.pack('!IHHiIII', 0xa1b2c3d4, 2, 4, 0, 0, 65535, 1))

    def write_packet(self, pkt_bytes, sec=None, usec=None):
        if sec is None:
            now = time.time()
            sec = int(now)
            usec = int((now - sec) * 1000000)
        length = len(pkt_bytes)
        hdr = struct.pack('!IIII', sec, usec, length, length)
        self.f.write(hdr + pkt_bytes)

    def close(self):
        self.f.close()

def build_http_pcap(filepath):
    writer = PcapWriter(filepath)
    client_mac = "00:15:5d:89:12:01"
    server_mac = "52:54:00:12:35:02"
    client_ip = "192.168.1.15"
    server_ip = "128.119.245.12"
    client_port = 54320
    server_port = 80
    base_t = 1775560000 # Base timestamp

    # SYN
    eth = make_eth_header(client_mac, server_mac)
    tcp = make_tcp_packet(client_ip, server_ip, client_port, server_port, 100, 0, 0x02) # SYN
    ip = make_ip_header(client_ip, server_ip, 6, len(tcp), ip_id=101)
    writer.write_packet(eth + ip + tcp, base_t, 100000)

    # SYN-ACK
    eth = make_eth_header(server_mac, client_mac)
    tcp = make_tcp_packet(server_ip, client_ip, server_port, client_port, 500, 101, 0x12) # SYN-ACK
    ip = make_ip_header(server_ip, client_ip, 6, len(tcp), ip_id=201)
    writer.write_packet(eth + ip + tcp, base_t, 140000)

    # ACK
    eth = make_eth_header(client_mac, server_mac)
    tcp = make_tcp_packet(client_ip, server_ip, client_port, server_port, 101, 501, 0x10) # ACK
    ip = make_ip_header(client_ip, server_ip, 6, len(tcp), ip_id=102)
    writer.write_packet(eth + ip + tcp, base_t, 150000)

    # HTTP GET Request
    http_get = (
        b"GET /wireshark-labs/HTTP-wireshark-file1.html HTTP/1.1\r\n"
        b"Host: gaia.cs.umass.edu\r\n"
        b"Connection: keep-alive\r\n"
        b"Upgrade-Insecure-Requests: 1\r\n"
        b"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36\r\n"
        b"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n"
        b"Accept-Encoding: gzip, deflate\r\n"
        b"Accept-Language: en-US,en;q=0.9\r\n"
        b"Accept-Charset: utf-8\r\n\r\n"
    )
    tcp = make_tcp_packet(client_ip, server_ip, client_port, server_port, 101, 501, 0x18, http_get) # PSH, ACK
    ip = make_ip_header(client_ip, server_ip, 6, len(tcp), ip_id=103)
    writer.write_packet(eth + ip + tcp, base_t, 200000)

    # TCP ACK from Server
    eth = make_eth_header(server_mac, client_mac)
    tcp = make_tcp_packet(server_ip, client_ip, server_port, client_port, 501, 101 + len(http_get), 0x10) # ACK
    ip = make_ip_header(server_ip, client_ip, 6, len(tcp), ip_id=202)
    writer.write_packet(eth + ip + tcp, base_t, 240000)

    # HTTP 200 OK Response
    html_body = b"<html>\nCongratulations! You have successfully retrieved this text file from the gaia.cs.umass.edu web server.\n</html>\n"
    http_resp = (
        b"HTTP/1.1 200 OK\r\n"
        b"Date: Wed, 07 Oct 2026 12:00:00 GMT\r\n"
        b"Server: Apache/2.4.52 (Ubuntu)\r\n"
        b"Last-Modified: Mon, 05 Oct 2026 12:00:00 GMT\r\n"
        b"ETag: \"80-5a3d0b2f\"\r\n"
        b"Accept-Ranges: bytes\r\n"
        b"Content-Length: " + str(len(html_body)).encode() + b"\r\n"
        b"Vary: Accept-Encoding\r\n"
        b"Keep-Alive: timeout=5, max=100\r\n"
        b"Connection: Keep-Alive\r\n"
        b"Content-Type: text/html; charset=UTF-8\r\n\r\n" + html_body
    )
    tcp = make_tcp_packet(server_ip, client_ip, server_port, client_port, 501, 101 + len(http_get), 0x18, http_resp) # PSH, ACK
    ip = make_ip_header(server_ip, client_ip, 6, len(tcp), ip_id=203)
    writer.write_packet(eth + ip + tcp, base_t, 284120)

    # Final ACK from client
    eth = make_eth_header(client_mac, server_mac)
    tcp = make_tcp_packet(client_ip, server_ip, client_port, server_port, 101 + len(http_get), 501 + len(http_resp), 0x10) # ACK
    ip = make_ip_header(client_ip, server_ip, 6, len(tcp), ip_id=104)
    writer.write_packet(eth + ip + tcp, base_t, 290000)

    writer.close()
    print(f"[+] Successfully generated {filepath}")

def build_smtp_pcap(filepath):
    writer = PcapWriter(filepath)
    client_mac = "00:15:5d:89:12:01"
    server_mac = "52:54:00:12:35:02"
    client_ip = "192.168.1.15"
    server_ip = "172.16.1.5"
    client_port = 52140
    server_port = 25
    base_t = 1775560100

    # SYN
    eth = make_eth_header(client_mac, server_mac)
    tcp = make_tcp_packet(client_ip, server_ip, client_port, server_port, 1000, 0, 0x02)
    ip = make_ip_header(client_ip, server_ip, 6, len(tcp), ip_id=301)
    writer.write_packet(eth + ip + tcp, base_t, 10000)

    # SYN-ACK
    eth = make_eth_header(server_mac, client_mac)
    tcp = make_tcp_packet(server_ip, client_ip, server_port, client_port, 2000, 1001, 0x12)
    ip = make_ip_header(server_ip, client_ip, 6, len(tcp), ip_id=401)
    writer.write_packet(eth + ip + tcp, base_t, 15000)

    # ACK
    eth = make_eth_header(client_mac, server_mac)
    tcp = make_tcp_packet(client_ip, server_ip, client_port, server_port, 1001, 2001, 0x10)
    ip = make_ip_header(client_ip, server_ip, 6, len(tcp), ip_id=302)
    writer.write_packet(eth + ip + tcp, base_t, 20000)

    # 220 Banner from server
    banner = b"220 mail.collegelabs.edu ESMTP Postfix Service Ready\r\n"
    eth = make_eth_header(server_mac, client_mac)
    tcp = make_tcp_packet(server_ip, client_ip, server_port, client_port, 2001, 1001, 0x18, banner)
    ip = make_ip_header(server_ip, client_ip, 6, len(tcp), ip_id=402)
    writer.write_packet(eth + ip + tcp, base_t, 30000)

    # HELO from client
    helo = b"HELO student-laptop.local\r\n"
    eth = make_eth_header(client_mac, server_mac)
    tcp = make_tcp_packet(client_ip, server_ip, client_port, server_port, 1001, 2001 + len(banner), 0x18, helo)
    ip = make_ip_header(client_ip, server_ip, 6, len(tcp), ip_id=303)
    writer.write_packet(eth + ip + tcp, base_t, 45000)

    # 250 Response
    r250_helo = b"250 mail.collegelabs.edu Hello [192.168.1.15], pleased to meet you\r\n"
    eth = make_eth_header(server_mac, client_mac)
    tcp = make_tcp_packet(server_ip, client_ip, server_port, client_port, 2001 + len(banner), 1001 + len(helo), 0x18, r250_helo)
    ip = make_ip_header(server_ip, client_ip, 6, len(tcp), ip_id=403)
    writer.write_packet(eth + ip + tcp, base_t, 60000)

    # MAIL FROM
    mail_from = b"MAIL FROM:<student@collegelabs.edu>\r\n"
    eth = make_eth_header(client_mac, server_mac)
    c_seq = 1001 + len(helo)
    s_seq = 2001 + len(banner) + len(r250_helo)
    tcp = make_tcp_packet(client_ip, server_ip, client_port, server_port, c_seq, s_seq, 0x18, mail_from)
    ip = make_ip_header(client_ip, server_ip, 6, len(tcp), ip_id=304)
    writer.write_packet(eth + ip + tcp, base_t, 75000)

    # 250 Sender OK
    r250_mail = b"250 2.1.0 Sender OK\r\n"
    eth = make_eth_header(server_mac, client_mac)
    tcp = make_tcp_packet(server_ip, client_ip, server_port, client_port, s_seq, c_seq + len(mail_from), 0x18, r250_mail)
    ip = make_ip_header(server_ip, client_ip, 6, len(tcp), ip_id=404)
    writer.write_packet(eth + ip + tcp, base_t, 90000)

    # RCPT TO
    rcpt_to = b"RCPT TO:<student@collegelabs.edu>\r\n"
    c_seq += len(mail_from)
    s_seq += len(r250_mail)
    eth = make_eth_header(client_mac, server_mac)
    tcp = make_tcp_packet(client_ip, server_ip, client_port, server_port, c_seq, s_seq, 0x18, rcpt_to)
    ip = make_ip_header(client_ip, server_ip, 6, len(tcp), ip_id=305)
    writer.write_packet(eth + ip + tcp, base_t, 105000)

    # 250 Recipient OK
    r250_rcpt = b"250 2.1.5 Recipient OK\r\n"
    eth = make_eth_header(server_mac, client_mac)
    tcp = make_tcp_packet(server_ip, client_ip, server_port, client_port, s_seq, c_seq + len(rcpt_to), 0x18, r250_rcpt)
    ip = make_ip_header(server_ip, client_ip, 6, len(tcp), ip_id=405)
    writer.write_packet(eth + ip + tcp, base_t, 120000)

    # DATA
    data_cmd = b"DATA\r\n"
    c_seq += len(rcpt_to)
    s_seq += len(r250_rcpt)
    eth = make_eth_header(client_mac, server_mac)
    tcp = make_tcp_packet(client_ip, server_ip, client_port, server_port, c_seq, s_seq, 0x18, data_cmd)
    ip = make_ip_header(client_ip, server_ip, 6, len(tcp), ip_id=306)
    writer.write_packet(eth + ip + tcp, base_t, 135000)

    # 354 Start Mail Input
    r354 = b"354 Start mail input; end with <CRLF>.<CRLF>\r\n"
    eth = make_eth_header(server_mac, client_mac)
    tcp = make_tcp_packet(server_ip, client_ip, server_port, client_port, s_seq, c_seq + len(data_cmd), 0x18, r354)
    ip = make_ip_header(server_ip, client_ip, 6, len(tcp), ip_id=406)
    writer.write_packet(eth + ip + tcp, base_t, 150000)

    # Encapsulated IMF (RFC 5322) Message
    imf_body = (
        b"From: \"Student User\" <student@collegelabs.edu>\r\n"
        b"To: \"Student User\" <student@collegelabs.edu>\r\n"
        b"Date: Wed, 07 Oct 2026 12:00:00 +0530\r\n"
        b"Subject: KTU S5 Networks Lab Experiment 3 SMTP Test\r\n"
        b"Message-ID: <20261007.KTU.PCCSL507@collegelabs.edu>\r\n"
        b"MIME-Version: 1.0\r\n"
        b"Content-Type: text/plain; charset=utf-8\r\n\r\n"
        b"Hello!\r\n"
        b"This is an automated test email addressed to myself for Experiment 3.\r\n"
        b"Demonstrating SMTP command sequence and IMF encapsulation in Wireshark.\r\n\r\n"
        b".\r\n"
    )
    c_seq += len(data_cmd)
    s_seq += len(r354)
    eth = make_eth_header(client_mac, server_mac)
    tcp = make_tcp_packet(client_ip, server_ip, client_port, server_port, c_seq, s_seq, 0x18, imf_body)
    ip = make_ip_header(client_ip, server_ip, 6, len(tcp), ip_id=307)
    writer.write_packet(eth + ip + tcp, base_t, 175000)

    # 250 Queued
    r250_done = b"250 2.0.0 Ok: queued as 4X8912KL\r\n"
    eth = make_eth_header(server_mac, client_mac)
    tcp = make_tcp_packet(server_ip, client_ip, server_port, client_port, s_seq, c_seq + len(imf_body), 0x18, r250_done)
    ip = make_ip_header(server_ip, client_ip, 6, len(tcp), ip_id=407)
    writer.write_packet(eth + ip + tcp, base_t, 195000)

    # QUIT
    quit_cmd = b"QUIT\r\n"
    c_seq += len(imf_body)
    s_seq += len(r250_done)
    eth = make_eth_header(client_mac, server_mac)
    tcp = make_tcp_packet(client_ip, server_ip, client_port, server_port, c_seq, s_seq, 0x18, quit_cmd)
    ip = make_ip_header(client_ip, server_ip, 6, len(tcp), ip_id=308)
    writer.write_packet(eth + ip + tcp, base_t, 210000)

    # 221 Bye
    r221 = b"221 2.0.0 mail.collegelabs.edu Service closing transmission channel\r\n"
    eth = make_eth_header(server_mac, client_mac)
    tcp = make_tcp_packet(server_ip, client_ip, server_port, client_port, s_seq, c_seq + len(quit_cmd), 0x18, r221)
    ip = make_ip_header(server_ip, client_ip, 6, len(tcp), ip_id=408)
    writer.write_packet(eth + ip + tcp, base_t, 225000)

    writer.close()
    print(f"[+] Successfully generated {filepath}")

def build_dns_pcap(filepath):
    writer = PcapWriter(filepath)
    client_mac = "00:15:5d:89:12:01"
    server_mac = "52:54:00:12:35:02"
    client_ip = "192.168.1.15"
    dns_server_ip = "8.8.8.8"
    client_port = 54219
    dns_port = 53
    base_t = 1775560200

    tx_id = 0x4A8F
    # DNS Question QNAME for ktu.edu.in
    qname = b"\x03ktu\x03edu\x02in\x00"

    # DNS Query Packet: TxID=0x4A8F, Flags=0x0100 (Standard Query, RD=1), QD=1, AN=0, NS=0, AR=0
    dns_query_payload = struct.pack('!HHHHHH', tx_id, 0x0100, 1, 0, 0, 0) + qname + struct.pack('!HH', 1, 1) # Type A, Class IN
    eth = make_eth_header(client_mac, server_mac)
    udp = make_udp_packet(client_ip, dns_server_ip, client_port, dns_port, dns_query_payload)
    ip = make_ip_header(client_ip, dns_server_ip, 17, len(udp), ip_id=501)
    writer.write_packet(eth + ip + udp, base_t, 145020)

    # DNS Response Packet: TxID=0x4A8F, Flags=0x8180 (Response, RD=1, RA=1, NoError=0), QD=1, AN=1, NS=0, AR=0
    # Answer RR: Name pointer 0xc00c, Type A (1), Class IN (1), TTL 300 (4 bytes), RDLENGTH 4 (2 bytes), IP 14.139.185.155 (4 bytes)
    rdata_ip = socket.inet_aton("14.139.185.155")
    answer_rr = struct.pack('!HHHIH', 0xc00c, 1, 1, 300, 4) + rdata_ip
    dns_resp_payload = struct.pack('!HHHHHH', tx_id, 0x8180, 1, 1, 0, 0) + qname + struct.pack('!HH', 1, 1) + answer_rr

    eth = make_eth_header(server_mac, client_mac)
    udp = make_udp_packet(dns_server_ip, client_ip, dns_port, client_port, dns_resp_payload)
    ip = make_ip_header(dns_server_ip, client_ip, 17, len(udp), ip_id=601)
    writer.write_packet(eth + ip + udp, base_t, 189340)

    writer.close()
    print(f"[+] Successfully generated {filepath}")

if __name__ == '__main__':
    build_http_pcap("exp02_wireshark_http/http_traffic.pcap")
    build_smtp_pcap("exp03_wireshark_smtp/smtp_traffic.pcap")
    build_dns_pcap("exp04_wireshark_dns/dns_traffic.pcap")
