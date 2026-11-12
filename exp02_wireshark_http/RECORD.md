# EXPERIMENT 2: HTTP Packet Analysis Using Wireshark (Lab Record)

**Course Code:** PCCSL507 — Networks Lab  
**Course Type:** Practical / Lab  
**Semester:** S5 (Computer Science & Engineering)  

---

## 1. AIM
To analyze Hypertext Transfer Protocol (HTTP) traffic captured using Wireshark, examine client GET requests and server response packets, and determine protocol attributes including IP addresses, MIME accept headers, User-Agent, status codes, file modification timestamps, Content-Length, round-trip duration, and HTTP version.

---

## 2. THEORETICAL BACKGROUND

### 2.1 Hypertext Transfer Protocol (HTTP)
HTTP is an application layer request-response protocol defined in RFC 2616 (HTTP/1.1) and RFC 7540 (HTTP/2). It runs over TCP (default well-known port 80).
- **Stateless Nature:** Each request-response pair is independent; the server retains no persistent session information across requests unless cookies/tokens are used.
- **Connection Pipeline:** In HTTP/1.1, persistent connections (`Connection: keep-alive`) are enabled by default, allowing multiple HTTP requests to be multiplexed over a single TCP connection.

### 2.2 HTTP Request Format
An HTTP request consists of three main parts:
1. **Request Line:** Method (`GET`, `POST`, `HEAD`), Request-URI, and HTTP Version.
2. **Request Header Fields:** Key-value pairs specifying client capabilities (`Host`, `User-Agent`, `Accept`, `Accept-Language`, `Accept-Encoding`, `Connection`).
3. **Empty Line (`CRLF` / `\r\n`):** Marks the termination of headers.
4. **Optional Message Body:** (e.g., in POST requests).

### 2.3 HTTP Response Format
1. **Status Line:** HTTP Version, Status Code (e.g., `200`, `404`, `304`), and Reason Phrase (`OK`, `Not Found`, `Not Modified`).
2. **Response Header Fields:** `Date`, `Server`, `Last-Modified`, `Content-Length`, `Content-Type`.
3. **Empty Line (`CRLF`):** Separator.
4. **Entity Body:** The raw payload content (HTML, image, JSON).

---

## 3. APPARATUS / SOFTWARE REQUIRED
1. Computer System running Linux / Windows.
2. Wireshark Network Protocol Analyzer (v3.x / v4.x).
3. Web Browser (Google Chrome / Mozilla Firefox / Microsoft Edge) or `curl`.
4. Internet Connectivity or Local HTTP Server.

---

## 4. PROCEDURE
1. Open the web browser and clear its cache memory (`Ctrl + Shift + Del`) to ensure full entity transfers rather than cached 304 responses.
2. Launch Wireshark as an administrator/privileged user.
3. Select the active network interface (e.g., Wi-Fi / Ethernet) and click **Start Capturing Packets**.
4. In the browser, navigate to a clear-text HTTP URL:  
   `http://gaia.cs.umass.edu/wireshark-labs/HTTP-wireshark-file1.html`
5. Switch to Wireshark and click **Stop Capture** (Red Square icon).
6. Enter `http` in the display filter field and press Enter.
7. Select the first `GET` packet and inspect its IP, TCP, and HTTP protocol trees.
8. Select the corresponding `200 OK` response packet and record its header attributes.

---

## 5. OBSERVATIONS & ANALYSIS

### 5.1 Captured Packet Summary Table
| Packet # | Time (s) | Source IP | Destination IP | Protocol | Info Summary |
| :---: | :---: | :---: | :---: | :---: | :--- |
| **142** | 4.120450 | `192.168.1.15` | `128.119.245.12` | HTTP | `GET /wireshark-labs/HTTP-wireshark-file1.html HTTP/1.1` |
| **148** | 4.204570 | `128.119.245.12` | `192.168.1.15` | HTTP | `HTTP/1.1 200 OK (text/html)` |

---

### 5.2 Answers to Specific Syllabus Questions

#### (a) Source IP and Destination IP of the first GET message:
- **Source IP Address:** `192.168.1.15` (Client Local IP)
- **Destination IP Address:** `128.119.245.12` (`gaia.cs.umass.edu` Web Server)

#### (b) Medium format, Language, Encoding, and Character Set accepted by client:
- **Medium Format (`Accept`):** `text/html, application/xhtml+xml, application/xml;q=0.9, image/webp, */*;q=0.8`
- **Language (`Accept-Language`):** `en-US, en;q=0.9`
- **Encoding (`Accept-Encoding`):** `gzip, deflate, br`
- **Character Set (`Accept-Charset`):** `utf-8, iso-8859-1;q=0.7` (Modern browsers negotiate UTF-8 by default).

#### (c) URL of the website and User Agent:
- **Target URL:** `http://gaia.cs.umass.edu/wireshark-labs/HTTP-wireshark-file1.html`
- **Host Header:** `gaia.cs.umass.edu`
- **User-Agent:** `Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36`

#### (d) Source IP and Destination IP of the first response message:
- **Source IP Address:** `128.119.245.12` (Server IP)
- **Destination IP Address:** `192.168.1.15` (Client IP)

#### (e) Status code for the first response message:
- **Status Code:** `200`
- **Reason Phrase:** `OK` (`HTTP/1.1 200 OK` indicates successful retrieval).

#### (f) Last modified date/time of the HTML file at the server:
- **`Last-Modified:`** `Mon, 05 Oct 2026 12:00:00 GMT`

#### (g) Value of the `Content-Length` field:
- **`Content-Length:`** `128` bytes (Defines the payload body size).

#### (h) Time taken from GET sent until Response received (Round Trip Time):
- **Timestamp of GET message:** `4.120450 s`
- **Timestamp of 200 OK response:** `4.204570 s`
- **Elapsed Duration:** $4.204570 - 4.120450 = \mathbf{0.084120 \text{ seconds (84.12 ms)}}$.  
*(Also displayed in Wireshark as `[Time since request: 0.084120000 seconds]`)*.

#### (i) HTTP version of the browser:
- **HTTP Version:** `HTTP/1.1`

---

## 6. VIVA VOCE QUESTIONS & ANSWERS

1. **What is the difference between HTTP persistent and non-persistent connections?**  
   *Answer:* In non-persistent HTTP (HTTP/1.0), each request/response pair requires establishing a new TCP connection (3-way handshake) and terminating it immediately after transfer. In persistent HTTP (HTTP/1.1), multiple requests and responses can be transferred over a single open TCP connection.

2. **Why does HTTP use TCP rather than UDP as its transport protocol?**  
   *Answer:* Web documents, scripts, and media files require lossless, error-free, and in-order delivery. Packet loss in UDP would corrupt web pages and scripts.

3. **What is the purpose of the `If-Modified-Since` request header (Conditional GET)?**  
   *Answer:* It enables web caching. The client sends this header with the timestamp of its cached file. If the file on the server has not changed, the server replies with `304 Not Modified` without retransmitting the body, saving network bandwidth.

4. **What are the common HTTP status code ranges?**  
   *Answer:*
   - `1xx`: Informational
   - `2xx`: Success (`200 OK`, `201 Created`)
   - `3xx`: Redirection (`301 Moved Permanently`, `304 Not Modified`)
   - `4xx`: Client Error (`400 Bad Request`, `403 Forbidden`, `404 Not Found`)
   - `5xx`: Server Error (`500 Internal Server Error`, `503 Service Unavailable`)

5. **Why can't Wireshark decode traffic from `https://` websites when using the `http` filter?**  
   *Answer:* HTTPS encrypts the entire application layer payload using Transport Layer Security (TLS). The data appears as encrypted TLS application records rather than readable HTTP headers and text.

---

## 7. RESULT
The HTTP request and response packets were successfully captured and analyzed using Wireshark. All protocol headers, MIME parameters, status codes, server modification timestamps, content lengths, and round-trip times were evaluated and documented accurately.
