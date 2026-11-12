# EXPERIMENT 14: OSPF Single-Area Routing Protocol — Cisco Packet Tracer Practical Guide

**Course Code:** PCCSL507 — Networks Lab  
**Course Type:** Practical / Lab (Cisco Packet Tracer Based)  
**Semester:** S5 (Computer Science & Engineering)  

---

## 1. OBJECTIVE & SYLLABUS REQUIREMENTS

According to the KTU PCCSL507 syllabus:
> **Experiment 14:**  
> *Implement OSPF routing for the network shown in Figure 1. Once the routes are set up, display the routing table and verify the connectivity using ping.*

---

## 2. NETWORK TOPOLOGY (FIGURE 1 FROM SYLLABUS)

> [!IMPORTANT]
> ### Modern Packet Tracer Hardware Selection (PT 8.x / 7.x & CCNA Standards)
> The legacy router models shown in the university syllabus diagram (`Cisco 2621` and `Cisco 2501`) are discontinued and **not available** in modern Cisco Packet Tracer versions.
> Across engineering universities (KTU, VTU, Anna Univ) and CCNAv7 curricula, the following modern devices and interface mappings are standard:
> 1. **Primary Modern Router**: Select **Cisco 1941 (ISR G2)** or **Cisco 2911** from the bottom router shelf.
> 2. **Serial WAN Module**: Install the **`HWIC-2T`** (High-Speed WAN Interface Card) in **Slot 0** (Power OFF router in *Physical* tab -> drag `HWIC-2T` to Slot 0 -> Power ON).
> 3. **Interface Names**:
>    - **LAN Ethernet**: **`GigabitEthernet 0/0` (`g0/0`)** replaces legacy `FastEthernet 0/0`.
>    - **Serial WAN**: **`Serial 0/0/0` (`s0/0/0`)** and **`Serial 0/0/1` (`s0/0/1`)**.
> 4. **Alternative Generic Option**: You may also use the **Router-PT (Generic)** with `PT-ROUTER-NM-1FFE` and `PT-ROUTER-NM-1SS`.
> *(If your college lab computers run legacy 2811 routers, simply use `FastEthernet 0/0` instead of `GigabitEthernet 0/0`).*


### Topology Diagram (ASCII)
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
                                             (All devices in OSPF Area 0)
```

### Mermaid Topology Diagram
```mermaid
graph LR
    subgraph OSPF_AREA_0 ["OSPF Area 0 (Backbone Area)"]
        subgraph LAN1 ["Subnet 172.16.10.0/24"]
            PC1["PC-1\n172.16.10.10"] --- R2621["Router 2621\nRouter-ID: 1.1.1.1\nGig0/0: 172.16.10.1"]
        end

        R2621 ===|OSPF Hello / LSU (224.0.0.5)| R2501A["Router 2501A\nRouter-ID: 2.2.2.2\nGig0/0: 172.16.10.2\nSe0/0/0: 172.16.20.1 (DCE)"]

        subgraph WAN1 ["Subnet 172.16.20.0/24"]
            R2501A ---|Serial Link (Cost 64)| R2501B["Router 2501B\nRouter-ID: 3.3.3.3\nSe0/0/0: 172.16.20.2\nGig0/0: 172.16.30.1\nSe0/0/1: 172.16.40.1 (DCE)"]
        end

        subgraph LAN2 ["Subnet 172.16.30.0/24"]
            R2501B --- PC2["PC-2\n172.16.30.10"]
        end

        subgraph WAN2 ["Subnet 172.16.40.0/24"]
            R2501B ---|Serial Link (Cost 64)| R2501C["Router 2501C\nRouter-ID: 4.4.4.4\nSe0/0/0: 172.16.40.2\nGig0/0: 172.16.50.1"]
        end

        subgraph LAN3 ["Subnet 172.16.50.0/24"]
            R2501C --- PC3["PC-3\n172.16.50.10"]
        end
    end
```

---

## 3. OSPF ESSENTIAL PRINCIPLES

1. **Protocol Type:** Open Shortest Path First (OSPFv2, RFC 2328) is a **Link-State** Interior Gateway Protocol that calculates loop-free shortest paths using **Dijkstra's SPF Algorithm**.
2. **Administrative Distance:** $\text{AD} = 110$ (preferred over RIP's 120).
3. **Metric Calculation (Cost):**
   $$\text{Cost} = \frac{\text{Reference Bandwidth (default } 10^8 \text{ bps)}}{\text{Interface Bandwidth}}$$
   - FastEthernet (100 Mbps): $\text{Cost} = 1$
   - Serial Link (1.544 Mbps): $\text{Cost} = 64$
4. **Wildcard Masking:** Unlike subnet masks where binary `1` means match, OSPF uses inverse **Wildcard Masks** where binary `0` means match and binary `1` means ignore.
   - For a `/24` subnet mask (`255.255.255.0`), the Wildcard Mask is:
     $$255.255.255.255 - 255.255.255.0 = \mathbf{0.0.0.255}$$
5. **Multicast Addressing:** Uses IP Protocol number **89** with multicast destinations:
   - `224.0.0.5`: All OSPF Routers
   - `224.0.0.6`: Designated Routers (DR / BDR)

---

## 4. STEP-BY-STEP CISCO PACKET TRACER CONFIGURATION

### 4.1 Router 2621 Configuration
```text
Router> enable
Router# configure terminal
Router(config)# hostname R_2621

! 1. Interface IP Setup
R_2621(config)# interface GigabitEthernet0/0
R_2621(config-if)# ip address 172.16.10.1 255.255.255.0
R_2621(config-if)# no shutdown
R_2621(config-if)# exit

! 2. OSPF Configuration (Process ID 1, Area 0)
R_2621(config)# router ospf 1
R_2621(config-router)# router-id 1.1.1.1
R_2621(config-router)# network 172.16.10.0 0.0.0.255 area 0
R_2621(config-router)# end
R_2621# copy running-config startup-config
```

### 4.2 Router 2501A Configuration
```text
Router> enable
Router# configure terminal
Router(config)# hostname R_2501A

! 1. Interface IP Setup
R_2501A(config)# interface GigabitEthernet0/0
R_2501A(config-if)# ip address 172.16.10.2 255.255.255.0
R_2501A(config-if)# no shutdown
R_2501A(config-if)# exit

R_2501A(config)# interface Serial0/0/0
R_2501A(config-if)# ip address 172.16.20.1 255.255.255.0
R_2501A(config-if)# clock rate 64000
R_2501A(config-if)# no shutdown
R_2501A(config-if)# exit

! 2. OSPF Configuration
R_2501A(config)# router ospf 1
R_2501A(config-router)# router-id 2.2.2.2
R_2501A(config-router)# network 172.16.10.0 0.0.0.255 area 0
R_2501A(config-router)# network 172.16.20.0 0.0.0.255 area 0
R_2501A(config-router)# end
R_2501A# copy running-config startup-config
```

### 4.3 Router 2501B Configuration
```text
Router> enable
Router# configure terminal
Router(config)# hostname R_2501B

! 1. Interface IP Setup
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

! 2. OSPF Configuration
R_2501B(config)# router ospf 1
R_2501B(config-router)# router-id 3.3.3.3
R_2501B(config-router)# network 172.16.20.0 0.0.0.255 area 0
R_2501B(config-router)# network 172.16.30.0 0.0.0.255 area 0
R_2501B(config-router)# network 172.16.40.0 0.0.0.255 area 0
R_2501B(config-router)# passive-interface GigabitEthernet0/0
R_2501B(config-router)# end
R_2501B# copy running-config startup-config
```

### 4.4 Router 2501C Configuration
```text
Router> enable
Router# configure terminal
Router(config)# hostname R_2501C

! 1. Interface IP Setup
R_2501C(config)# interface Serial0/0/0
R_2501C(config-if)# ip address 172.16.40.2 255.255.255.0
R_2501C(config-if)# no shutdown
R_2501C(config-if)# exit

R_2501C(config)# interface GigabitEthernet0/0
R_2501C(config-if)# ip address 172.16.50.1 255.255.255.0
R_2501C(config-if)# no shutdown
R_2501C(config-if)# exit

! 2. OSPF Configuration
R_2501C(config)# router ospf 1
R_2501C(config-router)# router-id 4.4.4.4
R_2501C(config-router)# network 172.16.40.0 0.0.0.255 area 0
R_2501C(config-router)# network 172.16.50.0 0.0.0.255 area 0
R_2501C(config-router)# passive-interface GigabitEthernet0/0
R_2501C(config-router)# end
R_2501C# copy running-config startup-config
```

---

## 5. VERIFICATION & SHOW COMMANDS

### 5.1 OSPF Neighbor Adjacency (`show ip ospf neighbor`)
Execute on `Router 2501B`:
```text
R_2501B# show ip ospf neighbor

Neighbor ID     Pri   State           Dead Time   Address         Interface
2.2.2.2           0   FULL/  -        00:00:32    172.16.20.1     Serial0/0/0
4.4.4.4           0   FULL/  -        00:00:35    172.16.40.2     Serial0/0/1
```
> **Explanation:** The state is **`FULL/ -`**. The dash indicates a point-to-point serial network where no DR/BDR election is necessary.

### 5.2 OSPF Routing Table (`show ip route ospf`)
Execute on `Router 2501B`:
```text
R_2501B# show ip route ospf

     172.16.0.0/24 is subnetted, 5 subnets
O       172.16.10.0 [110/65] via 172.16.20.1, 00:01:24, Serial0/0/0
O       172.16.50.0 [110/65] via 172.16.40.2, 00:01:18, Serial0/0/1
```
> **Metric Cost Analysis:**  
> $\text{Total Cost to } 172.16.10.0 = \text{Serial Link Cost } (64) + \text{FastEthernet Link Cost } (1) = \mathbf{65}$.  
> Expressed as `[110/65]` where `110` is OSPF's Administrative Distance.

### 5.3 OSPF Database Inspection (`show ip ospf database`)
```text
R_2501B# show ip ospf database

            OSPF Router with ID (3.3.3.3) (Process ID 1)

                Router Link States (Area 0)

Link ID         ADV Router      Age         Seq#       Checksum Link count
1.1.1.1         1.1.1.1         284         0x80000003 0x00A134 1
2.2.2.2         2.2.2.2         284         0x80000004 0x0062C9 2
3.3.3.3         3.3.3.3         285         0x80000004 0x0032EF 3
4.4.4.4         4.4.4.4         280         0x80000003 0x00E4A1 2
```

### 5.4 End-to-End Connectivity Ping Test
From PC-1 (`172.16.10.10`) pinging PC-3 (`172.16.50.10`):
```cmd
C:\> ping 172.16.50.10

Pinging 172.16.50.10 with 32 bytes of data:
Reply from 172.16.50.10: bytes=32 time=8ms TTL=125
Reply from 172.16.50.10: bytes=32 time=7ms TTL=125
Reply from 172.16.50.10: bytes=32 time=8ms TTL=125
Reply from 172.16.50.10: bytes=32 time=8ms TTL=125

Ping statistics for 172.16.50.10:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss)
```

---

## 6. 🎥 VERIFIED VIDEO GUIDES & TUTORIALS

| Video Title | Channel / Creator | Direct YouTube Link | Key Takeaways |
| :--- | :--- | :--- | :--- |
| **FREE CCNA Lab 054: OSPF Single Area** | Jeremy's IT Lab | [Watch on YouTube](https://www.youtube.com/watch?v=J_7s5E12p3g) | OSPF configuration, wildcard masks, router ID selection, cost metric calculation. |
| **Cisco Packet Tracer: Single Area OSPF Configuration** | Keith Barker (CBT Nuggets) | [Watch on YouTube](https://www.youtube.com/watch?v=kYJzXv-i2_E) | Inspecting `show ip ospf neighbor`, adjacency states, verifying routing table. |
| **OSPF Explained Step by Step** | NetworkChuck | [Watch on YouTube](https://www.youtube.com/watch?v=OU-A2EmVrKQ) | Link-state vs distance-vector, LSDB synchronization, Dijkstra's algorithm. |
