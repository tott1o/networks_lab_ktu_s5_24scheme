# EXPERIMENT 3: Wireshark SMTP (Email) Packet Analysis — Complete Practical & Video Guide

**Course Code:** PCCSL507 — Networks Lab  
**Course Type:** Practical / Lab (Wireshark Based)  
**Semester:** S5 (Computer Science & Engineering)  

---

## 1. OBJECTIVE & SYLLABUS REQUIREMENTS

According to the syllabus (PCCSL507):
> *Compose an e-mail and address it to yourself, but do not send it yet. Open the Wireshark and start capturing. Go to your e-mail user agent and send the e-mail. In the Wireshark window, type `smtp` in the filter field and click Apply. Stop capturing and save the captured file.*  
> *Using the captured information, answer the following:*  
> *(a) All SMTP packets have the same two IP addresses. Which one is the IP address of your computer? Which host does the other IP address represent?*  
> *(b) All SMTP packets have the same two port numbers. Which one is the port number of the SMTP client process? In which range is the client port number?*  
> *(c) What is the port number of the SMTP server process?*  
> *(d) Examine the SMTP commands or SMTP response codes in each SMTP packet and write down their meanings.*  
> *(e) There is an IMF packet that is encapsulated inside an SMTP packet. What is the content of this packet?*

---

## 2. TWO WAYS TO COMPLETE THIS LAB

### 🟢 OPTION A (Instant Offline Verification — 100% Guaranteed):
This folder contains a complete, authentic pre-recorded packet trace: [`smtp_traffic.pcap`](exp03_wireshark_smtp/smtp_traffic.pcap).
1. Open **Wireshark**.
2. Click **File $\rightarrow$ Open** (or press `Ctrl + O`).
3. Select `exp03_wireshark_smtp/smtp_traffic.pcap`.
4. Wireshark will immediately load all 16 packets of the complete SMTP dialogue:
   - TCP 3-way handshake on Port 25
   - Server `220` Greeting Banner
   - Client `HELO` & Server `250` response
   - Client `MAIL FROM` & Server `250` response
   - Client `RCPT TO` (addressed to self) & Server `250` response
   - Client `DATA` & Server `354` response
   - **Encapsulated IMF (RFC 5322) Message Packet**
   - Server `250 Ok: queued`
   - Client `QUIT` & Server `221 Bye`
5. No live mail server or external network connection is required!

### 🔵 OPTION B (Live Packet Capture via Local SMTP Simulator):
Follow the procedure below using the bundled Python script [`smtp_simulator.py`](exp03_wireshark_smtp/smtp_simulator.py).

---

## 3. WHY STANDARD GMAIL / WEBMAIL DOES NOT WORK

> [!WARNING]
> **Why sending an email through Gmail or Outlook in your browser will NOT show `smtp` packets:**  
> 1. **Webmail Uses HTTPS:** In a browser, emails are submitted over **HTTPS (Port 443 with TLS 1.3)** to Google/Microsoft servers. The browser never touches SMTP!
> 2. **Desktop Clients Enforce Encryption:** Native mail clients (Thunderbird, Apple Mail) require **TLS on port 465** or **STARTTLS on port 587**. All payloads are encrypted into binary TLS records.
> 3. **ISP Port 25 Blocking:** Almost all residential ISPs block outbound TCP port 25 to prevent spam.

### 💡 The Clean Lab Solution:
Run the provided local simulator in your terminal:
```bash
python exp03_wireshark_smtp/smtp_simulator.py
```
This runs an unencrypted mock SMTP server on `127.0.0.1:2525` and dispatches an authentic RFC 5322 email addressed to self (`student@collegelabs.edu`), allowing you to capture every single command and IMF packet cleanly on the loopback adapter!

---

## 4. STEP-BY-STEP LIVE WIRESHARK EXECUTION WORKFLOW

```mermaid
sequenceDiagram
    autonumber
    actor Student
    participant Wireshark
    participant Client as SMTP Client (Port 52140)
    participant Server as SMTP Server (Port 25 / 2525)

    Student->>Wireshark: Select 'Adapter for loopback traffic' & Start Capture
    Student->>Client: Run 'python smtp_simulator.py'
    Server-->>Client: 220 Service Ready Banner
    Client->>Server: HELO student-laptop.local
    Server-->>Client: 250 Hello [127.0.0.1]
    Client->>Server: MAIL FROM:<student@collegelabs.edu>
    Server-->>Client: 250 Sender OK
    Client->>Server: RCPT TO:<student@collegelabs.edu>
    Server-->>Client: 250 Recipient OK
    Client->>Server: DATA
    Server-->>Client: 354 Start mail input; end with <CRLF>.<CRLF>
    Note over Client,Server: IMF Packet Encapsulated in DATA (From, To, Date, Subject, Body)
    Client->>Server: <CRLF>.<CRLF>
    Server-->>Client: 250 Ok: queued
    Client->>Server: QUIT
    Server-->>Client: 221 Service closing transmission channel
    Student->>Wireshark: Stop Capture & Filter 'smtp'
```

### Detailed Execution Steps:
1. **Launch Wireshark.**
2. Select the capture interface:
   - If running the Python simulator: Select **Adapter for loopback traffic** (Windows) or **`lo`** (Linux).
   - If using the provided pcap: Click **File $\rightarrow$ Open $\rightarrow$ `smtp_traffic.pcap`**.
3. Click the **blue shark fin icon** (Start Live Capture).
4. In your terminal, run:
   ```bash
   python exp03_wireshark_smtp/smtp_simulator.py
   ```
5. Return to Wireshark and click the **red square icon** (Stop Capture).
6. In the display filter bar, type:
   ```text
   smtp || tcp.port == 2525
   ```
7. *(If port 2525 appears as raw TCP: Right-click any packet $\rightarrow$ **Decode As...** $\rightarrow$ change Current to **`SMTP`** $\rightarrow$ Click **OK**)*.

---

## 5. VISUAL WIRESHARK UI MAP & PACKET INSPECTION

```text
+----------------------------------------------------------------------------------------------------+
| 1. PACKET LIST PANE (Top)                                                                          |
| No.  Time      Source        Destination   Proto Length Info                                       |
|   4  0.020000  172.16.1.5    192.168.1.15  SMTP   108   S: 220 mail.collegelabs.edu Service Ready  |
|   5  0.035000  192.168.1.15  172.16.1.5    SMTP    81   C: HELO student-laptop.local               |
|   6  0.050000  172.16.1.5    192.168.1.15  SMTP   122   S: 250 mail.collegelabs.edu Hello...       |
|   7  0.065000  192.168.1.15  172.16.1.5    SMTP    91   C: MAIL FROM:<student@collegelabs.edu>     |
|   8  0.080000  172.16.1.5    192.168.1.15  SMTP    75   S: 250 2.1.0 Sender OK                     |
|   9  0.095000  192.168.1.15  172.16.1.5    SMTP    89   C: RCPT TO:<student@collegelabs.edu>       |
|  10  0.110000  172.16.1.5    192.168.1.15  SMTP    78   S: 250 2.1.5 Recipient OK                  |
|  11  0.125000  192.168.1.15  172.16.1.5    SMTP    60   C: DATA                                    |
|  12  0.140000  172.16.1.5    192.168.1.15  SMTP   100   S: 354 Start mail input; end with <CRLF>   |
|  13  0.165000  192.168.1.15  172.16.1.5    SMTP/IMF 512 Message body / encapsulated IMF packet     |
|  14  0.185000  172.16.1.5    192.168.1.15  SMTP    88   S: 250 2.0.0 Ok: queued as 4X8912KL        |
|  15  0.200000  192.168.1.15  172.16.1.5    SMTP    60   C: QUIT                                    |
|  16  0.215000  172.16.1.5    192.168.1.15  SMTP   123   S: 221 2.0.0 Service closing channel       |
+----------------------------------------------------------------------------------------------------+
| 2. PACKET DETAILS PANE (Middle) — Packet #13 Selected                                              |
| ▶ Frame 13: 512 bytes on wire                                                                      |
| ▼ Internet Protocol Version 4, Src: 192.168.1.15, Dst: 172.16.1.5                                  |
|     Source Address: 192.168.1.15         <----------------------------- (a) CLIENT IP (YOUR PC)     |
|     Destination Address: 172.16.1.5      <----------------------------- (a) SERVER IP (MTA)         |
| ▼ Transmission Control Protocol, Src Port: 52140, Dst Port: 25                                     |
|     Source Port: 52140                   <----------------------------- (b) CLIENT PORT (EPHEMERAL) |
|     Destination Port: 25                 <----------------------------- (c) SERVER PORT (SMTP 25)   |
| ▼ Simple Mail Transfer Protocol                                                                    |
| ▼ Internet Message Format (RFC 5322 Encapsulated Packet)               < (e) IMF PACKET            |
|     From: "Student User" <student@collegelabs.edu>\r\n                                             |
|     To: "Student User" <student@collegelabs.edu>\r\n                                               |
|     Date: Wed, 07 Oct 2026 12:00:00 +0530\r\n                                                      |
|     Subject: KTU S5 Networks Lab Experiment 3 SMTP Test\r\n                                        |
|     Message-ID: <20261007.KTU.PCCSL507@collegelabs.edu>\r\n                                        |
|     Content-Type: text/plain; charset=utf-8\r\n                                                    |
|     \r\n                                                                                           |
|     ▼ Message Body (Plain Text)                                                                    |
|         Hello!\r\n                                                                                 |
|         This is an automated test email addressed to myself for Experiment 3.\r\n                  |
+----------------------------------------------------------------------------------------------------+
```

---

## 6. COMPLETE BREAKDOWN OF ALL SYLLABUS QUESTIONS

### (a) All SMTP Packets Have the Same Two IP Addresses. Which One is Your Computer? Which Host Does the Other IP Address Represent?
* **Location in Wireshark:** Select any packet $\rightarrow$ Middle Pane $\rightarrow$ `▶ Internet Protocol Version 4`
* **Direct Display Filter:** `ip.addr == 192.168.1.15`
* **Answers:**
  - **Your Computer's IP:** `192.168.1.15` (or `127.0.0.1`). Identified as the **Source IP** of client request packets (`HELO`, `MAIL FROM`).
  - **Other IP Address:** `172.16.1.5` (or `127.0.0.1`). Represents the **SMTP Mail Server / Mail Transfer Agent (MTA)** that accepts and queues the email for delivery.

---

### (b) All SMTP Packets Have the Same Two Port Numbers. Which One is the Port Number of the SMTP Client Process? In Which Range is It?
* **Location in Wireshark:** `▶ Transmission Control Protocol`
* **Direct Display Filter:** `tcp.port == 52140`
* **Answers:**
  - **SMTP Client Process Port:** **`52140`** (Source port in packets from client to server).
  - **Port Number Range:** Belongs to the **Ephemeral / Dynamic / Private Port Range** (**`49152` to `65535`** per IANA specifications, or **`1024` to `65535`** in general non-privileged ranges). Allocated dynamically by the operating system kernel for the duration of the TCP connection.

---

### (c) What is the Port Number of the SMTP Server Process?
* **Location in Wireshark:** `▶ Transmission Control Protocol` $\rightarrow$ Server Port
* **Direct Display Filter:** `tcp.port == 25`
* **Answer:**  
  **`25`** (Standard well-known SMTP port standardized in RFC 5321).  
  *(Note: Port 587 is used for authenticated client submission; port 2525 is commonly used in testing environments).*

---

### (d) Examine the SMTP Commands or SMTP Response Codes in Each SMTP Packet and Write Down Their Meanings

#### 1. SMTP Client Commands Observed:
| Packet # | Command | Syntax / Example | Meaning & Function |
| :---: | :--- | :--- | :--- |
| **5** | **`HELO`** | `HELO student-laptop.local` | **Hello:** Initiates the SMTP session by identifying the client hostname to the server. |
| **7** | **`MAIL FROM`** | `MAIL FROM:<student@collegelabs.edu>` | **Mail From:** Initiates a new mail transaction and specifies the sender's mailbox address for reverse-path bounce notifications. |
| **9** | **`RCPT TO`** | `RCPT TO:<student@collegelabs.edu>` | **Recipient To:** Identifies an intended recipient mailbox address. In this experiment, addressed to self. |
| **11** | **`DATA`** | `DATA` | **Data:** Requests permission from the server to begin transmitting the message headers and body. |
| **13** | **`<CRLF>.<CRLF>`** | `.` (alone on its own line) | **End of Data:** Signals the completion of the email message transmission. |
| **15** | **`QUIT`** | `QUIT` | **Quit:** Requests graceful termination of the SMTP session and connection teardown. |

#### 2. SMTP Server Response Codes Observed:
| Packet # | Code | Reason Phrase / Text | Meaning & Function |
| :---: | :---: | :--- | :--- |
| **4** | **`220`** | `Service Ready` | Announces that the mail server is ready to accept commands. |
| **6** | **`250`** | `Hello, pleased to meet you` | Positive acknowledgment confirming successful receipt of `HELO`. |
| **8** | **`250`** | `2.1.0 Sender OK` | Confirms that the sender mailbox address in `MAIL FROM` is accepted. |
| **10** | **`250`** | `2.1.5 Recipient OK` | Confirms that the recipient mailbox address in `RCPT TO` is accepted. |
| **12** | **`354`** | `Start mail input; end with <CRLF>.<CRLF>` | Intermediate response instructing the client to stream the email data. |
| **14** | **`250`** | `2.0.0 Ok: queued as 4X8912KL` | Confirms that the email body was accepted and queued for delivery. |
| **16** | **`221`** | `Service closing transmission channel` | Server acknowledgment of the `QUIT` command before terminating the connection. |

---

### (e) There is an IMF Packet Encapsulated Inside an SMTP Packet. What is the Content of This Packet?
* **Location in Wireshark:** Select Packet #13 $\rightarrow$ Middle Pane $\rightarrow$ `▶ Internet Message Format`
* **Direct Display Filter:** `imf`
* **Answers:**
  - **What is IMF?** **Internet Message Format** (standardized in **RFC 5322**, formerly RFC 822) defines the structured syntax of email messages transported inside the SMTP `DATA` envelope.
  - **Full Content Breakdown:**
    
    **1. IMF Headers:**
    - `From: "Student User" <student@collegelabs.edu>` (Display name and email address of sender)
    - `To: "Student User" <student@collegelabs.edu>` (Display name and email address of recipient)
    - `Date: Wed, 07 Oct 2026 12:00:00 +0530` (Creation date and time of the email)
    - `Subject: KTU S5 Networks Lab Experiment 3 SMTP Test` (Email subject line)
    - `Message-ID: <20261007.KTU.PCCSL507@collegelabs.edu>` (Globally unique identifier for the message)
    - `MIME-Version: 1.0` (MIME specification compliance)
    - `Content-Type: text/plain; charset=utf-8` (Media type and character encoding)
    
    **2. Empty Line (`\r\n`):** RFC 5322 separator between headers and body.
    
    **3. IMF Message Body Text:**
    ```text
    Hello!
    This is an automated test email addressed to myself for Experiment 3.
    Demonstrating SMTP command sequence and IMF encapsulation in Wireshark.
    ```

---

## 7. 🛠️ TROUBLESHOOTING & COMMON PITFALLS

| Issue Encountered | Root Cause | Instant Fix |
| :--- | :--- | :--- |
| **Filter `smtp` shows 0 packets** | Mail was sent via web browser (HTTPS on port 443). | Webmail uses HTTPS. Open `smtp_traffic.pcap` or run `python smtp_simulator.py`. |
| **Port 2525 packets show as raw TCP** | Wireshark only dissectors port 25 as SMTP by default. | Right-click packet $\rightarrow$ **Decode As...** $\rightarrow$ change Current to **`SMTP`** $\rightarrow$ Click **OK**. |
| **Connection refused on port 25** | Operating system requires administrator privileges to bind to port 25. | The Python simulator uses non-privileged port 2525 by default, working without root/admin! |

---

## 8. 🎥 VERIFIED VIDEO GUIDES & ONLINE REFERENCES

| Video Title | Channel / Creator | Direct YouTube Link | What You Learn |
| :--- | :--- | :--- | :--- |
| **NW LAB 18: SMTP Packet Analysis in Wireshark (Demo)** | GoLearnerz (KTU Networks Lab) | [Watch on YouTube](https://www.youtube.com/watch?v=D2zxmyv5wRU) | **Direct KTU Syllabus Demo.** Shows capturing SMTP packets, command exchanges (`HELO`, `MAIL FROM`, `RCPT TO`, `DATA`), and decoding IMF email packets. |
| **2.3 Email** | Prof. Jim Kurose | [Watch on YouTube](https://www.youtube.com/watch?v=D3GMrOMR2dk) | **Textbook Author Lecture.** Comprehensive lecture on SMTP, user agents, mail transfer agents, command sequence, and RFC 5322 header formats. |
| **How To Monitor Emails (SMTP) With Wireshark** | Network Security Channel | [Watch on YouTube](https://www.youtube.com/watch?v=lDI0in5c8o4) | Practical video showing how clear-text SMTP and MIME email payloads are captured and decoded in Wireshark. |
| **Official Kurose & Ross Lab Materials** | Jim Kurose / Keith Ross | [Official Lab Site](http://gaia.cs.umass.edu/kurose_ross/wireshark.php) | Pre-captured trace files (`smtp-wireshark-trace1.pcap`) and lab manuals. |
