# EXPERIMENT 14: IMPLEMENTATION OF OSPF DYNAMIC ROUTING

**Course Code:** PCCSL507 — Networks Lab  
**Course Name:** Computer Science & Engineering Networks Laboratory  
**Semester:** S5  

---

## 1. AIM
To implement single-area Open Shortest Path First (OSPFv2) dynamic routing across the multi-router topology shown in Figure 1 of the syllabus using Cisco Packet Tracer, verify link-state database synchronization and neighbor adjacencies, and confirm end-to-end IP connectivity using `ping` and `traceroute`.

---

## 2. HARDWARE AND SOFTWARE REQUIREMENTS
1. **Simulation Environment:** Cisco Packet Tracer (v8.x or above)
2. **Devices Simulated:**
   - 4 × Cisco 2811 series Routers equipped with WIC-2T modules
   - 3 × Generic Workstation PCs (PC-1, PC-2, PC-3)
   - 1 × Catalyst 2960 Switch
   - Cabling: Serial DCE/DTE cables, Copper Straight-Through cables
3. **Host Computer:** PC running Windows / Linux

---

## 3. THEORY & PRINCIPLES

### 3.1 Overview of OSPF
Open Shortest Path First (OSPF) is an open-standard, link-state Interior Gateway Protocol (IGP) defined in **RFC 2328**. Unlike distance-vector protocols that share their entire routing table, OSPF routers flood **Link-State Advertisements (LSAs)** describing their local interfaces and link states to build an identical **Link-State Database (LSDB)** across all routers in an area.

Each router then executes **Dijkstra's Shortest Path First (SPF) algorithm** using itself as the root to construct a loop-free Shortest Path Tree (SPT) and populate the IP routing table.

### 3.2 OSPF Key Features
1. **Administrative Distance:** $\text{AD} = 110$.
2. **Metric (Cost):** Inversely proportional to bandwidth:
   $$\text{Cost} = \frac{\text{Reference Bandwidth}}{\text{Interface Bandwidth}} = \frac{10^8 \text{ bps}}{\text{Interface Bandwidth (bps)}}$$
   - FastEthernet (100 Mbps): $\text{Cost} = \frac{100,000,000}{100,000,000} = 1$
   - Serial Link (1.544 Mbps): $\text{Cost} = \frac{100,000,000}{1,544,000} \approx 64$
3. **Areas:** Organizes large networks hierarchically. All single-area deployments operate in **Area 0** (the Backbone Area).
4. **Transport:** Encapsulated directly within IP packets (IP Protocol number **89**).
5. **Multicast Addressing:**
   - `224.0.0.5`: AllSPFRouters
   - `224.0.0.6`: AllDRouters (Designated Routers)
6. **Router ID (RID):** A 32-bit identifier selected as:
   - Manually configured `router-id` (highest priority).
   - Highest IP address on an active loopback interface.
   - Highest IP address on an active physical interface.

---

## 4. NETWORK TOPOLOGY DIAGRAM

```text
  [172.16.10.0/24]                [172.16.20.0/24]                 [172.16.40.0/24]              [172.16.50.0/24]
       Ethernet                      Serial WAN                       Serial WAN                      Ethernet
 [Router 2621] ============ [Router 2501A] ----------------- [Router 2501B] ----------------- [Router 2501C] === [PC-3]
   Fa0/0: .1                 Fa0/0: .2  Se0/0/0: .1      Se0/0/0: .2  Se0/0/1: .1     Se0/0/0: .2  Fa0/0: .1    .10
       |                                                    |                                           
     [PC-1]                                               Fa0/0: .1                                     
      .10                                                   |                                           
                                                          [PC-2] .10                                    
                                                     [172.16.30.0/24] LAN                               
                                             (All devices in OSPF Area 0)
```

---

## 5. IP ADDRESSING & WILDCARD MASK TABLE

| Device | Interface | IP Address | Subnet Mask | Wildcard Mask | OSPF Area |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Router 2621** | `Fa0/0` | `172.16.10.1` | `255.255.255.0` | `0.0.0.255` | Area 0 |
| **Router 2501A**| `Fa0/0` | `172.16.10.2` | `255.255.255.0` | `0.0.0.255` | Area 0 |
| | `Se0/0/0` | `172.16.20.1` | `255.255.255.0` | `0.0.0.255` | Area 0 |
| **Router 2501B**| `Se0/0/0` | `172.16.20.2` | `255.255.255.0` | `0.0.0.255` | Area 0 |
| | `Fa0/0` | `172.16.30.1` | `255.255.255.0` | `0.0.0.255` | Area 0 |
| | `Se0/0/1` | `172.16.40.1` | `255.255.255.0` | `0.0.0.255` | Area 0 |
| **Router 2501C**| `Se0/0/0` | `172.16.40.2` | `255.255.255.0` | `0.0.0.255` | Area 0 |
| | `Fa0/0` | `172.16.50.1` | `255.255.255.0` | `0.0.0.255` | Area 0 |
| **PC-1** | `Fa0` | `172.16.10.10` | `255.255.255.0` | N/A | GW: 172.16.10.1 |
| **PC-2** | `Fa0` | `172.16.30.10` | `255.255.255.0` | N/A | GW: 172.16.30.1 |
| **PC-3** | `Fa0` | `172.16.50.10` | `255.255.255.0` | N/A | GW: 172.16.50.1 |

---

## 6. PROCEDURE / ALGORITHM

1. Replicate the Figure 1 topology in Cisco Packet Tracer with 4 routers, 1 switch, and 3 PCs.
2. Configure IP addresses on all physical interfaces and assign clock rates to DCE serial ports (`clock rate 64000`).
3. Enable OSPF routing process globally on each router using `router ospf 1`.
4. Assign explicit router IDs:
   - Router 2621: `1.1.1.1`
   - Router 2501A: `2.2.2.2`
   - Router 2501B: `3.3.3.3`
   - Router 2501C: `4.4.4.4`
5. Enable OSPF on all interfaces using the `network <subnet> <wildcard-mask> area 0` command.
6. Configure `passive-interface` on host-facing LAN interfaces to eliminate hello packet overhead on end-user links.
7. Verify neighbor adjacency states using `show ip ospf neighbor` (confirming `FULL` state).
8. Inspect learned OSPF routes using `show ip route ospf` and confirm metric cost calculation.
9. Verify bidirectional packet delivery between PC-1 and PC-3 using `ping` and trace route using `tracert`.

---

## 7. CISCO IOS ROUTER CONFIGURATIONS

### 7.1 Router 2621
```text
Router> enable
Router# configure terminal
Router(config)# hostname R_2621
R_2621(config)# interface FastEthernet0/0
R_2621(config-if)# ip address 172.16.10.1 255.255.255.0
R_2621(config-if)# no shutdown
R_2621(config-if)# exit
R_2621(config)# router ospf 1
R_2621(config-router)# router-id 1.1.1.1
R_2621(config-router)# network 172.16.10.0 0.0.0.255 area 0
R_2621(config-router)# end
R_2621# copy running-config startup-config
```

### 7.2 Router 2501A
```text
Router> enable
Router# configure terminal
Router(config)# hostname R_2501A
R_2501A(config)# interface FastEthernet0/0
R_2501A(config-if)# ip address 172.16.10.2 255.255.255.0
R_2501A(config-if)# no shutdown
R_2501A(config-if)# exit
R_2501A(config)# interface Serial0/0/0
R_2501A(config-if)# ip address 172.16.20.1 255.255.255.0
R_2501A(config-if)# clock rate 64000
R_2501A(config-if)# no shutdown
R_2501A(config-if)# exit
R_2501A(config)# router ospf 1
R_2501A(config-router)# router-id 2.2.2.2
R_2501A(config-router)# network 172.16.10.0 0.0.0.255 area 0
R_2501A(config-router)# network 172.16.20.0 0.0.0.255 area 0
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
R_2501B(config)# interface FastEthernet0/0
R_2501B(config-if)# ip address 172.16.30.1 255.255.255.0
R_2501B(config-if)# no shutdown
R_2501B(config-if)# exit
R_2501B(config)# interface Serial0/0/1
R_2501B(config-if)# ip address 172.16.40.1 255.255.255.0
R_2501B(config-if)# clock rate 64000
R_2501B(config-if)# no shutdown
R_2501B(config-if)# exit
R_2501B(config)# router ospf 1
R_2501B(config-router)# router-id 3.3.3.3
R_2501B(config-router)# network 172.16.20.0 0.0.0.255 area 0
R_2501B(config-router)# network 172.16.30.0 0.0.0.255 area 0
R_2501B(config-router)# network 172.16.40.0 0.0.0.255 area 0
R_2501B(config-router)# passive-interface FastEthernet0/0
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
R_2501C(config)# interface FastEthernet0/0
R_2501C(config-if)# ip address 172.16.50.1 255.255.255.0
R_2501C(config-if)# no shutdown
R_2501C(config-if)# exit
R_2501C(config)# router ospf 1
R_2501C(config-router)# router-id 4.4.4.4
R_2501C(config-router)# network 172.16.40.0 0.0.0.255 area 0
R_2501C(config-router)# network 172.16.50.0 0.0.0.255 area 0
R_2501C(config-router)# passive-interface FastEthernet0/0
R_2501C(config-router)# end
R_2501C# copy running-config startup-config
```

---

## 8. OBSERVATIONS & TEST OUTPUTS

### 8.1 Neighbor Adjacency Table on Router 2501B (`show ip ospf neighbor`)
```text
R_2501B# show ip ospf neighbor

Neighbor ID     Pri   State           Dead Time   Address         Interface
2.2.2.2           0   FULL/  -        00:00:34    172.16.20.1     Serial0/0/0
4.4.4.4           0   FULL/  -        00:00:36    172.16.40.2     Serial0/0/1
```
*Note:* Adjacency state has reached `FULL`, confirming complete LSDB synchronization between neighbors.

### 8.2 OSPF Learned Routes on Router 2501B (`show ip route ospf`)
```text
R_2501B# show ip route ospf

     172.16.0.0/24 is subnetted, 5 subnets
O       172.16.10.0 [110/65] via 172.16.20.1, 00:01:42, Serial0/0/0
O       172.16.50.0 [110/65] via 172.16.40.2, 00:01:38, Serial0/0/1
```
*Inference:*  
The route cost calculation is verified as:
$$\text{Cost to } 172.16.10.0 = \text{Serial Link } (64) + \text{FastEthernet Link } (1) = 65$$

### 8.3 Ping Test (PC-1 to PC-3)
```cmd
C:\> ping 172.16.50.10

Pinging 172.16.50.10 with 32 bytes of data:
Reply from 172.16.50.10: bytes=32 time=8ms TTL=125
Reply from 172.16.50.10: bytes=32 time=8ms TTL=125
Reply from 172.16.50.10: bytes=32 time=7ms TTL=125
Reply from 172.16.50.10: bytes=32 time=8ms TTL=125

Ping statistics for 172.16.50.10:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss)
```

---

## 9. VIVA VOCE QUESTIONS & ANSWERS

**Q1: How does OSPF differ from distance-vector routing protocols like RIP?**  
*Ans:* Distance-vector protocols periodically send their full routing table to direct neighbors ("routing by rumor"). In contrast, OSPF routers only flood Link-State Advertisements (LSAs) when topology changes occur, building a synchronized link-state database (LSDB) and calculating best paths locally using Dijkstra's algorithm.

**Q2: What is a Wildcard Mask and how is it calculated?**  
*Ans:* A wildcard mask tells the router which bits of an IP address to match (`0`) and which bits to ignore (`1`). It is calculated by subtracting the subnet mask from `255.255.255.255` (e.g., $255.255.255.255 - 255.255.255.0 = 0.0.0.255$).

**Q3: Why is there no Designated Router (DR) elected on serial links?**  
*Ans:* Serial links operate as **Point-to-Point** network types connecting exactly two endpoints. Because there are no multi-access broadcasts or complex $n(n-1)/2$ adjacency relationships, DR and BDR elections are unnecessary.

**Q4: What are the Hello and Dead intervals for OSPF by default?**  
*Ans:* On Broadcast and Point-to-Point networks, the default Hello timer is **10 seconds**, and the Dead timer is 4 times the Hello interval (**40 seconds**). Both routers must have matching timers to form an adjacency.

**Q5: What is the significance of Area 0 in OSPF?**  
*Ans:* Area 0 is the **Backbone Area**. In multi-area OSPF networks, all non-backbone areas must physically or virtually connect to Area 0 to route traffic between areas, preventing inter-area routing loops.

---

## 10. RESULT
Single-area OSPFv2 dynamic routing was successfully implemented on the multi-router network in Cisco Packet Tracer. All routers established full neighbor adjacencies, SPF cost metrics were verified, and end-to-end IP connectivity was validated using `ping` and `traceroute`.
