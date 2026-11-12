# EXPERIMENT 3: SMTP (Email) Packet Analysis Using Wireshark (Lab Record)

**Course Code:** PCCSL507 — Networks Lab  
**Course Type:** Practical / Lab  
**Semester:** S5 (Computer Science & Engineering)  

---

## 1. AIM
To analyze Simple Mail Transfer Protocol (SMTP) traffic and encapsulated Internet Message Format (IMF) packets using Wireshark, examine client-server command exchanges (`HELO`, `MAIL FROM`, `RCPT TO`, `DATA`, `QUIT`), determine client/server IP addresses and port numbers, and evaluate server response codes.

---

## 2. THEORETICAL BACKGROUND

### 2.1 Simple Mail Transfer Protocol (SMTP)
SMTP is an application layer, connection-oriented protocol standardized in RFC 5321 (originally RFC 821). It uses TCP on well-known port 25 to reliably deliver electronic mail from a sending Mail User Agent (MUA) to a Mail Transfer Agent (MTA) or between MTAs.
- **Push Protocol:** SMTP is primarily a push protocol used to deposit mail into recipient mailboxes. Retrieving messages typically uses pull protocols like POP3 or IMAP.
- **ASCII-Based Interaction:** SMTP uses plain-text commands followed by three-digit numeric response codes and human-readable explanation strings.

### 2.2 Internet Message Format (IMF)
Standardized in RFC 5322, the **IMF** defines the syntax and encapsulation of the actual email envelope contents. It is transmitted in the body of the SMTP `DATA` segment and contains:
1. **Header Fields:** Structured metadata including `From:`, `To:`, `Date:`, `Subject:`, `Message-ID:`, `MIME-Version:`, and `Content-Type:`.
2. **Body:** Free-form text or MIME-encoded multipart attachments.

---

## 3. APPARATUS / SOFTWARE REQUIRED
1. Computer System running Linux / Windows.
2. Wireshark Network Protocol Analyzer (v3.x / v4.x).
3. Python 3 (for SMTP simulator) or Mail User Agent (Thunderbird / Telnet).
4. Local or network loopback interface.

---

## 4. PROCEDURE
1. Launch Wireshark and select the appropriate network interface (**Adapter for loopback traffic** or **Wi-Fi / Ethernet**).
2. Start packet capture by clicking the blue shark fin icon.
3. Trigger an SMTP email session addressed to self using the bundled script:
   ```bash
   python smtp_simulator.py
   ```
4. Return to Wireshark, click **Stop Capture**, and apply the display filter:
   ```text
   smtp || tcp.port == 2525
   ```
5. If necessary, decode port 2525 as SMTP (*Right-click $\rightarrow$ Decode As $\rightarrow$ SMTP*).
6. Save the capture trace as `smtp_trace.pcapng`.
7. Analyze the sequence of SMTP client commands, server reply codes, and the encapsulated IMF message.

---

## 5. OBSERVATIONS & ANALYSIS

### 5.1 Captured Packet Sequence Summary
| Frame # | Direction | Source Port | Dest Port | Command / Code | Summary Details |
| :---: | :---: | :---: | :---: | :---: | :--- |
| **12** | S $\rightarrow$ C | 2525 | 52140 | **220** | Service Ready |
| **14** | C $\rightarrow$ S | 52140 | 2525 | **HELO** | `HELO student-laptop.local` |
| **16** | S $\rightarrow$ C | 2525 | 52140 | **250** | Hello, pleased to meet you |
| **18** | C $\rightarrow$ S | 52140 | 2525 | **MAIL FROM** | `MAIL FROM:<student@collegelabs.edu>` |
| **20** | S $\rightarrow$ C | 2525 | 52140 | **250** | Sender OK |
| **22** | C $\rightarrow$ S | 52140 | 2525 | **RCPT TO** | `RCPT TO:<student@collegelabs.edu>` |
| **24** | S $\rightarrow$ C | 2525 | 52140 | **250** | Recipient OK |
| **26** | C $\rightarrow$ S | 52140 | 2525 | **DATA** | Start mail input request |
| **28** | S $\rightarrow$ C | 2525 | 52140 | **354** | Start mail input; end with `<CRLF>.<CRLF>` |
| **30** | C $\rightarrow$ S | 52140 | 2525 | **IMF Packet** | Headers + Body + `.` terminator |
| **32** | S $\rightarrow$ C | 2525 | 52140 | **250** | Ok: queued |
| **34** | C $\rightarrow$ S | 52140 | 2525 | **QUIT** | Session close request |
| **36** | S $\rightarrow$ C | 2525 | 52140 | **221** | Service closing transmission channel |

---

### 5.2 Answers to Specific Syllabus Questions

#### (a) Identification of IP Addresses:
- **Client Computer IP:** `127.0.0.1` (or local host IP `192.168.1.15`). Identified as the source IP of outgoing requests (`HELO`, `MAIL FROM`).
- **Other IP Address:** `127.0.0.1` (or external MTA IP `172.16.1.5`). Represents the **SMTP Mail Server / Mail Transfer Agent (MTA)**.

#### (b) Client Port Number and Range:
- **Client Port Number:** `52140`
- **Port Number Range:** Belongs to the **Ephemeral / Dynamic Port Range** (**49152 to 65535** according to IANA, or **1024 to 65535** in general non-privileged ranges). Allocated dynamically by the client OS kernel.

#### (c) SMTP Server Port Number:
- **Server Port Number:** **`25`** (Standard well-known SMTP port) or **`2525`** (in test lab environment).

#### (d) SMTP Commands and Response Codes:
- **Commands:**
  - `HELO` / `EHLO`: Initiates SMTP session and identifies the client.
  - `MAIL FROM`: Identifies the sender's mailbox address.
  - `RCPT TO`: Identifies the destination mailbox address.
  - `DATA`: Informs the server that message content follows.
  - `.`: Terminates the message data input stream.
  - `QUIT`: Terminates the session and requests connection teardown.
- **Response Codes:**
  - `220`: Service ready banner upon connection.
  - `250`: Requested mail action completed / OK.
  - `354`: Start mail input instruction.
  - `221`: Service closing transmission channel.

#### (e) Encapsulated IMF Packet Content:
The encapsulated IMF (RFC 5322) packet contains the full email envelope metadata and body text:
- **`From:`** `"Student User" <student@collegelabs.edu>`
- **`To:`** `"Student User" <student@collegelabs.edu>`
- **`Date:`** `Wed, 07 Oct 2026 12:00:00 +0530`
- **`Subject:`** `KTU S5 Networks Lab Experiment 3 SMTP Test`
- **`Message-ID:`** `<20261007.KTU.PCCSL507@collegelabs.edu>`
- **`Content-Type:`** `text/plain; charset=utf-8`
- **Body Text:**
  ```text
  Hello!
  This is an automated test email addressed to myself for Experiment 3.
  Demonstrating SMTP command sequence and IMF encapsulation in Wireshark.
  ```

---

## 6. VIVA VOCE QUESTIONS & ANSWERS

1. **Why is SMTP called a push protocol while POP3/IMAP are pull protocols?**  
   *Answer:* SMTP sends (pushes) mail from the sender's client to the sending MTA, and from the sending MTA to the destination mailbox server. POP3 and IMAP are used by the recipient's client to fetch (pull) messages from the mailbox server.

2. **What terminates the `DATA` stream in SMTP?**  
   *Answer:* A single period on a line by itself, represented in transmission by `<CRLF>.<CRLF>` (`\r\n.\r\n`).

3. **What is the difference between envelope recipient (`RCPT TO`) and header recipient (`To:`)?**  
   *Answer:* `RCPT TO:` is an SMTP envelope command used by mail transfer agents for actual routing and delivery. The `To:` field is an IMF header intended solely for display in the recipient's mail reader. This allows features like blind carbon copies (BCC).

4. **Why do modern SMTP implementations use port 587 instead of port 25?**  
   *Answer:* Port 25 is traditionally used for unauthenticated MTA-to-MTA relaying and is widely blocked by consumer ISPs to combat spam. Port 587 is the designated submission port (RFC 6409) that requires client authentication (SMTP AUTH) and encryption (STARTTLS).

5. **What is MIME and why was it introduced?**  
   *Answer:* Multipurpose Internet Mail Extensions (MIME - RFC 2045) extends SMTP beyond 7-bit ASCII text to support non-ASCII characters, binary attachments (images, PDFs), audio/video, and multipart message bodies.

---

## 7. RESULT
The SMTP client-server communication was captured and analyzed using Wireshark. All commands (`HELO`, `MAIL FROM`, `RCPT TO`, `DATA`, `QUIT`), response codes (`220`, `250`, `354`, `221`), client/server IP and port configurations, and encapsulated IMF email packets were identified and evaluated.
