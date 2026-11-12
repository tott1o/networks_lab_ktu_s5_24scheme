# KTU S5 NETWORKS LAB (PCCSL507) — COMPLETE LAB MANUAL & CODEBASE

This repository contains the complete implementation, executable C source code (POSIX socket programming), Wireshark packet capture guides, Cisco Packet Tracer configuration guides, and publication-ready **Lab Records** with detailed theoretical background, network topologies, algorithms, compilation/execution steps, sample outputs, video guides, and viva-voce Q&A for **all experiments (Experiments 2 to 17)** of the **APJ Abdul Kalam Technological University (KTU)** B.Tech Computer Science and Engineering **Networks Lab (PCCSL507)** syllabus.

---

## 📁 Repository Structure

```
networklab/
├── exp02_wireshark_http/                  # Expt 2: HTTP Packet Analysis in Wireshark
│   ├── WIRESHARK_GUIDE.md                 # Step-by-Step Practical & YouTube Video Guide
│   ├── RECORD.md                          # Complete Lab Record (Theory, Questions a-i, Viva)
│   ├── http_traffic.pcap                  # Pre-recorded authentic HTTP trace (7 packets)
│   └── http_test_server.py                # Local Python HTTP server helper
│
├── exp03_wireshark_smtp/                  # Expt 3: SMTP (Email) Packet Analysis in Wireshark
│   ├── WIRESHARK_GUIDE.md                 # Step-by-Step Practical & YouTube Video Guide
│   ├── RECORD.md                          # Complete Lab Record (Theory, Questions a-e, Viva)
│   ├── smtp_traffic.pcap                  # Pre-recorded authentic SMTP/IMF trace (16 packets)
│   └── smtp_simulator.py                  # Local unencrypted SMTP & IMF email generator
│
├── exp04_wireshark_dns/                   # Expt 4: DNS Packet Analysis in Wireshark
│   ├── WIRESHARK_GUIDE.md                 # Step-by-Step Practical & YouTube Video Guide
│   ├── RECORD.md                          # Complete Lab Record (Theory, Questions a-l, Viva)
│   ├── dns_traffic.pcap                   # Pre-recorded authentic DNS trace (Query & Response)
│   └── dns_query_tool.py                  # Standalone DNS query tool
│
├── exp05_tcp_matrix_classification/       # Expt 5: TCP Matrix Type Identification
│   ├── server.c / client.c                # C Sockets (POSIX)
│   └── RECORD.md                          # Complete Lab Record
│
├── exp06_udp_slang_translator/            # Expt 6: UDP New-Gen Slang Translator
│   ├── server.c / client.c                # C Sockets (POSIX)
│   └── RECORD.md                          # Complete Lab Record
│
├── exp07_tcp_multiuser_chat/              # Expt 7: TCP Multi-Client Chat Server (fork-based)
│   ├── server.c / client.c                # C Sockets (POSIX fork() concurrency)
│   └── RECORD.md                          # Complete Lab Record
│
├── exp08_udp_concurrent_time_server/      # Expt 8: Concurrent UDP Remote Time Server (fork-based)
│   ├── server.c / client.c                # C Sockets (POSIX fork() concurrency)
│   └── RECORD.md                          # Complete Lab Record
│
├── exp09_tcp_concurrent_file_server/      # Expt 9: Concurrent TCP File Server (fork-based)
│   ├── server.c / client.c                # Multiprocess fork() & PID Reporter C Code
│   ├── sample.txt                         # Test file hosted on server
│   └── RECORD.md                          # Complete Lab Record
│
├── exp10_raw_socket_packet_sniffer/       # Expt 10: Link-Layer Packet Sniffer via Raw Sockets
│   ├── sniffer.c                          # Raw Socket (AF_PACKET) Header Dissector in C
│   └── RECORD.md                          # Complete Lab Record
│
├── exp11_router_commands_familiarization/ # Expt 11: Familiarizing Router Commands
│   ├── PACKET_TRACER_GUIDE.md             # Sub-questions (a to l), UI Setup, Show Commands
│   ├── RECORD.md                          # Complete Lab Record (Modes, DTE/DCE, Clock, Viva)
│   └── commands_cheatsheet.txt            # Ready-to-use Router CLI Cheat Sheet
│
├── exp12_static_routing/                  # Expt 12: Static Routing Implementation
│   ├── PACKET_TRACER_GUIDE.md             # Multi-Router Topology (Figure 1), Ping Tests
│   ├── RECORD.md                          # Complete Lab Record (Theory, AD, Tracert, Viva)
│   └── router_configs/                    # CLI Configs for R_2621, R_2501A, R_2501B, R_2501C (Cisco 1941 / 2911)
│
├── exp13_ripv2_routing/                   # Expt 13: RIPv2 Dynamic Routing Protocol
│   ├── PACKET_TRACER_GUIDE.md             # RIPv2 Setup, Multicast 224.0.0.9, Convergence
│   ├── RECORD.md                          # Complete Lab Record (Bellman-Ford, Timers, Viva)
│   └── router_configs/                    # Copy-paste CLI Configs
│
├── exp14_ospf_routing/                    # Expt 14: Single-Area OSPF Dynamic Routing
│   ├── PACKET_TRACER_GUIDE.md             # OSPF Area 0, Wildcard Masks, SPF Metric Cost
│   ├── RECORD.md                          # Complete Lab Record (Dijkstra, Adjacency, Viva)
│   └── router_configs/                    # Copy-paste CLI Configs
│
├── exp15_standard_acl_campus/             # Expt 15: Standard ACL Host Filtering
│   ├── PACKET_TRACER_GUIDE.md             # Campus Network (Figure 2), Allow Host_B, Deny Labs
│   ├── RECORD.md                          # Complete Lab Record (Source Filtering, Placement)
│   └── router_configs/                    # Copy-paste CLI Configs
│
├── exp16_extended_acl_hackathon/          # Expt 16: Extended ACL Selective Port 80 Filtering
│   ├── PACKET_TRACER_GUIDE.md             # Subnetting 140.80.0.0/21, Block HTTP, Allow Ping/FTP
│   ├── RECORD.md                          # Complete Lab Record (VLSM Design, Port Filtering)
│   └── router_configs/                    # Copy-paste CLI Configs
│
├── exp17_ipv6_ripng_routing/              # Expt 17: IPv6 Routing using RIPng
│   ├── PACKET_TRACER_GUIDE.md             # IPv6 Topology (Figure 3), UDP 521, Multicast FF02::9
│   ├── RECORD.md                          # Complete Lab Record (128-bit Addressing, Link-Local)
│   └── router_configs/                    # Copy-paste CLI Configs
│
├── ntworklab.pdf                          # Official KTU PCCSL507 syllabus document
└── README.md                              # Master index and evaluation guidelines
```

---

## 📋 Complete Syllabus Experiments Summary

### Part A: Wireshark Based Experiments (Experiments 2 to 4)

| Expt No. | Protocol | Focus & Key Analysis | Guide & Record Links |
| :---: | :---: | :--- | :--- |
| **2** | **HTTP** | Clear browser cache, capture HTTP GET request and 200 OK response. Determine IP addresses, Accept headers (medium, language, encoding, charset), User-Agent, status codes, `Last-Modified`, `Content-Length`, RTT duration, and HTTP version. | [Wireshark Guide](exp02_wireshark_http/WIRESHARK_GUIDE.md) \| [Lab Record](exp02_wireshark_http/RECORD.md) |
| **3** | **SMTP** | Compose and send email to self, capture on port 25/587. Identify client and server IP addresses, ephemeral client port and server port 25, analyze SMTP commands (`HELO`, `MAIL FROM`, `RCPT TO`, `DATA`, `QUIT`) and response codes (`220`, `250`, `354`, `221`), and dissect encapsulated RFC 5322 IMF packet. | [Wireshark Guide](exp03_wireshark_smtp/WIRESHARK_GUIDE.md) \| [Lab Record](exp03_wireshark_smtp/RECORD.md) |
| **4** | **DNS** | Flush OS/browser DNS cache (`ipconfig /flushdns` or `resolvectl flush-caches`), capture DNS query and response resolving college website. Examine UDP port 53, Transaction ID, 16-bit flags (QR, AA, TC, RD, RA, RCODE), and record section counts (Questions, Answers, Authority, Additional). | [Wireshark Guide](exp04_wireshark_dns/WIRESHARK_GUIDE.md) \| [Lab Record](exp04_wireshark_dns/RECORD.md) |

---

### Part B: Socket Programming Experiments (Experiments 5 to 10)

| Expt No. | Protocol | Architecture / Concurrency | Description | Record Link |
| :---: | :---: | :---: | :--- | :--- |
| **5** | **TCP** | Connection-Oriented Stream | Client inputs order $N$, generates square matrix $[1, 50]$, sends to server. Server classifies whether it is **Diagonal**, **Upper Triangular**, **Lower Triangular**, or **General Matrix**, and returns result string. | [Lab Record](exp05_tcp_matrix_classification/RECORD.md) |
| **6** | **UDP** | Connectionless Datagram | Translates "new generation" English text abbreviations (`tbh`, `ig`, `tbf`, `atm`, `irl`, `lol`, `asap`, `omg`, `ttyl`, `idk`, `nvm`, `idc`) into formal English while preserving punctuation and casing. | [Lab Record](exp06_udp_slang_translator/RECORD.md) |
| **7** | **TCP** | Multi-threaded (`pthread`) | Multi-user chat server supporting concurrent client connections, handles, broadcast messaging, and join/leave notifications. | [Lab Record](exp07_tcp_multiuser_chat/RECORD.md) |
| **8** | **UDP** | Concurrent Multi-threaded | Concurrent Time Server executing on remote host. Handles UDP requests concurrently via worker threads, returning formatted system time along with server PID and thread metadata. | [Lab Record](exp08_udp_concurrent_time_server/RECORD.md) |
| **9** | **TCP** | Concurrent Multiprocess (`fork()`) | Concurrent file server using `fork()`. Checks file existence, streams file contents (or error), and returns the worker server Process ID (PID) to verify process isolation. Reaps zombies via `SIGCHLD`. | [Lab Record](exp09_tcp_concurrent_file_server/RECORD.md) |
| **10** | **RAW** | Link-Layer (`AF_PACKET`) | Packet sniffer using raw sockets (`ETH_P_ALL`). Decodes Ethernet MAC headers, IPv4 headers, TCP/UDP/ICMP transport headers, and inspects payload hex/ASCII dumps. | [Lab Record](exp10_raw_socket_packet_sniffer/RECORD.md) |

---

### Part C: Cisco Packet Tracer Based Experiments (Experiments 11 to 17)

| Expt No. | Topic | Focus & Verification | Guide & Record Links |
| :---: | :---: | :--- | :--- |
| **11** | **Router Commands Familiarization** | Navigating User (`>`), Privileged (`#`), and Config (`(config)#`) modes. System info (`show version`), routing table (`show ip route`), command history, software clock, interface stats (brief and detailed), DTE/DCE controller inspection (`show controllers`), configuring Serial and GigabitEthernet ports with IP addresses, masks, and clock rates. | [Packet Tracer Guide](exp11_router_commands_familiarization/PACKET_TRACER_GUIDE.md) \| [Lab Record](exp11_router_commands_familiarization/RECORD.md) |
| **12** | **Static Routing** | Multi-router internetwork (Figure 1 in syllabus). Configuring static routes (`ip route <net> <mask> <next-hop>`) across 4 routers (2621, 2501A, 2501B, 2501C). Verifying static route flag `S [1/0]` in routing table, hop-by-hop `tracert`, and end-to-end `ping`. | [Packet Tracer Guide](exp12_static_routing/PACKET_TRACER_GUIDE.md) \| [Lab Record](exp12_static_routing/RECORD.md) |
| **13** | **RIPv2 Dynamic Routing** | Figure 1 topology running RIPv2. Classless routing with `version 2`, `no auto-summary`, and `network 172.16.0.0`. Inspecting periodic 30s updates to multicast `224.0.0.9`, passive interfaces, hop count metric, administrative distance 120, and convergence verification. | [Packet Tracer Guide](exp13_ripv2_routing/PACKET_TRACER_GUIDE.md) \| [Lab Record](exp13_ripv2_routing/RECORD.md) |
| **14** | **OSPF Dynamic Routing** | Figure 1 topology configured in single-area OSPF Area 0. Router IDs, wildcard mask calculations, Link-State Advertisements (LSAs), neighbor adjacencies (`FULL` state), Dijkstra's SPF cost metric calculation ($10^8/\text{BW}$), and end-to-end `ping`. | [Packet Tracer Guide](exp14_ospf_routing/PACKET_TRACER_GUIDE.md) \| [Lab Record](exp14_ospf_routing/RECORD.md) |
| **15** | **Standard ACL (Campus Network)** | Campus network (Figure 2 in syllabus). Requirement: Allow ONLY Host_B to communicate with administrative network `172.16.10.0`, while restricting student Lab_B and Lab_C hosts. Applying Standard ACL 10 outbound on Router-A (close to destination), verifying ping from Host_B (Allowed) vs Lab_B/C (Blocked). | [Packet Tracer Guide](exp15_standard_acl_campus/PACKET_TRACER_GUIDE.md) \| [Lab Record](exp15_standard_acl_campus/RECORD.md) |
| **16** | **Extended ACL (Selective Port Filtering)** | Subnetting Class B network `140.80.0.0` into 20 subnets ($/21$, block size 8). Identifying CCF in Subnet 4 (`140.80.32.0/21`) and hackathon server at 7th address in Subnet 16 (`140.80.128.7`). Configuring Extended ACL 101 to block HTTP port 80 / HTTPS port 443 from CCF while allowing ICMP ping and FTP file services. | [Packet Tracer Guide](exp16_extended_acl_hackathon/PACKET_TRACER_GUIDE.md) \| [Lab Record](exp16_extended_acl_hackathon/RECORD.md) |
| **17** | **IPv6 Routing using RIPng** | IPv6 multi-subnet topology (Figure 3 in syllabus). Enabling `ipv6 unicast-routing`, defining RIPng process `CISCO_RIP`, interface-level enablement (`ipv6 rip <name> enable`), link-local next-hop addresses (`FE80::`), and UDP port 521 multicasts to `FF02::9`. Verified with `ping` and `tracert`. | [Packet Tracer Guide](exp17_ipv6_ripng_routing/PACKET_TRACER_GUIDE.md) \| [Lab Record](exp17_ipv6_ripng_routing/RECORD.md) |

---

## 🎥 Verified Video Tutorials & Online Learning References

| Topic / Lab | Verified Video Link | Instructor / Channel |
| :--- | :--- | :--- |
| **Expt 2: HTTP Packet Analysis Demo** | [NW LAB 17: HTTP Packet Analysis in Wireshark](https://www.youtube.com/watch?v=XJTOE3Q3_7c) | GoLearnerz (KTU Networks Lab) |
| **HTTP Protocol & Caching Theory** | [2.2 The Web and HTTP (part 1)](https://www.youtube.com/watch?v=S9GEPaQ1lFs) | Prof. Jim Kurose (Textbook Author) |
| **Expt 3: SMTP Email Analysis Demo** | [NW LAB 18: SMTP Packet Analysis in Wireshark](https://www.youtube.com/watch?v=D2zxmyv5wRU) | GoLearnerz (KTU Networks Lab) |
| **Electronic Mail & SMTP Theory** | [2.3 Email](https://www.youtube.com/watch?v=D3GMrOMR2dk) | Prof. Jim Kurose (Textbook Author) |
| **Expt 4: DNS Packet Analysis Demo** | [NW LAB 19: DNS Packet Analysis in Wireshark](https://www.youtube.com/watch?v=97GhdtkIEQk) | GoLearnerz (KTU Networks Lab) |
| **DNS Architecture & Hierarchy Theory** | [2.4 The Domain Name System (DNS)](https://www.youtube.com/watch?v=6lRcMh5Yphg) | Prof. Jim Kurose (Textbook Author) |
| **Expt 11: Router Commands Familiarization** | [Basic Router Configuration in Packet Tracer](https://www.youtube.com/watch?v=J_7s5E12p3g) | Jeremy's IT Lab |
| **Expt 12: Static Routing Full Walkthrough** | [Free CCNA Lab 024: Static Routing](https://www.youtube.com/watch?v=4T19U81s4kY) | Jeremy's IT Lab |
| **Expt 13: RIPv2 Dynamic Routing Protocol** | [Day 25: RIP & EIGRP (CCNA 200-301)](https://www.youtube.com/watch?v=J_7s5E12p3g) | Jeremy's IT Lab |
| **Expt 14: Single-Area OSPF Configuration** | [FREE CCNA Lab 054: OSPF Single Area](https://www.youtube.com/watch?v=J_7s5E12p3g) | Jeremy's IT Lab |
| **Expt 15: Standard ACL Configuration** | [Free CCNA | Standard ACLs | Day 34](https://www.youtube.com/watch?v=FdX83Q9HCxK) | Jeremy's IT Lab |
| **Expt 16: Extended ACL & Port Filtering** | [Extended ACL Lab: CCNA 200-301](https://www.youtube.com/watch?v=ESniaVzFYpv) | Jeremy's IT Lab |
| **Expt 17: IPv6 Routing using RIPng** | [Packet Tracer: Configuring RIPng (7.3.2.3)](https://www.youtube.com/watch?v=kYJzXv-i2_E) | CCNA Lab Walkthroughs |
| **Wireshark Setup & Packet Dissection** | [Learn Wireshark! Tutorial for BEGINNERS](https://www.youtube.com/watch?v=OU-A2EmVrKQ) | Chris Greer (Packet Pioneer) |
| **Official Kurose/Ross Wireshark Labs** | [Wireshark Labs Official Page](http://gaia.cs.umass.edu/kurose_ross/wireshark.php) | Kurose & Ross |
| **NPTEL Networks Course** | [NPTEL Computer Networks Course](https://nptel.ac.in/courses/106106091) | IIT Kharagpur (Syllabus Ref) |
# ktu_computer_network_lab24scheme
