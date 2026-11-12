# EXPERIMENT 11: FAMILIARIZING ROUTER COMMANDS

**Course Code:** PCCSL507 — Networks Lab  
**Course Name:** Computer Science & Engineering Networks Laboratory  
**Semester:** S5  

---

## 1. AIM
To familiarize essential Cisco IOS router commands using Cisco Packet Tracer, including navigating CLI command modes, inspecting system information and hardware statistics, managing configurations, checking routing tables, and configuring Serial and FastEthernet interfaces with IP addresses and clock rates.

---

## 2. HARDWARE AND SOFTWARE REQUIREMENTS
1. **Software:** Cisco Packet Tracer (v7.x / v8.x or above)
2. **Devices Simulated:**
   - 2 × Cisco Routers (2811 / 1941 series) equipped with WIC-2T Serial WAN modules
   - 1 × PC (Workstation)
   - Cables: Copper Straight-Through cable, Serial DCE-to-DTE cable
3. **Host System:** Personal Computer running Windows / Linux with Packet Tracer installed

---

## 3. THEORY & PRINCIPLES

### 3.1 Cisco IOS Command Modes
Cisco Internetwork Operating System (IOS) organizes command execution into hierarchical modes:
1. **User EXEC Mode (`Router>`):**
   - Default entry level after console connection.
   - Allows basic non-destructive operational checks (`ping`, `traceroute`, `show clock`).
   - Cannot view sensitive configurations or alter router states.
2. **Privileged EXEC Mode (`Router#`):**
   - Accessed via the `enable` command (usually password-protected via `enable secret`).
   - Allows full administrative visibility (`show running-config`, `show ip route`), debugging, reloading, and file manipulation.
3. **Global Configuration Mode (`Router(config)#`):**
   - Accessed via `configure terminal` from Privileged EXEC mode.
   - Affects system-wide operation (hostname, banners, routing protocols, static routes).
4. **Specific Configuration Sub-Modes:**
   - Interface Mode (`Router(config-if)#`): Configures ports (`ip address`, `clock rate`, `no shutdown`).
   - Router Mode (`Router(config-router)#`): Configures routing protocols (RIP, OSPF, EIGRP).
   - Line Mode (`Router(config-line)#`): Configures access lines (Console, Aux, VTY).

### 3.2 Router Memory Architecture
- **ROM (Read-Only Memory):** Contains the bootstrap program, POST diagnostics, and mini-IOS (ROMmon).
- **Flash Memory:** Non-volatile electrically erasable memory holding the full Cisco IOS image file (`.bin`).
- **RAM (Random Access Memory):** Volatile workspace holding running configuration (`running-config`), routing tables, ARP cache, and packet buffers.
- **NVRAM (Non-Volatile RAM):** Retains configuration across reboots; stores `startup-config` and configuration register settings.

### 3.3 WAN Serial Connections: DCE vs DTE
- In real-world WAN circuits, service providers install a Channel Service Unit/Data Service Unit (CSU/DSU), which provides clock synchronization (acting as **DCE**). The customer router connects as **DTE**.
- In back-to-back lab environments, one router's serial interface must simulate the CSU/DSU clock source. The router connected with the DCE cable must be explicitly configured with the `clock rate` command (e.g., `clock rate 64000` bps). The opposite router (DTE) synchronizes its transmission clock to this incoming line signal automatically.

---

## 4. NETWORK TOPOLOGY DIAGRAM

```text
       +-----------------------+                         +-----------------------+
       |       ROUTER-1        |      Serial Link        |       ROUTER-2        |
       |  (Hostname: Router1)  |      172.16.20.0/24     |  (Hostname: Router2)  |
       |  Se0/0/0: 172.16.20.1 |=========================|  Se0/0/0: 172.16.20.2 |
       |     [DCE - 64000]     |       Serial DCE/DTE    |         [DTE]         |
       +-----------+-----------+                         +-----------------------+
                   | Fa0/0: 172.16.10.1/24
                   | (Copper Straight-Through)
                   |
       +-----------+-----------+
       |         PC-A          |
       |     172.16.10.10/24   |
       |   GW: 172.16.10.1     |
       +-----------------------+
```

---

## 5. PROCEDURE / ALGORITHM

1. **Topology Creation:**
   - Place two 2811 routers and one generic PC into the workspace.
   - Power down each router in the **Physical** tab, insert a `WIC-2T` module in `Slot 0`, and power on.
   - Connect PC-A to Router-1 using a Copper Straight-Through cable.
   - Connect Router-1 to Router-2 using a Serial DCE cable (Router-1 gets the DCE end).
2. **Mode Switching:**
   - Test mode transitions using `enable`, `disable`, and `configure terminal`.
3. **System Inspection:**
   - Run `show version` to inspect IOS build, CPU, DRAM, Flash, and hardware IDs.
   - Run `show ip protocols` and `show ip route`.
4. **Configuration Management & Environment:**
   - Set the software clock using `clock set`.
   - Add static host mapping and view using `show hosts`.
   - Inspect command history using `show history` and alter buffer size with `terminal history size`.
5. **Interface Configuration:**
   - On Router-1, assign IP `172.16.10.1/24` to `Fa0/0` and bring up interface with `no shutdown`.
   - On Router-1, assign IP `172.16.20.1/24` to `Se0/0/0`, set `clock rate 64000`, and execute `no shutdown`.
   - On Router-2, assign IP `172.16.20.2/24` to `Se0/0/0` and execute `no shutdown`.
6. **Controller Inspection & Verification:**
   - Run `show controllers serial 0/0/0` on both routers to verify DCE/DTE detection.
   - Run `show ip interface brief` and `show interfaces` to confirm operational status (`up / up`).
   - Ping between Router-1 and Router-2 over the serial WAN link.
   - Save the configuration permanently to NVRAM using `copy running-config startup-config`.

---

## 6. EXPERIMENTAL COMMANDS & OBSERVATIONS

### Task (a): Knowing the current mode & switching to privileged mode
- **Observation:** In User EXEC mode, the prompt ends with `>`. In Privileged EXEC mode, the prompt ends with `#`.
```text
Router> enable
Router# disable
Router>
```

### Task (b): Switching to configuration mode
- **Observation:** Global configuration mode displays the prompt `Router(config)#`.
```text
Router# configure terminal
Router(config)# exit
Router#
```

### Task (c): Obtaining router information (Type, OS, Memory, Interfaces)
- **Command:** `show version`
```text
Router# show version
Cisco IOS Software, 2800 Software (C2800NM-ADVIPSERVICESK9-M), Version 12.4(15)T1
ROM: System Bootstrap, Version 12.4(13r)T
Router uptime is 22 minutes
System image file is "flash:c2800nm-advipservicesk9-mz.124-15.T1.bin"
Cisco 2811 (revision 1.0) with 249856K/12288K bytes of memory.
2 FastEthernet interfaces
2 Serial(sync/async) interfaces
239K bytes of non-volatile configuration memory.
62720K bytes of ATA CompactFlash (Read/Write)
Configuration register is 0x2102
```

### Task (d): Viewing status of configured routing protocols
- **Command:** `show ip protocols`
```text
Router# show ip protocols
*** IP Routing is enabled ***
(No dynamic routing protocols active)
```

### Task (e): Showing the routing table
- **Command:** `show ip route`
```text
Router# show ip route
Codes: C - connected, S - static, R - RIP, M - mobile, B - BGP
Gateway of last resort is not set

C    172.16.10.0/24 is directly connected, FastEthernet0/0
C    172.16.20.0/24 is directly connected, Serial0/0/0
```

### Task (f): Saving the running configuration
- **Command:** `copy running-config startup-config` or `write memory`
```text
Router# copy running-config startup-config
Destination filename [startup-config]? 
Building configuration...
[OK]
```

### Task (g): Viewing the command history
- **Command:** `show history`
```text
Router# show history
  enable
  configure terminal
  show version
  show ip route
  show ip protocols
  copy running-config startup-config
  show history
```

### Task (h): Viewing and setting the router clock
- **Command:** `show clock` and `clock set`
```text
Router# clock set 11:15:00 11 October 2026
Router# show clock
11:15:02.418 UTC Sun Oct 11 2026
```

### Task (i): Viewing list of hosts
- **Command:** `show hosts`
```text
Router(config)# ip host PC-A 172.16.10.10
Router(config)# exit
Router# show hosts
Default domain is not set
Name/address lookup uses domain service

Host                      Port  Flags      Age Type   Address(es)
PC-A                      None  (temp, OK)  0   IP     172.16.10.10
```

### Task (j): Displaying interface statistics (Brief & Detailed)
- **Brief View:** `show ip interface brief`
```text
Router# show ip interface brief
Interface              IP-Address      OK? Method Status                Protocol
FastEthernet0/0        172.16.10.1     YES manual up                    up      
FastEthernet0/1        unassigned      YES unset  administratively down down    
Serial0/0/0            172.16.20.1     YES manual up                    up      
Serial0/0/1            unassigned      YES unset  administratively down down    
```
- **Detailed View:** `show interfaces Serial0/0/0`
```text
Router# show interfaces Serial0/0/0
Serial0/0/0 is up, line protocol is up
  Hardware is PowerQUICC MPC860
  Internet address is 172.16.20.1/24
  MTU 1500 bytes, BW 1544 Kbit/sec, DLY 20000 usec,
     reliability 255/255, txload 1/255, rxload 1/255
  Encapsulation HDLC, loopback not set
  Keepalive set (10 sec)
  Last input 00:00:04, output 00:00:02, output hang never
```

### Task (k): Knowing controller type (DTE or DCE)
- **Command:** `show controllers serial 0/0/0`
```text
Router1# show controllers serial 0/0/0
Interface Serial0/0/0
Hardware is PowerQUICC MPC860
DCE V.35, clock rate 64000
```
```text
Router2# show controllers serial 0/0/0
Interface Serial0/0/0
Hardware is PowerQUICC MPC860
DTE V.35, TX and RX clocks detected
```

### Task (l): Configuring Serial and Ethernet interfaces
- **Router 1 CLI Input:**
```text
Router1(config)# interface FastEthernet0/0
Router1(config-if)# ip address 172.16.10.1 255.255.255.0
Router1(config-if)# no shutdown
Router1(config-if)# exit
Router1(config)# interface Serial0/0/0
Router1(config-if)# ip address 172.16.20.1 255.255.255.0
Router1(config-if)# clock rate 64000
Router1(config-if)# no shutdown
```
- **Router 2 CLI Input:**
```text
Router2(config)# interface Serial0/0/0
Router2(config-if)# ip address 172.16.20.2 255.255.255.0
Router2(config-if)# no shutdown
```

---

## 7. VERIFICATION & CONNECTIVITY TESTING
- Ping from Router 1 to Router 2 across WAN link:
```text
Router1# ping 172.16.20.2
Type escape sequence to abort.
Sending 5, 100-byte ICMP Echos to 172.16.20.2, timeout is 2 seconds:
!!!!!
Success rate is 100 percent (5/5), round-trip min/avg/max = 1/2/4 ms
```
- Ping from PC-A to default gateway:
```cmd
C:\> ping 172.16.10.1
Pinging 172.16.10.1 with 32 bytes of data:
Reply from 172.16.10.1: bytes=32 time=1ms TTL=255
Reply from 172.16.10.1: bytes=32 time=1ms TTL=255
Reply from 172.16.10.1: bytes=32 time=1ms TTL=255
Reply from 172.16.10.1: bytes=32 time=1ms TTL=255
Ping statistics: Packets: Sent = 4, Received = 4, Lost = 0 (0% loss)
```

---

## 8. VIVA VOCE QUESTIONS & ANSWERS

**Q1: What is the difference between User EXEC mode and Privileged EXEC mode?**  
*Ans:* User EXEC mode (`>`) is a read-only mode with restricted access to basic network tests (`ping`, `traceroute`). Privileged EXEC mode (`#`) provides complete supervisory control, allowing full system inspection, debug tracing, and entry into configuration mode.

**Q2: What is the purpose of the `clock rate` command on a serial interface?**  
*Ans:* The `clock rate` command specifies the bit synchronization speed (in bits per second) across a WAN serial link. It must only be configured on the interface with the DCE cable connected, which simulates the telco CSU/DSU clocking device.

**Q3: Where is the `running-config` stored versus the `startup-config`?**  
*Ans:* The `running-config` resides in volatile RAM and is lost upon power loss. The `startup-config` is saved in NVRAM (Non-Volatile RAM), which retains data across reboots.

**Q4: What causes a router interface to display `Status: up, Protocol: down`?**  
*Ans:* Physical Layer 1 is operational (electrical signal detected), but Data Link Layer 2 encapsulation or keepalive protocol negotiation has failed (e.g., clock rate missing on DCE, framing mismatch, or HDLC/PPP protocol mismatch).

**Q5: What is the default configuration register value on a Cisco router and what does it mean?**  
*Ans:* `0x2102`. It instructs the router to look for boot instructions in NVRAM and load the Cisco IOS software image from Flash memory.

---

## 9. RESULT
All fundamental Cisco IOS router commands were successfully executed and verified in Cisco Packet Tracer. Serial and FastEthernet interfaces were configured, hardware controller types were verified, and end-to-end IP reachability was established.
