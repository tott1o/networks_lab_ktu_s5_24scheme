# EXPERIMENT 17: IPV6 ROUTING USING RIPng

**Course Code:** PCCSL507 — Networks Lab  
**Course Name:** Computer Science & Engineering Networks Laboratory  
**Semester:** S5  

---

## 1. AIM
To implement IPv6 dynamic routing using RIPng (Routing Information Protocol next generation) on the multi-subnet topology shown in Figure 3 of the syllabus using Cisco Packet Tracer, examine link-local next-hop addresses and IPv6 routing table convergence, and verify end-to-end communication between IPv6 hosts using `ping` and `traceroute`.

---

## 2. HARDWARE AND SOFTWARE REQUIREMENTS
1. **Simulation Platform:** Cisco Packet Tracer (v8.x or above)
2. **Devices Simulated:**
   - 3 × Cisco Routers (1941 / 2811 series) equipped with HWIC-2T / WIC-2T serial modules
   - 3 × Workstation PCs (Host-A, Host-B, Host-C)
   - Cabling: Serial DCE/DTE cables, Copper Straight-Through cables, Copper Crossover cables
3. **Host Computer:** PC running Windows / Linux

---

## 3. THEORY & PRINCIPLES

### 3.1 Overview of IPv6 Addressing
IPv6 addresses are **128 bits** in length, formatted as eight groups of four hexadecimal digits separated by colons (e.g., `2001:0DB8:1111:0001:0000:0000:0000:0001` or compressed `2001:DB8:1111:1::1`).
Key address types include:
- **Global Unicast Addresses (2000::/3):** Globally routable IPv6 addresses analogous to IPv4 public addresses.
- **Link-Local Addresses (FE80::/10):** Automatically generated on every IPv6 interface for communication on a single physical link. Routers never forward packets with link-local source/destination addresses beyond the local link.
- **Multicast Addresses (FF00::/8):** Used for one-to-many communication, replacing IPv4 broadcasts.

### 3.2 Fundamentals of RIPng (RFC 2080)
RIPng is the IPv6 extension of RIPv2 based on the distance-vector Bellman-Ford algorithm:
1. **Metric:** Hop count (maximum 15 hops; 16 = infinity / unreachable).
2. **Administrative Distance:** 120.
3. **Transport Protocol:** Runs directly over **UDP port 521** (IPv4 RIP uses UDP port 520).
4. **Multicast Address:** Transmits updates to **`FF02::9`** (All RIPng Routers multicast address).
5. **Next-Hop Addressing:** RIPng uses the **Link-Local IPv6 address (`FE80::/10`)** of the advertising router as the next-hop address in the routing table, rather than the global unicast IP.
6. **Interface-Level Enablement:** Unlike IPv4 RIP which uses global `network` statements, RIPng is enabled directly under interface configuration mode using `ipv6 rip <process-name> enable`.

---

## 4. NETWORK TOPOLOGY DIAGRAM

```text
  [Subnet 1: 2001:DB8:1111:1::/64]                                        [Subnet 2: 2001:DB8:1111:2::/64]
           [ Host-A ]                                                                  [ Host-B ]
       2001:DB8:1111:1::11                                                         2001:DB8:1111:2::12
               |                                                                           |
             (LAN)                                                                       (LAN)
               |                                                                           |
             G0/0: ::1                     [Subnet 4: 2001:DB8:1111:4::/64]              G0/0: ::1
        +--------------+                      WAN Serial Link (S0/0/0 to S0/0/1)    +--------------+
        |   ROUTER 1   | S0/0/0: ::1 (DCE) ------------------------ S0/0/1: ::2 |   ROUTER 2   |
        |     (R1)     |                                                            |     (R2)     |
        +--------------+                                                            +--------------+
             G0/1/0: ::1
               |
               |  [Subnet 5: 2001:DB8:1111:5::/64]
               |  Gigabit/Ethernet Link (G0/1/0 to G0/0/0)
               |
        +--------------+
        |   ROUTER 3   | G0/0/0: ::3
        |     (R3)     |
        +--------------+
             G0/0: ::1
               |
             (LAN)
               |
           [ Host-C ]
       2001:DB8:1111:3::13
  [Subnet 3: 2001:DB8:1111:3::/64]
```

---

## 5. IPV6 ADDRESSING TABLE

| Device | Interface | IPv6 Address / Prefix | Gateway | Role |
| :--- | :--- | :--- | :--- | :--- |
| **Host-A** | `Fa0` | `2001:DB8:1111:1::11/64` | `2001:DB8:1111:1::1` | Subnet 1 Host |
| **Host-B** | `Fa0` | `2001:DB8:1111:2::12/64` | `2001:DB8:1111:2::1` | Subnet 2 Host |
| **Host-C** | `Fa0` | `2001:DB8:1111:3::13/64` | `2001:DB8:1111:3::1` | Subnet 3 Host |
| **Router R1** | `G0/0` | `2001:DB8:1111:1::1/64` | N/A | Subnet 1 Gateway |
| | `S0/0/0` | `2001:DB8:1111:4::1/64` | N/A | Subnet 4 WAN DCE (Clock 64000) |
| | `G0/1` | `2001:DB8:1111:5::1/64` | N/A | Subnet 5 Link to R3 |
| **Router R2** | `S0/0/1` | `2001:DB8:1111:4::2/64` | N/A | Subnet 4 WAN DTE |
| | `G0/0` | `2001:DB8:1111:2::1/64` | N/A | Subnet 2 Gateway |
| **Router R3** | `G0/0` | `2001:DB8:1111:5::3/64` | N/A | Subnet 5 Link from R1 |
| | `G0/1` | `2001:DB8:1111:3::1/64` | N/A | Subnet 3 Gateway |

---

## 6. PROCEDURE / ALGORITHM

1. Construct the 3-router IPv6 topology in Cisco Packet Tracer as specified in Figure 3.
2. Globally enable IPv6 packet routing on all routers using the `ipv6 unicast-routing` command.
3. Configure the designated 64-bit global unicast IPv6 addresses on each router interface and bring ports up with `no shutdown`.
4. Configure clock rate (`clock rate 64000`) on the DCE serial interface of R1.
5. Create the named RIPng process on each router using `ipv6 router rip CISCO_RIP`.
6. Enter interface configuration mode for each active interface and bind the RIPng routing process using `ipv6 rip CISCO_RIP enable`.
7. Assign static IPv6 addresses and default gateways to workstations Host-A, Host-B, and Host-C.
8. Allow RIPng timers to converge (approximately 30 seconds).
9. Display the IPv6 routing table using `show ipv6 route` and inspect RIPng protocol parameters with `show ipv6 rip`.
10. Test end-to-end reachability between all IPv6 subnets using `ping` and trace route using `tracert`.

---

## 7. CISCO IOS ROUTER CONFIGURATIONS

### 7.1 Router R1
```text
Router> enable
Router# configure terminal
Router(config)# hostname R1
R1(config)# ipv6 unicast-routing
R1(config)# ipv6 router rip CISCO_RIP
R1(config-rtr)# exit

R1(config)# interface GigabitEthernet0/0
R1(config-if)# ipv6 address 2001:DB8:1111:1::1/64
R1(config-if)# ipv6 rip CISCO_RIP enable
R1(config-if)# no shutdown
R1(config-if)# exit

R1(config)# interface Serial0/0/0
R1(config-if)# ipv6 address 2001:DB8:1111:4::1/64
R1(config-if)# clock rate 64000
R1(config-if)# ipv6 rip CISCO_RIP enable
R1(config-if)# no shutdown
R1(config-if)# exit

R1(config)# interface GigabitEthernet0/1
R1(config-if)# ipv6 address 2001:DB8:1111:5::1/64
R1(config-if)# ipv6 rip CISCO_RIP enable
R1(config-if)# no shutdown
R1(config-if)# end
R1# copy running-config startup-config
```

### 7.2 Router R2
```text
Router> enable
Router# configure terminal
Router(config)# hostname R2
R2(config)# ipv6 unicast-routing
R2(config)# ipv6 router rip CISCO_RIP
R2(config-rtr)# exit

R2(config)# interface Serial0/0/1
R2(config-if)# ipv6 address 2001:DB8:1111:4::2/64
R2(config-if)# ipv6 rip CISCO_RIP enable
R2(config-if)# no shutdown
R2(config-if)# exit

R2(config)# interface GigabitEthernet0/0
R2(config-if)# ipv6 address 2001:DB8:1111:2::1/64
R2(config-if)# ipv6 rip CISCO_RIP enable
R2(config-if)# no shutdown
R2(config-if)# end
R2# copy running-config startup-config
```

### 7.3 Router R3
```text
Router> enable
Router# configure terminal
Router(config)# hostname R3
R3(config)# ipv6 unicast-routing
R3(config)# ipv6 router rip CISCO_RIP
R3(config-rtr)# exit

R3(config)# interface GigabitEthernet0/0
R3(config-if)# ipv6 address 2001:DB8:1111:5::3/64
R3(config-if)# ipv6 rip CISCO_RIP enable
R3(config-if)# no shutdown
R3(config-if)# exit

R3(config)# interface GigabitEthernet0/1
R3(config-if)# ipv6 address 2001:DB8:1111:3::1/64
R3(config-if)# ipv6 rip CISCO_RIP enable
R3(config-if)# no shutdown
R3(config-if)# end
R3# copy running-config startup-config
```

---

## 8. OBSERVATIONS & TEST OUTPUTS

### 8.1 Converged IPv6 Routing Table on Router R2 (`show ipv6 route`)
```text
R2# show ipv6 route

IPv6 Routing Table - 7 entries
Codes: C - Connected, L - Local, S - Static, R - RIP, B - BGP

R   2001:DB8:1111:1::/64 [120/2]
     via FE80::201:97FF:FE28:1A01, Serial0/0/1
C   2001:DB8:1111:2::/64 [0/0]
     via GigabitEthernet0/0, directly connected
L   2001:DB8:1111:2::1/128 [0/0]
     via GigabitEthernet0/0, receive
R   2001:DB8:1111:3::/64 [120/3]
     via FE80::201:97FF:FE28:1A01, Serial0/0/1
C   2001:DB8:1111:4::/64 [0/0]
     via Serial0/0/1, directly connected
L   2001:DB8:1111:4::2/128 [0/0]
     via Serial0/0/1, receive
R   2001:DB8:1111:5::/64 [120/2]
     via FE80::201:97FF:FE28:1A01, Serial0/0/1
```
*Key Inferences:*
- RIPng routes are identified by code **`R`**.
- Administrative Distance is **120**.
- The next-hop router is referenced via its link-local address (`FE80::...`).
- Route metric correctly reflects the accumulated hop count ($2$ hops for Subnet 1 and Subnet 5, $3$ hops for Subnet 3).

### 8.2 Protocol Status Output on R1 (`show ipv6 rip`)
```text
R1# show ipv6 rip
RIP process "CISCO_RIP", port 521, multicast-group FF02::9, pid 39
     Administrative distance is 120. Maximum paths is 16
     Updates every 30 seconds, expire after 180
     Holddown lasts 0 seconds, garbage collect after 120
     Split horizon is on; poison reverse is on
```

### 8.3 End-to-End IPv6 Ping Verification
```cmd
C:\> ping 2001:DB8:1111:2::12

Pinging 2001:DB8:1111:2::12 with 32 bytes of data:
Reply from 2001:DB8:1111:2::12: time=7ms
Reply from 2001:DB8:1111:2::12: time=6ms
Reply from 2001:DB8:1111:2::12: time=6ms
Reply from 2001:DB8:1111:2::12: time=7ms

Ping statistics for 2001:DB8:1111:2::12:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss)
```

### 8.4 Trace Route Verification (`tracert`)
```cmd
C:\> tracert 2001:DB8:1111:3::13

Tracing route to 2001:DB8:1111:3::13 over a maximum of 30 hops:

  1   <1 ms    2001:DB8:1111:1::1
  2    4 ms    2001:DB8:1111:5::3
  3    6 ms    2001:DB8:1111:3::13

Trace complete.
```

---

## 9. VIVA VOCE QUESTIONS & ANSWERS

**Q1: What command must be executed on a Cisco router before any IPv6 routing protocol will work?**  
*Ans:* `ipv6 unicast-routing`. Without this global command, the router acts only as an IPv6 host and will not route IPv6 packets or establish routing protocol adjacencies.

**Q2: What is the destination multicast address and transport port used by RIPng?**  
*Ans:* RIPng multicasts updates to **`FF02::9`** (All-RIP-Routers multicast group) using **UDP port 521**.

**Q3: Why does RIPng use link-local addresses instead of global unicast addresses as next-hops?**  
*Ans:* Link-local addresses (`FE80::/10`) are guaranteed to be unique on the local link and do not change even if global prefix renumbering occurs, ensuring robust neighbor communication independent of public addressing changes.

**Q4: How is RIPng configuration fundamentally different from IPv4 RIP configuration?**  
*Ans:* IPv4 RIP uses the global `network` command under `router rip` to match interface IP addresses. RIPng has no `network` command; instead, it is enabled directly under individual interfaces using `ipv6 rip <process-name> enable`.

**Q5: What is the maximum hop count in RIPng?**  
*Ans:* The maximum valid hop count is **15**. A metric of **16** denotes an unreachable network (infinity).

---

## 10. RESULT
The IPv6 network was successfully configured using RIPng in Cisco Packet Tracer. All routers dynamically exchanged IPv6 routing updates over multicast address `FF02::9`, populated their routing tables with link-local next hops, and established 100% end-to-end IP reachability as verified using `ping` and `tracert`.
