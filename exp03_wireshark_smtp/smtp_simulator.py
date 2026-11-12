#!/usr/bin/env python3
"""
Experiment 3 Helper: Local SMTP Simulator for Wireshark Capture
Course: PCCSL507 Networks Lab
Description: Runs an unencrypted mock SMTP server on 127.0.0.1:2525 and dispatches
             an RFC 5322 email message so students can capture and analyze pure SMTP
             and IMF (Internet Message Format) packets in Wireshark without TLS encryption.
"""

import socket
import threading
import time

SERVER_HOST = "127.0.0.1"
SERVER_PORT = 2525

def run_smtp_server():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((SERVER_HOST, SERVER_PORT))
    server.listen(1)
    print(f"[*] SMTP Server listening on {SERVER_HOST}:{SERVER_PORT}")

    conn, addr = server.accept()
    print(f"[+] Client connected from {addr[0]}:{addr[1]}")

    # 1. 220 Service Ready Banner
    conn.sendall(b"220 mail.collegelabs.edu Simple Mail Transfer Service Ready\r\n")

    while True:
        data = conn.recv(1024)
        if not data:
            break
        msg = data.decode("utf-8", errors="replace")

        if msg.upper().startswith("HELO") or msg.upper().startswith("EHLO"):
            conn.sendall(b"250 mail.collegelabs.edu Hello [127.0.0.1], pleased to meet you\r\n")
        elif msg.upper().startswith("MAIL FROM"):
            conn.sendall(b"250 2.1.0 Sender OK\r\n")
        elif msg.upper().startswith("RCPT TO"):
            conn.sendall(b"250 2.1.5 Recipient OK\r\n")
        elif msg.upper().startswith("DATA"):
            conn.sendall(b"354 Start mail input; end with <CRLF>.<CRLF>\r\n")
            # Collect data until "\r\n.\r\n"
            mail_data = b""
            while b"\r\n.\r\n" not in mail_data:
                chunk = conn.recv(2048)
                if not chunk:
                    break
                mail_data += chunk
            conn.sendall(b"250 2.0.0 Ok: queued as 4X8912KL\r\n")
        elif msg.upper().startswith("QUIT"):
            conn.sendall(b"221 2.0.0 mail.collegelabs.edu Service closing transmission channel\r\n")
            break

    conn.close()
    server.close()
    print("[-] SMTP Server session completed.")

def run_smtp_client():
    time.sleep(1) # wait for server to listen
    print("[*] Client connecting to SMTP Server...")
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((SERVER_HOST, SERVER_PORT))

    banner = client.recv(1024).decode()
    print(f"Server: {banner.strip()}")

    # Send HELO
    client.sendall(b"HELO student-laptop.local\r\n")
    resp = client.recv(1024).decode()
    print(f"Server: {resp.strip()}")

    # Send MAIL FROM
    client.sendall(b"MAIL FROM:<student@collegelabs.edu>\r\n")
    resp = client.recv(1024).decode()
    print(f"Server: {resp.strip()}")

    # Send RCPT TO (addressed to self as per syllabus)
    client.sendall(b"RCPT TO:<student@collegelabs.edu>\r\n")
    resp = client.recv(1024).decode()
    print(f"Server: {resp.strip()}")

    # Send DATA
    client.sendall(b"DATA\r\n")
    resp = client.recv(1024).decode()
    print(f"Server: {resp.strip()}")

    # Encapsulated IMF (Internet Message Format - RFC 5322) Message
    imf_message = (
        "From: \"Student User\" <student@collegelabs.edu>\r\n"
        "To: \"Student User\" <student@collegelabs.edu>\r\n"
        "Date: Wed, 07 Oct 2026 12:00:00 +0530\r\n"
        "Subject: KTU S5 Networks Lab Experiment 3 SMTP Test\r\n"
        "Message-ID: <20261007.KTU.PCCSL507@collegelabs.edu>\r\n"
        "MIME-Version: 1.0\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "\r\n"
        "Hello!\r\n"
        "This is an automated test email addressed to myself for Experiment 3.\r\n"
        "Demonstrating SMTP command sequence and IMF encapsulation in Wireshark.\r\n"
        "\r\n.\r\n"
    )
    client.sendall(imf_message.encode("utf-8"))
    resp = client.recv(1024).decode()
    print(f"Server: {resp.strip()}")

    # Send QUIT
    client.sendall(b"QUIT\r\n")
    resp = client.recv(1024).decode()
    print(f"Server: {resp.strip()}")

    client.close()
    print("[*] Client finished email dispatch.")

def main():
    print("=" * 65)
    print("   EXPERIMENT 3: LOCAL SMTP + IMF PACKET GENERATOR")
    print("=" * 65)
    print("[*] 1. Open Wireshark and capture on 'Adapter for loopback traffic'")
    print("[*] 2. Use display filter: 'tcp.port == 2525 || smtp'")
    print("[*] 3. Run this script to execute the complete SMTP dialog")
    print("=" * 65)
    t = threading.Thread(target=run_smtp_server)
    t.start()
    run_smtp_client()
    t.join()

if __name__ == "__main__":
    main()
