# EXPERIMENT 16: SELECTIVE SERVICE FILTERING USING EXTENDED ACCESS LISTS (ACL)

**Course Code:** PCCSL507 — Networks Lab  
**Course Name:** Computer Science & Engineering Networks Laboratory  
**Semester:** S5  

---

## 1. AIM
To design and implement an IP addressing and subnetting scheme for a Class B campus network (`140.80.0.0`) partitioned into at least 20 subnets, identify the Central Computing Facility (CCF) in the 4th subnet and the Hackathon registration server in the 16th subnet, configure an Extended Access Control List (ACL) to block web traffic (HTTP port 80 / HTTPS port 443) from CCF to the server while permitting all other network services (ICMP ping, FTP, DNS), and verify enforcement using Cisco Packet Tracer.

---

## 2. HARDWARE AND SOFTWARE REQUIREMENTS
1. **Simulation Software:** Cisco Packet Tracer (v8.x or above)
2. **Devices Simulated:**
   - 2 × Cisco 2811 Routers with HWIC-2T serial modules
   - 2 × Cisco Catalyst 2960 Switches
   - 1 × CCF Student Workstation (PC-PT)
   - 1 × Hackathon Server (Server-PT running HTTP, FTP, and ICMP services)
   - Cabling: Copper Straight-Through cables, Serial DCE/DTE cables
3. **Host Computer:** PC running Windows / Linux

---

## 3. THEORY & PRINCIPLES

### 3.1 Subnetting Mathematical Analysis
The college is allocated the Class B network address `140.80.0.0` (default mask `/16`, `255.255.0.0`).  
To satisfy the requirement of dividing the address space into at least 20 subnets:
$$\text{Number of subnets} = 2^n \ge 20 \implies n = 5 \text{ bits borrowed}$$

The new subnet mask is:
$$\text{Prefix length} = 16 + 5 = \mathbf{/21} \quad (255.255.248.0)$$

The block size (step value) in the third octet:
$$\text{Block size} = 256 - 248 = \mathbf{8}$$

Subnet allocations:
- Subnet 0: `140.80.0.0/21`
- Subnet 1: `140.80.8.0/21`
- Subnet 2: `140.80.16.0/21`
- Subnet 3: `140.80.24.0/21`
- **Subnet 4 (CCF Subnet):** $\mathbf{140.80.32.0/21}$
  - Host address range: `140.80.32.1` to `140.80.39.254`
  - Gateway IP: `140.80.32.1`
  - CCF Client PC: `140.80.32.10`
  - Subnet Wildcard Mask: $255.255.255.255 - 255.255.248.0 = \mathbf{0.0.7.255}$
- **Subnet 16 (Hackathon Server Subnet):**
  $$\text{3rd octet start} = 16 \times 8 = 128 \implies \mathbf{140.80.128.0/21}$$
  - 1st usable address (Gateway): `140.80.128.1`
  - **7th address in 16th subnet:** $\mathbf{140.80.128.7}$ (Server IP)

### 3.2 Extended Access Control Lists (ACLs)
Extended ACLs (numbered 100–199 and 2000–2699) allow fine-grained packet inspection based on:
1. Source IP address and wildcard mask
2. Destination IP address and wildcard mask
3. Transport protocol (TCP, UDP, ICMP, IP)
4. Application port numbers (e.g., `eq 80` for HTTP, `eq 443` for HTTPS, `eq 21` for FTP)

### 3.3 Extended ACL Placement Rule
Unlike Standard ACLs which are placed near the destination, **Extended ACLs must be placed as close to the source as possible** (inbound on Router-CCF's `GigabitEthernet0/0`). This discards prohibited traffic before it traverses WAN serial links, conserving network bandwidth and reducing transit router processing overhead.

---

## 4. NETWORK TOPOLOGY DIAGRAM

```text
   [CCF Subnet 4: 140.80.32.0/21]                             [CSE Server Farm Subnet 16: 140.80.128.0/21]
         [ CCF Student PC ]                                                [ Hackathon Server ]
           140.80.32.10                                                       140.80.128.7
                |                                                                  |
           [Switch CCF]                                                       [Switch CSE]
                |                                                                  |
            Gig0/0: .1                                                          Gig0/0: .1
       [ Router-CCF (R1) ] ============================================== [ Router-CSE (R2) ]
        Se0/0/0: 10.0.0.1                  WAN Serial Link                 Se0/0/0: 10.0.0.2
       [ APPLY EXTENDED ACL 101 ]           (10.0.0.0/30)
          (Inbound on Gig0/0)
```

---

## 5. IP ADDRESSING & PORT TABLE

| Node | Interface | IP Address | Subnet Mask | Default Gateway | Service / Role |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **CCF PC** | `Fa0` | `140.80.32.10` | `255.255.248.0` | `140.80.32.1` | CCF Student Workstation |
| **Hackathon Server**| `Fa0` | `140.80.128.7` | `255.255.248.0` | `140.80.128.1` | HTTP (80), FTP (21), ICMP |
| **Router-CCF** | `Gig0/0` | `140.80.32.1` | `255.255.248.0` | N/A | Gateway (ACL 101 Inbound) |
| | `Se0/0/0` | `10.0.0.1` | `255.255.255.252`| N/A | WAN Serial DCE (Clock 64000) |
| **Router-CSE** | `Se0/0/0` | `10.0.0.2` | `255.255.255.252`| N/A | WAN Serial DTE |
| | `Gig0/0` | `140.80.128.1` | `255.255.248.0` | N/A | CSE Gateway |

---

## 6. PROCEDURE / ALGORITHM

1. Construct the 2-router campus backbone connecting CCF to the CSE department in Cisco Packet Tracer.
2. Configure IP parameters on all interfaces according to the `/21` addressing plan.
3. Establish dynamic routing (OSPF single area) between routers to provide baseline inter-subnet reachability.
4. On Hackathon Server, enable HTTP, HTTPS, and FTP services.
5. Create Extended ACL 101 on Router-CCF:
   - Statement 1: `access-list 101 deny tcp 140.80.32.0 0.0.7.255 host 140.80.128.7 eq 80`
   - Statement 2: `access-list 101 deny tcp 140.80.32.0 0.0.7.255 host 140.80.128.7 eq 443`
   - Statement 3: `access-list 101 permit ip any any`
6. Apply ACL 101 inbound on Router-CCF's CCF-facing interface:
   - `interface GigabitEthernet0/0`
   - `ip access-group 101 in`
7. Perform the verification checks:
   - Web browser test to `http://140.80.128.7` from CCF Student PC: Must timeout / fail.
   - ICMP ping test to `140.80.128.7` from CCF Student PC: Must succeed.
   - FTP test to `140.80.128.7` from CCF Student PC: Must connect successfully.
8. Inspect match counters on Router-CCF using `show access-lists 101`.

---

## 7. CISCO IOS ROUTER CONFIGURATIONS

### 7.1 Router-CCF Configuration (Including Extended ACL 101)
```text
Router> enable
Router# configure terminal
Router(config)# hostname Router-CCF

! Interface IP Setup
Router-CCF(config)# interface GigabitEthernet0/0
Router-CCF(config-if)# ip address 140.80.32.1 255.255.248.0
Router-CCF(config-if)# no shutdown
Router-CCF(config-if)# exit

Router-CCF(config)# interface Serial0/0/0
Router-CCF(config-if)# ip address 10.0.0.1 255.255.255.252
Router-CCF(config-if)# clock rate 64000
Router-CCF(config-if)# no shutdown
Router-CCF(config-if)# exit

! Dynamic Routing (OSPF)
Router-CCF(config)# router ospf 1
Router-CCF(config-router)# network 140.80.32.0 0.0.7.255 area 0
Router-CCF(config-router)# network 10.0.0.0 0.0.0.3 area 0
Router-CCF(config-router)# exit

! Extended Access List 101 Configuration
Router-CCF(config)# access-list 101 remark Block Hackathon website from CCF students
Router-CCF(config)# access-list 101 deny tcp 140.80.32.0 0.0.7.255 host 140.80.128.7 eq 80
Router-CCF(config)# access-list 101 deny tcp 140.80.32.0 0.0.7.255 host 140.80.128.7 eq 443
Router-CCF(config)# access-list 101 permit ip any any

! Apply ACL 101 Inbound on CCF LAN Interface
Router-CCF(config)# interface GigabitEthernet0/0
Router-CCF(config-if)# ip access-group 101 in
Router-CCF(config-if)# end
Router-CCF# copy running-config startup-config
```

### 7.2 Router-CSE Configuration
```text
Router> enable
Router# configure terminal
Router(config)# hostname Router-CSE

Router-CSE(config)# interface Serial0/0/0
Router-CSE(config-if)# ip address 10.0.0.2 255.255.255.252
Router-CSE(config-if)# no shutdown
Router-CSE(config-if)# exit

Router-CSE(config)# interface GigabitEthernet0/0
Router-CSE(config-if)# ip address 140.80.128.1 255.255.248.0
Router-CSE(config-if)# no shutdown
Router-CSE(config-if)# exit

Router-CSE(config)# router ospf 1
Router-CSE(config-router)# network 140.80.128.0 0.0.7.255 area 0
Router-CSE(config-router)# network 10.0.0.0 0.0.0.3 area 0
Router-CSE(config-router)# end
Router-CSE# copy running-config startup-config
```

---

## 8. OBSERVATIONS & TEST OUTPUTS

### 8.1 Verification 1: Web Browser Access to Hackathon Website (BLOCKED)
- **URL Requested:** `http://140.80.128.7`
- **Observed Result:** `Request Timeout`
- **Inference:** Layer 4 TCP port 80 SYN segments are matched against rule 1 of ACL 101 and dropped by the router.

### 8.2 Verification 2: Diagnostic ICMP Ping Test (ALLOWED)
```cmd
C:\> ping 140.80.128.7

Pinging 140.80.128.7 with 32 bytes of data:
Reply from 140.80.128.7: bytes=32 time=4ms TTL=126
Reply from 140.80.128.7: bytes=32 time=4ms TTL=126
Reply from 140.80.128.7: bytes=32 time=4ms TTL=126
Reply from 140.80.128.7: bytes=32 time=5ms TTL=126

Ping statistics for 140.80.128.7:
    Packets: Sent = 4, Received = 4, Lost = 0 (0% loss)
```
- **Inference:** ICMP echo requests match statement 3 (`permit ip any any`) and pass transparently.

### 8.3 Verification 3: FTP Service Access Test (ALLOWED)
```cmd
C:\> ftp 140.80.128.7
Connected to 140.80.128.7.
220 Service ready for new user.
User (140.80.128.7:(none)): cisco
331 User name okay, need password.
Password: cisco
230 User logged in, proceed.
ftp> quit
221 Goodbye.
```
- **Inference:** Other network services provided by the server remain accessible without disruption.

### 8.4 Inspection of Extended ACL Counters on Router-CCF
```text
Router-CCF# show access-lists 101

Extended IP access list 101
    10 deny tcp 140.80.32.0 0.0.7.255 host 140.80.128.7 eq www (6 matches)
    20 deny tcp 140.80.32.0 0.0.7.255 host 140.80.128.7 eq 443
    30 permit ip any any (28 matches)
```

---

## 9. VIVA VOCE QUESTIONS & ANSWERS

**Q1: How do Extended ACLs differ from Standard ACLs in terms of filtering capabilities?**  
*Ans:* Standard ACLs filter solely based on source IP address. Extended ACLs can evaluate source and destination IP addresses, protocol suite (TCP, UDP, ICMP), and Layer 4 port numbers, enabling application-specific selective filtering.

**Q2: Why must Extended ACLs be positioned close to the traffic source?**  
*Ans:* Placing extended ACLs close to the source discards unauthorized packets at the first router ingress interface, preventing denied traffic from consuming bandwidth across WAN serial links or intermediate routers.

**Q3: How is the wildcard mask `0.0.7.255` derived for a `/21` subnet mask?**  
*Ans:* A `/21` subnet mask in dotted-decimal is `255.255.248.0`. Subtracting each octet from `255.255.255.255`:
$$(255-255).(255-255).(255-248).(255-0) = \mathbf{0.0.7.255}$$

**Q4: What is the significance of the `eq www` argument in Cisco ACLs?**  
*Ans:* `eq www` is an alias for `eq 80`, specifying TCP destination port 80 used by the Hypertext Transfer Protocol (HTTP).

**Q5: What would occur if the statement `access-list 101 permit ip any any` were omitted?**  
*Ans:* The default implicit `deny any` at the end of the ACL would block all non-web traffic as well, completely isolating CCF students from pinging, transferring files, or accessing any other campus services.

---

## 10. RESULT
The campus network was successfully partitioned into `/21` subnets with CCF at `140.80.32.0/21` and the hackathon server at `140.80.128.7`. Extended Access List 101 was configured on Router-CCF, successfully blocking website access (HTTP port 80) while allowing ICMP ping and FTP file services as verified in Cisco Packet Tracer.
