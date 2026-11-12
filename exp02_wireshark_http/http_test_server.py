#!/usr/bin/env python3
"""
Experiment 2 Helper: Local HTTP Server for Clean Wireshark Capture
Course: PCCSL507 Networks Lab
Description: Runs an unencrypted HTTP/1.1 server on port 8000 so students
             can capture 100% pure HTTP GET and 200 OK packets on the
             loopback/Ethernet interface without HTTPS redirection or TLS encryption.
"""

from http.server import HTTPServer, SimpleHTTPRequestHandler
import sys

PORT = 8000

class LabHTTPRequestHandler(SimpleHTTPRequestHandler):
    def end_headers(self):
        # Add custom headers to demonstrate HTTP response header fields
        self.send_header("Last-Modified", "Wed, 07 Oct 2026 12:00:00 GMT")
        self.send_header("Server", "KTU-Networks-Lab-HTTP-Server/1.1")
        super().end_headers()

def run():
    server_address = ("127.0.0.1", PORT)
    httpd = HTTPServer(server_address, LabHTTPRequestHandler)
    print("=" * 60)
    print(f"[*] Local HTTP Lab Server running at http://127.0.0.1:{PORT}/")
    print("[*] Open Wireshark, listen on 'Loopback (Adapter for loopback traffic)'")
    print(f"[*] In your browser, open: http://127.0.0.1:{PORT}/")
    print("[*] Or run: curl -v http://127.0.0.1:8000/")
    print("[*] Press Ctrl+C to stop the server.")
    print("=" * 60)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n[*] Stopping HTTP server.")
        httpd.server_close()

if __name__ == "__main__":
    run()
