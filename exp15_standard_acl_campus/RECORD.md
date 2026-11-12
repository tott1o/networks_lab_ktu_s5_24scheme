# EXPERIMENT 15: ACCESS CONTROL USING STANDARD ACCESS LISTS (ACL)

**Course Code:** PCCSL507 — Networks Lab  
**Course Name:** Computer Science & Engineering Networks Laboratory  
**Semester:** S5  

---

## 1. AIM
To implement traffic filtering on a college campus network (Figure 2 in syllabus) using Standard Access Control Lists (ACLs) in Cisco Packet Tracer, allowing only `Host_B` to communicate with the protected administrative subnet `172.16.10.0`, while restricting student laboratory hosts (`Lab_B` and `Lab_C`), and verifying enforcement via ICMP `ping` testing and ACL match counter analysis.

---

## 2. HARDWARE AND SOFTWARE REQUIREMENTS
1. **Simulation Environment:** Cisco Packet Tracer (v8.x or above)
2. **Devices Simulated:**
   - 3 × Cisco Routers (Cisco 1941 / 2911 ISR series; or Router-PT / 2811) equipped with HHWIC-2T serial modules
   - 3 × Cisco Catalyst 2960 Switches
   - 4 × End-user PCs (Host_A, Host_B, Lab_B, Lab_C)
   - Cabling: Copper Straight-Through cables, Serial DCE/DTE cables
3. **Host Computer:** PC running Windows / Linux

---

## 3. THEORY & PRINCIPLES

### 3.1 Overview of Access Control Lists (ACLs)
An Access Control List (ACL) is a sequential collection of permit or deny statements applied to IP addresses or upper-layer protocols. Routers evaluate packets passing through an interface against the ACL entries in sequential, top-down order.

### 3.2 Standard vs. Extended ACLs
| Attribute | Standard ACL | Extended ACL |
| :--- | :--- | :--- |
| **Numbered Range** | `1 – 99` and `1300 – 1999` | `100 – 199` and `2000 – 2699` |
| **Inspection Scope** | Source IP address only | Source IP, Destination IP, Protocol (TCP/UDP/ICMP), Port numbers |
| **Placement Guideline** | As close to the **destination** as possible | As close to the **source** as possible |

### 3.3 Rule Evaluation Mechanics
1. **Top-Down Sequential Processing:** Packets are compared against ACL entries in order. Once a match occurs, the permit or deny action is applied immediately, and no further lines are checked.
2. **Implicit Deny Any:** At the very end of every access list, an invisible, default statement exists:  
   $$\text{\texttt{deny any}}$$  
   If a packet does not explicitly match any permit statement in the list, it is dropped.
3. **Interface Application Direction:**
   - **Inbound (`in`):** Packet is filtered before being processed by the router's routing engine.
   - **Outbound (`out`):** Packet has already been routed through the router and is filtered just before being transmitted out onto the physical interface wire.

### 3.4 Placement Rationale for Standard ACL
Because standard ACLs can only filter based on the source IP and cannot distinguish destination networks, applying a standard ACL near the source would block traffic indiscriminately to all destinations. Therefore, Standard ACLs must always be placed **as close to the destination as possible** (Router-A's outbound interface towards `172.16.10.0`).

---

## 4. NETWORK TOPOLOGY DIAGRAM

```text
  [Host_A Subnet: 172.16.10.0/24]      [Host_B Subnet: 172.16.30.0/24]         [Labs Subnet: 172.16.50.0/24]
               Host_A                              Host_B                            Lab_B        Lab_C
           (172.16.10.10)                      (172.16.30.2)                     (172.16.50.10) (172.16.50.20)
                 |                                   |                                  \          /
             [Switch 1]                          [Switch 2]                               [Switch 3]
                 |                                   |                                        |
              Gig0/0: .1                           Gig0/0: .1                                Gig0/0: .1
           [Router-A (R1)] ================== [Router-B (R2)] ======================= [Router-C (R3)]
            Se0/0/0: .1     Serial 172.16.20.0/24 Se0/0/0: .2  Se0/0/1: .1  Serial 172.16.40.0/24  Se0/0/0: .2
          [APPLY ACL 10 HERE]
          (Outbound on Gig0/0)
```

---

## 5. IP ADDRESSING TABLE

| Device | Interface | IP Address | Subnet Mask | Default Gateway | Remarks |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Host_A** | `FastEthernet0` | `172.16.10.10` | `255.255.255.0` | `172.16.10.1` | Destination Campus Server |
| **Host_B** | `FastEthernet0` | `172.16.30.2` | `255.255.255.0` | `172.16.30.1` | Authorized Campus Host |
| **Lab_B** | `FastEthernet0` | `172.16.50.10` | `255.255.255.0` | `172.16.50.1` | Blocked Student Host 1 |
| **Lab_C** | `FastEthernet0` | `172.16.50.20` | `255.255.255.0` | `172.16.50.1` | Blocked Student Host 2 |
| **Router-A** | `Gig0/0` | `172.16.10.1` | `255.255.255.0` | N/A | Gateway (ACL 10 Outbound) |
| | `Se0/0/0` | `172.16.20.1` | `255.255.255.0` | N/A | Serial DCE (Clock 64000) |
| **Router-B** | `Se0/0/0` | `172.16.20.2` | `255.255.255.0` | N/A | Serial DTE from Router-A |
| | `Gig0/0` | `172.16.30.1` | `255.255.255.0` | N/A | Gateway to Host_B |
| | `Se0/0/1` | `172.16.40.1` | `255.255.255.0` | N/A | Serial DCE (Clock 64000) |
| **Router-C** | `Se0/0/0` | `172.16.40.2` | `255.255.255.0` | N/A | Serial DTE from Router-B |
| | `Gig0/0` | `172.16.50.1` | `255.255.255.0` | N/A | Gateway to Lab_B/C |

---

## 6. PROCEDURE / ALGORITHM

1. Set up the campus network topology in Cisco Packet Tracer with 3 routers, 3 switches, and 4 PCs as per Figure 2.
2. Configure IP addresses and masks on all interfaces and bring them up with `no shutdown`.
3. Enable dynamic routing (OSPF single-area or static routing) across all routers to verify underlying baseline reachability.
4. Verify initial baseline connectivity before ACL enforcement: Ensure all PCs can ping Host_A.
5. Create Standard ACL 10 on Router-A:
   - `access-list 10 permit host 172.16.30.2`
   - `access-list 10 deny any`
6. Apply the access list outbound on Router-A's LAN interface towards the protected subnet:
   - `interface GigabitEthernet0/0`
   - `ip access-group 10 out`
7. Perform the verification checks required by the syllabus:
   - Check (a): Ping Host_A (`172.16.10.10`) from Host_B (`172.16.30.2`) — Must SUCCEED.
   - Check (b): Ping Host_A (`172.16.10.10`) from Lab_B (`172.16.50.10`) — Must FAIL.
   - Check (c): Ping Host_A (`172.16.10.10`) from Lab_C (`172.16.50.20`) — Must FAIL.
8. Inspect match counters on Router-A using `show access-lists 10`.

---

## 7. CISCO IOS ROUTER CONFIGURATIONS

### 7.1 Router-A Configuration (Including ACL 10)
```text
Router> enable
Router# configure terminal
Router(config)# hostname Router-A
Router-A(config)# interface GigabitEthernet0/0
Router-A(config-if)# ip address 172.16.10.1 255.255.255.0
Router-A(config-if)# no shutdown
Router-A(config-if)# exit
Router-A(config)# interface Serial0/0/0
Router-A(config-if)# ip address 172.16.20.1 255.255.255.0
Router-A(config-if)# clock rate 64000
Router-A(config-if)# no shutdown
Router-A(config-if)# exit

! Dynamic Routing (OSPF)
Router-A(config)# router ospf 1
Router-A(config-router)# network 172.16.10.0 0.0.0.255 area 0
Router-A(config-router)# network 172.16.20.0 0.0.0.255 area 0
Router-A(config-router)# exit

! Standard ACL Definition
Router-A(config)# access-list 10 remark Allow only Host_B to communicate with 172.16.10.0
Router-A(config)# access-list 10 permit host 172.16.30.2
Router-A(config)# access-list 10 deny any

! Apply ACL Outbound on Gig0/0 towards Host_A
Router-A(config)# interface GigabitEthernet0/0
Router-A(config-if)# ip access-group 10 out
Router-A(config-if)# end
Router-A# copy running-config startup-config
```

### 7.2 Router-B Configuration
```text
Router> enable
Router# configure terminal
Router(config)# hostname Router-B
Router-B(config)# interface Serial0/0/0
Router-B(config-if)# ip address 172.16.20.2 255.255.255.0
Router-B(config-if)# no shutdown
Router-B(config-if)# exit
Router-B(config)# interface GigabitEthernet0/0
Router-B(config-if)# ip address 172.16.30.1 255.255.255.0
Router-B(config-if)# no shutdown
Router-B(config-if)# exit
Router-B(config)# interface Serial0/0/1
Router-B(config-if)# ip address 172.16.40.1 255.255.255.0
Router-B(config-if)# clock rate 64000
Router-B(config-if)# no shutdown
Router-B(config-if)# exit
Router-B(config)# router ospf 1
Router-B(config-router)# network 172.16.20.0 0.0.0.255 area 0
Router-B(config-router)# network 172.16.30.0 0.0.0.255 area 0
Router-B(config-router)# network 172.16.40.0 0.0.0.255 area 0
Router-B(config-router)# end
Router-B# copy running-config startup-config
```

### 7.3 Router-C Configuration
```text
Router> enable
Router# configure terminal
Router(config)# hostname Router-C
Router-C(config)# interface Serial0/0/0
Router-C(config-if)# ip address 172.16.40.2 255.255.255.0
Router-C(config-if)# no shutdown
Router-C(config-if)# exit
Router-C(config)# interface GigabitEthernet0/0
Router-C(config-if)# ip address 172.16.50.1 255.255.255.0
Router-C(config-if)# no shutdown
Router-C(config-if)# exit
Router-C(config)# router ospf 1
Router-C(config-router)# network 172.16.40.0 0.0.0.255 area 0
Router-C(config-router)# network 172.16.50.0 0.0.0.255 area 0
Router-C(config-router)# end
Router-C# copy running-config startup-config
```

---

## 8. OBSERVATIONS & TEST OUTPUTS

### 8.1 Test (a): Ping Host_A from Host_B (Allowed)
```cmd
C:\> ping 172.16.10.10

Pinging 172.16.10.10 with 32 bytes of data:
Reply from 172.16.10.10: bytes=32 time=5ms TTL=126
Reply from 172.16.10.10: bytes=32 time=4ms TTL=126
Reply from 172.16.10.10: bytes=32 time=4ms TTL=126
Reply from 172.16.10.10: bytes=32 time=5ms TTL=126

Ping statistics for 172.16.10.10:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss)
```
*Result:* Success. Host_B has explicit permission to communicate with 172.16.10.0.

### 8.2 Test (b): Ping Host_A from Lab_B (Blocked)
```cmd
C:\> ping 172.16.10.10

Pinging 172.16.10.10 with 32 bytes of data:
Reply from 172.16.20.1: Destination host unreachable.
Reply from 172.16.20.1: Destination host unreachable.
Reply from 172.16.20.1: Destination host unreachable.
Reply from 172.16.20.1: Destination host unreachable.

Ping statistics for 172.16.10.10:
    Packets: Sent = 4, Received = 0, Lost = 4 (100% loss)
```
*Result:* Blocked. Router-A drops packets originating from `172.16.50.10` and sends ICMP unreachable.

### 8.3 Test (c): Ping Host_A from Lab_C (Blocked)
```cmd
C:\> ping 172.16.10.10

Pinging 172.16.10.10 with 32 bytes of data:
Reply from 172.16.20.1: Destination host unreachable.
Reply from 172.16.20.1: Destination host unreachable.
Reply from 172.16.20.1: Destination host unreachable.
Reply from 172.16.20.1: Destination host unreachable.

Ping statistics for 172.16.10.10:
    Packets: Sent = 4, Received = 0, Lost = 4 (100% loss)
```
*Result:* Blocked. Packet filter verified.

### 8.4 Inspection of ACL Counters on Router-A
```text
Router-A# show access-lists 10
Standard IP access list 10
    10 permit 172.16.30.2 (4 matches)
    20 deny any (8 matches)
```

---

## 9. VIVA VOCE QUESTIONS & ANSWERS

**Q1: Why must Standard ACLs be placed as close to the destination as possible?**  
*Ans:* Standard ACLs can only filter based on source IP address and cannot specify destination addresses. Placing a standard ACL close to the source would filter that host from communicating with all other network destinations, rather than just the intended target.

**Q2: What is the keyword `host` shorthand for in Cisco ACL syntax?**  
*Ans:* The `host` keyword represents a single 32-bit IP address with a wildcard mask of `0.0.0.0` (e.g., `permit host 172.16.30.2` is identical to `permit 172.16.30.2 0.0.0.0`).

**Q3: Explain the function of the "implicit deny" statement in Cisco ACLs.**  
*Ans:* Cisco IOS automatically places an unwritten `deny any` at the bottom of every access list. Any packet that fails to match one of the preceding permit statements is unconditionally discarded.

**Q4: How does applying an ACL `in` differ from applying it `out`?**  
*Ans:* `in` filters packets as they enter the router interface before routing table lookup takes place. `out` filters packets after the router has decided which exit interface to use to forward the packet.

**Q5: Can you delete a single line from a classic numbered ACL without deleting the entire list?**  
*Ans:* In older IOS, executing `no access-list <N> ...` deletes the entire access list. In modern Cisco IOS, numbered ACLs can be edited by entering ACL configuration mode (`ip access-list standard 10`) and referencing sequence numbers (e.g., `no 20`).

---

## 10. RESULT
Standard Access Control List 10 was successfully configured on Router-A in Cisco Packet Tracer. Host_B was permitted to communicate with Host_A on network 172.16.10.0, while access from Lab_B and Lab_C was denied as verified by ICMP ping results and ACL match statistics.
