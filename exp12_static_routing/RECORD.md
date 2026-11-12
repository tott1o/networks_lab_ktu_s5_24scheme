# EXPERIMENT 12: STATIC ROUTING IMPLEMENTATION

**Course Code:** PCCSL507 — Networks Lab  
**Course Name:** Computer Science & Engineering Networks Laboratory  
**Semester:** S5  

---

## 1. AIM
To configure and verify static routing across a multi-router internetwork (Figure 1 in syllabus) using Cisco Packet Tracer, display the IPv4 routing table of all routers, and verify end-to-end IP packet delivery using `ping` and `traceroute`.

---

## 2. HARDWARE AND SOFTWARE REQUIREMENTS
1. **Simulation Platform:** Cisco Packet Tracer (v8.x or above)
2. **Simulated Devices:**
   - 4 × Cisco Routers (Cisco 1941 / 2911 ISR series; or Router-PT / 2811) equipped with HHWIC-2T serial modules
   - 3 × Workstations (PC-1, PC-2, PC-3)
   - 1 × Cisco Catalyst 2960 switch (for the shared Ethernet segment)
   - Cabling: Copper Straight-Through cables, Serial DCE/DTE cables
3. **Host System:** Personal Computer running Windows / Linux

---

## 3. THEORY & PRINCIPLES

### 3.1 Principles of Static Routing
A router forwards packets based on its routing table. By default, a router only knows about its **directly connected networks** (flagged as `C` in the routing table). Any packet destined for a non-local network is dropped with an ICMP *Destination Host Unreachable* message unless a route exists.

**Static routing** is the process where a network administrator manually installs routing paths using the global configuration command:
$$\text{\texttt{ip route }} \langle\text{destination-network}\rangle \;\langle\text{subnet-mask}\rangle \;\langle\text{next-hop-ip} \mid \text{exit-interface}\rangle$$

### 3.2 Key Characteristics of Static Routes
1. **Administrative Distance (AD):**
   - Directly Connected Network: $\text{AD} = 0$
   - Static Route pointing to Next-Hop IP: $\text{AD} = 1$
   - Lower AD indicates higher trustworthiness. Static routes override all dynamic routing protocols (e.g., OSPF $\text{AD}=110$, RIP $\text{AD}=120$).
2. **Zero Protocol Overhead:** Static routes generate zero network bandwidth overhead because no periodic routing updates or hellos are exchanged.
3. **Predictable Determinism:** Routing paths do not fluctuate dynamically unless manually changed by the administrator.
4. **Bidirectional Requirement:** Routing must be bidirectional. For communication to occur between Host A and Host B, both the forward path (from A to B) and the reverse path (from B back to A) must be present in every intermediate router's routing table.

---

## 4. NETWORK TOPOLOGY DIAGRAM

```text
 [172.16.10.0/24]                [172.16.20.0/24]                 [172.16.40.0/24]              [172.16.50.0/24]
      Ethernet                      Serial WAN                       Serial WAN                      Ethernet
[Router 2621] ============ [Router 2501A] ----------------- [Router 2501B] ----------------- [Router 2501C] === [PC-3]
  Gig0/0: .1                 Gig0/0: .2  Se0/0/0: .1      Se0/0/0: .2  Se0/0/1: .1     Se0/0/0: .2  Gig0/0: .1    .10
      |                                                    |                                           
    [PC-1]                                               Gig0/0: .1                                     
     .10                                                   |                                           
                                                         [PC-2] .10                                    
                                                    [172.16.30.0/24] LAN                               
```

---

## 5. IP ADDRESSING TABLE

| Device | Interface | IP Address | Subnet Mask | Gateway | Description |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Router 2621** | `Gig0/0` | `172.16.10.1` | `255.255.255.0` | N/A | LAN 172.16.10.0/24 |
| **Router 2501A**| `Gig0/0` | `172.16.10.2` | `255.255.255.0` | N/A | Uplink to 2621 |
| | `Se0/0/0` | `172.16.20.1` | `255.255.255.0` | N/A | WAN link to 2501B (DCE) |
| **Router 2501B**| `Se0/0/0` | `172.16.20.2` | `255.255.255.0` | N/A | WAN link from 2501A (DTE) |
| | `Gig0/0` | `172.16.30.1` | `255.255.255.0` | N/A | LAN 172.16.30.0/24 |
| | `Se0/0/1` | `172.16.40.1` | `255.255.255.0` | N/A | WAN link to 2501C (DCE) |
| **Router 2501C**| `Se0/0/0` | `172.16.40.2` | `255.255.255.0` | N/A | WAN link from 2501B (DTE) |
| | `Gig0/0` | `172.16.50.1` | `255.255.255.0` | N/A | LAN 172.16.50.0/24 |
| **PC-1** | `FastEthernet0` | `172.16.10.10` | `255.255.255.0` | `172.16.10.1` | Client Host 1 |
| **PC-2** | `FastEthernet0` | `172.16.30.10` | `255.255.255.0` | `172.16.30.1` | Client Host 2 |
| **PC-3** | `FastEthernet0` | `172.16.50.10` | `255.255.255.0` | `172.16.50.1` | Client Host 3 |

---

## 6. PROCEDURE / ALGORITHM

1. **Topology Assembly:**
   - Launch Cisco Packet Tracer and assemble 4 routers (2811 series), 1 switch (Catalyst 2960), and 3 PCs.
   - Power off routers, insert `HHWIC-2T` serial modules into Slot 0, and power back on.
   - Interconnect routers using Serial DCE cables for WAN links and Straight-Through cables for LANs.
2. **Interface Addressing:**
   - Assign IP addresses and subnet masks to all interfaces according to the Addressing Table.
   - Configure clock rate (`clock rate 64000`) on all DCE serial interfaces.
   - Bring interfaces up with the `no shutdown` command.
3. **Static Route Calculation & Configuration:**
   - On **Router 2621**: Configure static routes for subnets `172.16.20.0/24`, `172.16.30.0/24`, `172.16.40.0/24`, and `172.16.50.0/24` pointing to next-hop `172.16.10.2`.
   - On **Router 2501A**: Configure static routes for subnets `172.16.30.0/24`, `172.16.40.0/24`, and `172.16.50.0/24` pointing to next-hop `172.16.20.2`.
   - On **Router 2501B**: Configure static route to `172.16.10.0/24` via `172.16.20.1`, and static route to `172.16.50.0/24` via `172.16.40.2`.
   - On **Router 2501C**: Configure static routes for subnets `172.16.10.0/24`, `172.16.20.0/24`, and `172.16.30.0/24` pointing to next-hop `172.16.40.1`.
4. **Verification:**
   - Display routing tables on all 4 routers using `show ip route`.
   - Verify static route flag `S` and administrative distance `[1/0]`.
   - Perform end-to-end ICMP ping tests between PC-1, PC-2, and PC-3.
   - Verify multi-hop transit using `tracert`.

---

## 7. CISCO IOS ROUTER CONFIGURATIONS

### 7.1 Router 2621
```text
Router> enable
Router# configure terminal
Router(config)# hostname R_2621
R_2621(config)# interface GigabitEthernet0/0
R_2621(config-if)# ip address 172.16.10.1 255.255.255.0
R_2621(config-if)# no shutdown
R_2621(config-if)# exit
R_2621(config)# ip route 172.16.20.0 255.255.255.0 172.16.10.2
R_2621(config)# ip route 172.16.30.0 255.255.255.0 172.16.10.2
R_2621(config)# ip route 172.16.40.0 255.255.255.0 172.16.10.2
R_2621(config)# ip route 172.16.50.0 255.255.255.0 172.16.10.2
R_2621(config)# end
R_2621# copy running-config startup-config
```

### 7.2 Router 2501A
```text
Router> enable
Router# configure terminal
Router(config)# hostname R_2501A
R_2501A(config)# interface GigabitEthernet0/0
R_2501A(config-if)# ip address 172.16.10.2 255.255.255.0
R_2501A(config-if)# no shutdown
R_2501A(config-if)# exit
R_2501A(config)# interface Serial0/0/0
R_2501A(config-if)# ip address 172.16.20.1 255.255.255.0
R_2501A(config-if)# clock rate 64000
R_2501A(config-if)# no shutdown
R_2501A(config-if)# exit
R_2501A(config)# ip route 172.16.30.0 255.255.255.0 172.16.20.2
R_2501A(config)# ip route 172.16.40.0 255.255.255.0 172.16.20.2
R_2501A(config)# ip route 172.16.50.0 255.255.255.0 172.16.20.2
R_2501A(config)# end
R_2501A# copy running-config startup-config
```

### 7.3 Router 2501B
```text
Router> enable
Router# configure terminal
Router(config)# hostname R_2501B
R_2501B(config)# interface Serial0/0/0
R_2501B(config-if)# ip address 172.16.20.2 255.255.255.0
R_2501B(config-if)# no shutdown
R_2501B(config-if)# exit
R_2501B(config)# interface GigabitEthernet0/0
R_2501B(config-if)# ip address 172.16.30.1 255.255.255.0
R_2501B(config-if)# no shutdown
R_2501B(config-if)# exit
R_2501B(config)# interface Serial0/0/1
R_2501B(config-if)# ip address 172.16.40.1 255.255.255.0
R_2501B(config-if)# clock rate 64000
R_2501B(config-if)# no shutdown
R_2501B(config-if)# exit
R_2501B(config)# ip route 172.16.10.0 255.255.255.0 172.16.20.1
R_2501B(config)# ip route 172.16.50.0 255.255.255.0 172.16.40.2
R_2501B(config)# end
R_2501B# copy running-config startup-config
```

### 7.4 Router 2501C
```text
Router> enable
Router# configure terminal
Router(config)# hostname R_2501C
R_2501C(config)# interface Serial0/0/0
R_2501C(config-if)# ip address 172.16.40.2 255.255.255.0
R_2501C(config-if)# no shutdown
R_2501C(config-if)# exit
R_2501C(config)# interface GigabitEthernet0/0
R_2501C(config-if)# ip address 172.16.50.1 255.255.255.0
R_2501C(config-if)# no shutdown
R_2501C(config-if)# exit
R_2501C(config)# ip route 172.16.10.0 255.255.255.0 172.16.40.1
R_2501C(config)# ip route 172.16.20.0 255.255.255.0 172.16.40.1
R_2501C(config)# ip route 172.16.30.0 255.255.255.0 172.16.40.1
R_2501C(config)# end
R_2501C# copy running-config startup-config
```

---

## 8. OBSERVATIONS & VERIFICATION LOGS

### 8.1 Routing Table on Router 2501B
```text
R_2501B# show ip route
Codes: C - connected, S - static, R - RIP, M - mobile, B - BGP
       D - EIGRP, EX - EIGRP external, O - OSPF

Gateway of last resort is not set

S    172.16.10.0/24 [1/0] via 172.16.20.1
C    172.16.20.0/24 is directly connected, Serial0/0/0
C    172.16.30.0/24 is directly connected, GigabitEthernet0/0
C    172.16.40.0/24 is directly connected, Serial0/0/1
S    172.16.50.0/24 [1/0] via 172.16.40.2
```

### 8.2 End-to-End Ping Test (PC-1 to PC-3)
```cmd
C:\> ping 172.16.50.10

Pinging 172.16.50.10 with 32 bytes of data:
Reply from 172.16.50.10: bytes=32 time=9ms TTL=125
Reply from 172.16.50.10: bytes=32 time=8ms TTL=125
Reply from 172.16.50.10: bytes=32 time=7ms TTL=125
Reply from 172.16.50.10: bytes=32 time=8ms TTL=125

Ping statistics for 172.16.50.10:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss),
Approximate round trip times in milli-seconds:
    Minimum = 7ms, Maximum = 9ms, Average = 8ms
```

### 8.3 Hop-by-Hop Trace Route (`tracert`)
```cmd
C:\> tracert 172.16.50.10

Tracing route to 172.16.50.10 over a maximum of 30 hops:

  1   <1 ms    172.16.10.1
  2    2 ms    172.16.10.2
  3    5 ms    172.16.20.2
  4    7 ms    172.16.40.2
  5    8 ms    172.16.50.10

Trace complete.
```

---

## 9. VIVA VOCE QUESTIONS & ANSWERS

**Q1: What does `[1/0]` represent next to a static route in `show ip route`?**  
*Ans:* `[1/0]` represents `[Administrative Distance / Metric]`. For a static route pointing to a next-hop IP, the Administrative Distance is `1` (indicating high priority), and the metric cost is `0`.

**Q2: What is a default static route and when should it be configured?**  
*Ans:* A default static route (`ip route 0.0.0.0 0.0.0.0 <next-hop>`) matches all destination IP addresses for which no specific route exists in the routing table. It is ideal for stub routers (like Router 2621 or Router 2501C) having only one path out of the network.

**Q3: What happens if you configure a static route specifying only the exit interface on an Ethernet broadcast network?**  
*Ans:* The router assumes the destination network is directly attached to the broadcast medium and attempts to send ARP requests for every destination IP, resulting in massive ARP cache bloat and potential packet drops unless proxy ARP is enabled.

**Q4: How does a floating static route provide backup redundancy?**  
*Ans:* A floating static route is configured with an administrative distance higher than the primary routing protocol (e.g., `ip route 172.16.50.0 255.255.255.0 172.16.60.2 150`). It remains dormant and only installs into the routing table if the primary route fails.

**Q5: Why did the first ICMP packet sometimes experience timeout during ping?**  
*Ans:* Address Resolution Protocol (ARP) must resolve the Layer 2 MAC address of the gateway or destination host before encapsulating the ICMP packet, occasionally causing the first packet to time out while ARP cache updates.

---

## 10. RESULT
Static routing was successfully configured on the multi-router network in Cisco Packet Tracer. All routing tables correctly displayed static routing entries (`S [1/0]`), and 100% end-to-end IP reachability was confirmed using `ping` and `tracert`.
