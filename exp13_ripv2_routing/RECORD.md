# EXPERIMENT 13: IMPLEMENTATION OF RIPv2 DYNAMIC ROUTING

**Course Code:** PCCSL507 — Networks Lab  
**Course Name:** Computer Science & Engineering Networks Laboratory  
**Semester:** S5  

---

## 1. AIM
To implement the Routing Information Protocol version 2 (RIPv2) on the multi-router network shown in Figure 1 of the syllabus using Cisco Packet Tracer, examine the convergence of the dynamic routing tables, inspect RIP timers and protocol statistics, and verify end-to-end IP reachability using `ping` and `traceroute`.

---

## 2. HARDWARE AND SOFTWARE REQUIREMENTS
1. **Simulation Platform:** Cisco Packet Tracer (v8.x or above)
2. **Devices Simulated:**
   - 4 × Cisco 2811 Routers with HWIC-2T serial modules
   - 3 × End-user PCs (PC-1, PC-2, PC-3)
   - 1 × Cisco Catalyst 2960 Switch
   - Cables: Serial DCE/DTE cables, Copper Straight-Through cables
3. **Host Machine:** PC running Windows / Linux

---

## 3. THEORY & PRINCIPLES

### 3.1 Overview of RIPv2
The Routing Information Protocol (RIP) is an Interior Gateway Protocol (IGP) based on the **Bellman-Ford distance-vector algorithm** (RFC 2453). RIP routers periodically broadcast/multicast their entire routing table to directly connected neighbors.

### 3.2 Key Characteristics of RIPv2
1. **Metric:** Uses **hop count** as its sole metric. One hop represents one router transit. The maximum allowable hop count is **15**. A hop count of **16** is considered infinity (unreachable network).
2. **Administrative Distance (AD):** $\text{AD} = 120$.
3. **Transport Layer:** Runs over **UDP port 520**.
4. **Multicast Transmission:** Transmits routing updates to the reserved multicast address **`224.0.0.9`**, preventing non-RIP hosts on the subnet from having to process the packets at the CPU level.
5. **Classless Routing (VLSM & CIDR):** RIPv2 includes the 32-bit subnet mask within update packets, allowing variable-length subnet masking and supernetting.
6. **Auto-Summarization:** By default, Cisco IOS auto-summarizes subnets at major class boundaries (`172.16.0.0/16`). The command **`no auto-summary`** is required to preserve specific `/24` subnet masks across the network.

### 3.3 RIP Timers
- **Update Timer (30 seconds):** Interval between periodic routing table advertisements.
- **Invalid / Timeout Timer (180 seconds):** If no update is received for a route within 180 seconds, its metric is set to 16 (marked invalid).
- **Hold-down Timer (180 seconds):** When a route is marked unreachable, new updates with equal or worse metrics are ignored for 180 seconds to prevent routing loops.
- **Flush Timer (240 seconds):** Time after which an invalid route is purged completely from the routing table.

### 3.4 Loop Prevention Mechanisms
- **Split Horizon:** A router never advertises a route back out the same interface through which it was learned.
- **Poison Reverse:** Explicitly advertises an unreachable route with metric 16 back to the neighbor to instantly poison the dead path.

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

| Device | Interface | IP Address | Subnet Mask | Connected Subnet |
| :--- | :--- | :--- | :--- | :--- |
| **Router 2621** | `Gig0/0` | `172.16.10.1` | `255.255.255.0` | 172.16.10.0/24 |
| **Router 2501A**| `Gig0/0` | `172.16.10.2` | `255.255.255.0` | 172.16.10.0/24 |
| | `Se0/0/0` | `172.16.20.1` | `255.255.255.0` | 172.16.20.0/24 (DCE, 64000) |
| **Router 2501B**| `Se0/0/0` | `172.16.20.2` | `255.255.255.0` | 172.16.20.0/24 (DTE) |
| | `Gig0/0` | `172.16.30.1` | `255.255.255.0` | 172.16.30.0/24 |
| | `Se0/0/1` | `172.16.40.1` | `255.255.255.0` | 172.16.40.0/24 (DCE, 64000) |
| **Router 2501C**| `Se0/0/0` | `172.16.40.2` | `255.255.255.0` | 172.16.40.0/24 (DTE) |
| | `Gig0/0` | `172.16.50.1` | `255.255.255.0` | 172.16.50.0/24 |
| **PC-1** | `FastEthernet0` | `172.16.10.10` | `255.255.255.0` | GW: 172.16.10.1 |
| **PC-2** | `FastEthernet0` | `172.16.30.10` | `255.255.255.0` | GW: 172.16.30.1 |
| **PC-3** | `FastEthernet0` | `172.16.50.10` | `255.255.255.0` | GW: 172.16.50.1 |

---

## 6. PROCEDURE / ALGORITHM

1. Set up the 4-router topology with Serial and FastEthernet connections in Cisco Packet Tracer.
2. Configure IP addresses and subnet masks on all router interfaces and bring them up with `no shutdown`.
3. Set the clock rate on DCE interfaces: `clock rate 64000`.
4. Enter router configuration mode on all routers:
   - Command: `router rip`
   - Specify version 2: `version 2`
   - Disable automatic summarization: `no auto-summary`
   - Advertise major network: `network 172.16.0.0`
5. On LAN interfaces (e.g., `GigabitEthernet0/0` on 2501B and 2501C), configure `passive-interface` to stop sending unnecessary RIP multicast traffic onto end-user subnets.
6. Allow approximately 30–60 seconds for RIP convergence.
7. Inspect routing protocols using `show ip protocols` and inspect routing tables using `show ip route`.
8. Verify end-to-end communication from PC-1 to PC-3 using `ping` and trace path using `tracert`.

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
R_2621(config)# router rip
R_2621(config-router)# version 2
R_2621(config-router)# no auto-summary
R_2621(config-router)# network 172.16.0.0
R_2621(config-router)# end
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
R_2501A(config)# router rip
R_2501A(config-router)# version 2
R_2501A(config-router)# no auto-summary
R_2501A(config-router)# network 172.16.0.0
R_2501A(config-router)# end
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
R_2501B(config)# router rip
R_2501B(config-router)# version 2
R_2501B(config-router)# no auto-summary
R_2501B(config-router)# network 172.16.0.0
R_2501B(config-router)# passive-interface GigabitEthernet0/0
R_2501B(config-router)# end
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
R_2501C(config)# router rip
R_2501C(config-router)# version 2
R_2501C(config-router)# no auto-summary
R_2501C(config-router)# network 172.16.0.0
R_2501C(config-router)# passive-interface GigabitEthernet0/0
R_2501C(config-router)# end
R_2501C# copy running-config startup-config
```

---

## 8. OBSERVATIONS & TEST OUTPUTS

### 8.1 Active Protocol Parameters (`show ip protocols`)
```text
R_2501B# show ip protocols

Routing Protocol is "rip"
  Sending updates every 30 seconds, next due in 12 seconds
  Invalid after 180 seconds, hold down 180, flushed after 240
  Default version control: send version 2, receive version 2
    Interface             Send  Recv  Triggered RIP  Key-chain
    Serial0/0/0           2     2                                    
    Serial0/0/1           2     2                                    
  Automatic network summarization is not in effect
  Routing for Networks:
    172.16.0.0
  Passive Interface(s):
    GigabitEthernet0/0
  Routing Information Sources:
    Gateway         Distance      Last Update
    172.16.20.1          120      00:00:14
    172.16.40.2          120      00:00:07
  Distance: (default is 120)
```

### 8.2 Converged Routing Table on Router 2501B (`show ip route`)
```text
R_2501B# show ip route

Codes: C - connected, S - static, R - RIP, M - mobile, B - BGP

Gateway of last resort is not set

     172.16.0.0/24 is subnetted, 5 subnets
R       172.16.10.0 [120/1] via 172.16.20.1, 00:00:14, Serial0/0/0
C       172.16.20.0 is directly connected, Serial0/0/0
C       172.16.30.0 is directly connected, GigabitEthernet0/0
C       172.16.40.0 is directly connected, Serial0/0/1
R       172.16.50.0 [120/1] via 172.16.40.2, 00:00:07, Serial0/0/1
```

### 8.3 Ping Test (PC-1 to PC-3)
```cmd
C:\> ping 172.16.50.10

Pinging 172.16.50.10 with 32 bytes of data:
Reply from 172.16.50.10: bytes=32 time=9ms TTL=125
Reply from 172.16.50.10: bytes=32 time=8ms TTL=125
Reply from 172.16.50.10: bytes=32 time=7ms TTL=125
Reply from 172.16.50.10: bytes=32 time=8ms TTL=125

Ping statistics for 172.16.50.10:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss)
```

---

## 9. VIVA VOCE QUESTIONS & ANSWERS

**Q1: Why is RIPv2 considered classless while RIPv1 is classful?**  
*Ans:* RIPv2 carries 32-bit subnet mask information along with route prefixes in its update packets, enabling support for Variable Length Subnet Masking (VLSM) and CIDR. RIPv1 carries only the IP address and relies on classful subnet masks.

**Q2: What is the significance of the `no auto-summary` command in RIPv2?**  
*Ans:* By default, RIP summarizes subnets to their major Class A/B/C network boundary when advertising across subnets. The `no auto-summary` command forces RIPv2 to advertise specific subnet masks, ensuring proper routing in discontiguous or subnetted networks.

**Q3: What multicast IP address does RIPv2 use, and why is multicast preferred over broadcast?**  
*Ans:* RIPv2 uses `224.0.0.9`. Multicasting is preferred because only hosts/routers listening for RIP updates process the packet at the network interface card level. Unrelated network devices discard the packet in hardware without interrupting CPU cycles.

**Q4: How does the Split Horizon rule prevent routing loops?**  
*Ans:* Split Horizon prevents a router from advertising a learned routing prefix back out of the same interface from which it originally received it, preventing two adjacent routers from bouncing packets back and forth.

**Q5: What is the purpose of configuring a `passive-interface` in RIP?**  
*Ans:* `passive-interface` stops the router from broadcasting/multicasting periodic RIP updates out onto that interface (e.g., towards end-user PCs), saving LAN bandwidth and enhancing security, while still advertising that network prefix to other routers.

---

## 10. RESULT
RIPv2 dynamic routing was successfully configured and verified on the multi-router network in Cisco Packet Tracer. All subnets dynamically converged with administrative distance 120, and complete end-to-end IP reachability was confirmed using `ping` and `tracert`.
