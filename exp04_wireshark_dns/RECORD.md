# EXPERIMENT 4: DNS Packet Analysis Using Wireshark (Lab Record)

**Course Code:** PCCSL507 — Networks Lab  
**Course Type:** Practical / Lab  
**Semester:** S5 (Computer Science & Engineering)  

---

## 1. AIM
To analyze Domain Name System (DNS) protocol traffic captured using Wireshark, examine DNS query and response messages, inspect message headers, transaction IDs, port numbers, bit-level flag fields (QR, AA, TC, RD, RA, RCODE), and record counts (Questions, Answers, Authority, Additional records).

---

## 2. THEORETICAL BACKGROUND

### 2.1 The Domain Name System (DNS)
The Domain Name System (RFC 1034 / RFC 1035) is a hierarchical, distributed database that translates human-friendly hostnames (such as `ktu.edu.in`) into machine-readable IP addresses (such as `14.139.185.155`).
- **Transport Layer:** DNS predominantly runs over **UDP on port 53** to achieve low latency. TCP port 53 is used when response data exceeds 512 bytes (or 4096 bytes with EDNS0) or for DNS zone transfers.
- **Hierarchical Structure:** Consists of Root Name Servers, Top-Level Domain (TLD) Servers (`.in`, `.edu`, `.com`), Authoritative Name Servers, and Local Recursive Resolvers.

### 2.2 DNS Message Format
Every DNS message has a fixed 12-byte header:
1. **Identification (16 bits):** An identifier assigned by the client program to match queries with asynchronous replies.
2. **Flags (16 bits):**
   - `QR` (1 bit): `0` = Query, `1` = Response.
   - `Opcode` (4 bits): Type of query (`0` = Standard Query).
   - `AA` (1 bit): Authoritative Answer flag (Response only).
   - `TC` (1 bit): Truncation flag.
   - `RD` (1 bit): Recursion Desired.
   - `RA` (1 bit): Recursion Available flag (Response only).
   - `Z` (1 bit): Reserved.
   - `AD` (1 bit): Authentic Data (DNSSEC).
   - `CD` (1 bit): Checking Disabled (DNSSEC).
   - `RCODE` (4 bits): Response Code (`0` = NoError, `3` = NXDomain).
3. **Record Counters (4 x 16 bits):**
   - `QDCOUNT`: Number of question entries.
   - `ANCOUNT`: Number of answer resource records (RRs).
   - `NSCOUNT`: Number of authority name server records.
   - `ARCOUNT`: Number of additional resource records.

---

## 3. APPARATUS / SOFTWARE REQUIRED
1. Computer System running Linux / Windows.
2. Wireshark Network Protocol Analyzer (v3.x / v4.x).
3. Command Line Utilities (`ipconfig`, `systemd-resolve`, `nslookup`) or Web Browser.
4. Active Internet Connection.

---

## 4. PROCEDURE
1. Flush the operating system DNS resolver cache:
   - On Windows: Run `ipconfig /flushdns` in command prompt.
   - On Linux: Run `sudo resolvectl flush-caches` or `sudo systemd-resolve --flush-caches`.
2. Clear the browser host cache (`chrome://net-internals/#dns` $\rightarrow$ Clear host cache).
3. Open Wireshark and begin capturing on the active network interface.
4. Trigger a DNS query for the college domain (`ktu.edu.in`):
   - By visiting `http://ktu.edu.in` in the browser, or
   - By executing `nslookup ktu.edu.in` in terminal.
5. Stop Wireshark capture and apply the display filter:
   ```text
   dns
   ```
6. Locate the DNS query and matching response packet using the Transaction ID.
7. Record all header fields, flags, and record counts.

---

## 5. OBSERVATIONS & ANALYSIS

### 5.1 Captured Packet Summary Table
| Frame # | Time (s) | Source IP | Destination IP | Protocol | Length | Info Summary |
| :---: | :---: | :---: | :---: | :---: | :---: | :--- |
| **84** | 2.145020 | `192.168.1.15` | `8.8.8.8` | DNS | 74 bytes | `Standard query 0x4a8f A ktu.edu.in` |
| **88** | 2.189340 | `8.8.8.8` | `192.168.1.15` | DNS | 90 bytes | `Standard query response 0x4a8f A ktu.edu.in A 14.139.185.155` |

---

### 5.2 Answers to Specific Syllabus Questions

#### (a) Packet number of the first DNS query message resolving the college website:
- **Packet Number:** **Frame #84** *(Wireshark trace listing index)*.

#### (b) Transport layer protocol of the query message:
- **Protocol:** **`UDP` (User Datagram Protocol)**.

#### (c) Corresponding DNS response packet number and transport protocol:
- **Response Packet Number:** **Frame #88** (Correlated by Transaction ID `0x4a8f`).
- **Protocol:** **`UDP` (User Datagram Protocol)**.

#### (d) Source and destination port numbers for the DNS query message:
- **Source Port:** `54219` (Ephemeral client port).
- **Destination Port:** **`53`** (Well-known DNS server port).

#### (e) Source and destination port numbers for the DNS response message:
- **Source Port:** **`53`** (DNS server port).
- **Destination Port:** `54219` (Client port).

#### (f) Destination IP address to which the DNS query message was sent:
- **Destination IP Address:** `8.8.8.8` (Configured Recursive DNS Resolver).

#### (g) Query message ID, Response message ID, and purpose of this field:
- **Query Message ID:** `0x4A8F` (19087 decimal)
- **Response Message ID:** `0x4A8F` (19087 decimal)
- **Purpose:** Because UDP is a connectionless protocol, the client can transmit multiple queries simultaneously. The 16-bit Transaction ID enables the client OS resolver to match each asynchronous response to its corresponding query.

#### (h) Length of the Flag field in a DNS message:
- **Length:** **`16 bits` (2 bytes)**.

#### (i) Bit in the Flag field determining query vs response:
- **Determining Bit:** **Bit 0** (The Most Significant Bit, designated as the **QR bit**):
  - `QR = 0`: Message is a **Query**.
  - `QR = 1`: Message is a **Response**.

#### (j) Bits used ONLY in the response message and their functions:
1. **Bit 5 — Authoritative Answer (AA):** Set to `1` if the responding server is an authoritative server for the domain, or `0` if served from cache.
2. **Bit 8 — Recursion Available (RA):** Set to `1` by the server to inform the client that recursive querying is supported.
3. **Bits 12–15 — Response Code (RCODE, 4 bits):** Reports query status:
   - `0000` (`0`): `NoError` (Successful resolution).
   - `0011` (`3`): `NXDomain` (Non-Existent Domain).
   - `0010` (`2`): `ServFail` (Server failure).
   - `0101` (`5`): `Refused` (Query refused).

#### (k) Records present in the Query message:
- **Question Records:** **`1`** (`ktu.edu.in: type A, class IN`)
- **Answer Records:** **`0`**
- **Authority Records:** **`0`**
- **Additional Records:** **`0`** (or `1` if EDNS0 pseudo-RR is used)

#### (l) Records present in the Response message:
- **Question Records:** **`1`** (`ktu.edu.in: type A, class IN`)
- **Answer Records:** **`1`** (`14.139.185.155`, TTL = 300s)
- **Authority Records:** **`0`**
- **Additional Records:** **`0`**

---

## 6. VIVA VOCE QUESTIONS & ANSWERS

1. **Why does DNS use UDP port 53 rather than TCP for standard queries?**  
   *Answer:* Speed and efficiency. A UDP DNS transaction requires only a single request-response exchange (1 RTT). TCP requires a 3-way handshake (1 RTT), query/response (1 RTT), and 4-way termination, introducing unnecessary latency and server connection overhead.

2. **Under what circumstances does DNS switch to TCP?**  
   *Answer:* When response data exceeds 512 bytes (indicated by the `TC` Truncation bit set to 1), for DNSSEC key exchanges exceeding UDP buffer limits, and for DNS Zone Transfers (`AXFR` / `IXFR`) between primary and secondary name servers.

3. **What is the difference between Recursive and Iterative DNS queries?**  
   *Answer:* In a **recursive query**, the client asks the local resolver to find the full answer, and the resolver takes responsibility for querying other servers until the answer is found. In an **iterative query**, the contacted server responds with the best referral it has (e.g. pointing to the TLD or authoritative server), requiring the querier to make the subsequent requests itself.

4. **What does the `TTL` (Time-To-Live) field in an Answer Record mean?**  
   *Answer:* The TTL specifies the number of seconds that the client or intermediate caching resolver is allowed to store and reuse the cached record before issuing a fresh query to authoritative servers.

5. **What is an `A` record vs a `CNAME` record?**  
   *Answer:* An `A` (Address) record maps a hostname directly to a 32-bit IPv4 address. A `CNAME` (Canonical Name) record maps an alias hostname to another canonical hostname (e.g., `www.ktu.edu.in` $\rightarrow$ `ktu.edu.in`).

---

## 7. RESULT
The Domain Name System (DNS) query and response transactions were captured and analyzed using Wireshark. The 12-byte header structure, 16-bit flags (QR, AA, RA, RCODE), UDP port mappings, Transaction IDs, and record section counts were systematically examined and validated.
