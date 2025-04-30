This project is a custom-built C++ simulation of a simplified computer network, focusing on core concepts like packet framing,
local vs. remote routing, and dynamic routing tables. It features:

  -Custom Frame Structure: Includes source/destination addressing, data payload, and frame check logic (simple checksum).
  
  -Routing Logic:
  
        Devices can be directly connected or reachable via multiple hops.
        
        Routers use prefix-based routing tables (i.e., address 73 maps to prefix 50).
        
        Frames are routed either locally or via recursive hops through intermediary routers.
        
  -Interactive Console Input: Users can simulate sending messages between machines, observe how the frame is forwarded, and view routing decisions.
  
  -Hashmap-Based Data Structures:
  
        connectedDevices maps device addresses to ports.
        
        routingTable maps address prefixes to {port, next router} pairs.
        
  -Binary Frame Display: Frame data is printed as binary for a more technical visual representation.

This project was built to deepen understanding of packet-switched networks and routing principles. 

While not intended as a full protocol implementation, it's a hands-on tool for exploring how routers forward frames and make routing decisions.

```
Ethernet Frame (abridged):
  - - - - - - - - - - - - - - - - - - -  
| Preamble | Dest. | Src. | Data | FCS |
  - - - - - - - - - - - - - - - - - - -  

Your machine: 
Address: 52 | Connected to router @ addr: 89

Initializing network with broadcast...

FRAME: | 10101010 | 11111111 | 00110100 | 01001000 01100101 01101100 01101100 01101111 00100001 00100000 01010111 01101000 01101111 00100000 01101001 01110011 00100000 01100011 01101111 01101110 01101110 01100101 01100011 01110100 01100101 01100100 00111111 | 00110011 |

Connected Devices for Router 89:
  Device Address: 52 -> Port: 1
  Device Address: 67 -> Port: 2
  Device Address: 71 -> Port: 3

DNS query:
  Domain Name: NY-server-1 -> Address: 206
  Domain Name: CA-server-2 -> Address: 19
  Domain Name: CA-server-1 -> Address: 6
  Domain Name: NY-server-2 -> Address: 230
  Domain Name: UK-server-1 -> Address: 250
  Domain Name: UK-server-2 -> Address: 252

Input target destination address: 252
Input data you want to send (text) (q to quit): Hello, UK Server 2!

FRAME: | 10101010 | 11111100 | 00110100 | 01001000 01100101 01101100 01101100 01101111 00101100 00100000 01010101 01001011 00100000 01010011 01100101 01110010 01110110 01100101 01110010 00100000 00110010 00100001 | 00110000 |

Routing to next router: 10 via port: 3
Routing to next router: 181 via port: 4
Routing to next router: 11 via port: 4
Routing to next router: 90 via port: 4
Routing to next router: 140 via port: 2
Routing to next router: 220 via port: 1

   I am device 252 and router 220 sent me: 
   Hello, UK Server 2!
   From machine: 52 via port: 2
```

Sample output of the most complex route ^. 
The frames consist of a preamble (10101010 as an example, which would allow for the recipient's clock to synchronize and also provide an easy error check), a destination address, source addr., the data (in real life these are IP packets, but in my sim it's simpler, just the ascii of the string the user inputs), and the frame check sequence (some math used to check if you received the data in whole (but in my sim there's no loss programmed... yet... the FCS is just for fun). 
Then it gives info on your local machine, and automatically broadcasts to address FFFF to find out who else is connected to the local router (in this case devices at addrs 67 and 71). 
Then a DNS query is probed with the default DNS of the local router, and this shows the domain names of servers it can reach. 
The user then inputs their target, the data to send, and the frame is constructed and shown byte-by-byte. 
Each hop the frame takes is output so the user can see their packet go, and finally once it's received, a confirmation is sent back by the recipient (later, TCP will be implemented and data will be split into packets, but that's a whole separate project).

https://sites.google.com/view/reagans-personal-portfolio/internet-protocol-sim
