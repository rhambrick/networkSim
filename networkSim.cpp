// Compile in terminal with: clang++ networkSim.cpp -o out -std=c++11

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

class Frame {
    public:
        int destinationAddress;
        int sourceAddress;
        string data;

        // Constructor
        Frame (int dest, int source, string d) {
            destinationAddress = dest;
            sourceAddress = source;
            data = d;
        }
        void printFrame() {
            // note 8 bit limit, so addresses are 0-255.
            cout << endl << "FRAME: | 10101010 | " << bitset<8>(destinationAddress) << " | " << bitset<8>(sourceAddress) << " | ";
            // print data string as ascii binary for fun
            for (char c : data) {
                for (int i = 7; i >= 0; --i) {
                    cout << ((c >> i) & 1);
                }
                cout << ' '; // space between characters
            }
            int frameCheck = destinationAddress + sourceAddress;
            cout << "| " << bitset<8>(frameCheck) << " |" << endl << endl;
        }
  };

class Router {
    public:
        int address;
        unordered_map<int, int> connectedDevices; // Hashmap for seeing what devices are connected: device addr, port
        unordered_map<int, vector<int> > routingTable; // Map for routing table: destination addr prefix, [port, next router]
        // Routing table example: 151, [3, 98] means that addresses in the 150 bucket (151-200) can be reached via port 3 where router 98 is connected.

        // Constructor
        Router (int addr) {
            address = addr;
            connectedDevices;
            routingTable;
        }
        void addDevice(int deviceAddr, int port) {
            connectedDevices[deviceAddr] = port;
        }
        void addRoute(int destRouterAddr, vector<int> ports) {
            routingTable[destRouterAddr] = ports;   // ports contain next router addr too
        }
        void printConnectedDevices() {
            cout << "Connected Devices for Router " << address << ":\n";
            for (const auto& element : connectedDevices) {
                cout << "  Device Address: " << element.first << " -> Port: " << element.second << endl;
            }
            cout << endl;
        }
        void printRoutingTable() {      // debug purposes
            cout << "Routing Table for Router " << address << ":\n";
            for (const auto& entry : routingTable) {
                cout << "  Destination Addr. Prefix: " << entry.first << " -> Ports, Next Router: ";
                for (int port : entry.second) {
                    cout << port << " ";
                }
                cout << endl;
            }
            cout << endl;
        }
};

void sendFrame(Frame outboundFrame, Router nearestRouter, unordered_map<int, Router> allRouters) {
    // Broadcast
    if (outboundFrame.destinationAddress == 0xFFFF) {
        nearestRouter.printConnectedDevices();
    }
    // Everything else
    else {
        bool frameSent = false;
        // 1) check if destination address is directly connected, if so, send it on easily
        // 2) check if destination address is in routing table range
        // 2.5) if so, forward frame out of port indicated in routing table, printing that result
        // 2.75) nearestRouter now needs to become the next router in the chain, can include this in routing table also
        // 3) print confirmation once packet is recieved (this will be removed later when we introduce some TCP stuff)
        if (nearestRouter.connectedDevices.find(outboundFrame.destinationAddress) != nearestRouter.connectedDevices.end()){
            cout << endl << "    I am device " << outboundFrame.destinationAddress << " and router " << nearestRouter.address << " sent me: " << endl << "    " << outboundFrame.data << endl <<"    From machine: " << outboundFrame.sourceAddress << " via port: " << nearestRouter.connectedDevices[outboundFrame.destinationAddress] << endl << endl;
            frameSent = true;
            return;
        }

        if (!frameSent) {
            // Loop through routing table entry by entry
            for (const auto& entry : nearestRouter.routingTable) {
                // Grab the prefix of IP addresses
                int prefix = entry.first;
                // if destination address is within range of prefix (0-49, 50-99, 100-149, 150-199, 200-249)
                if (outboundFrame.destinationAddress >= prefix && outboundFrame.destinationAddress < prefix + 50) {
                    int port = entry.second[0];
                    int nextRouterAddr = entry.second[1];

                    cout << "Routing to next router: " << nextRouterAddr << " via port: " << port << endl;

                    Router nextRouter = allRouters.at(nextRouterAddr);
                    sendFrame(outboundFrame, nextRouter, allRouters);
                }
                // else, if the address is > 249 (these are the "overseas addresses")
                else if (outboundFrame.destinationAddress >= 250) {
                    int port = entry.second[0];
                    int nextRouterAddr = entry.second[1];

                    cout << "Routing to next router: " << nextRouterAddr << " via port: " << port << endl;

                    Router nextRouter = allRouters.at(nextRouterAddr);
                    sendFrame(outboundFrame, nextRouter, allRouters);
                }
            }
        }
        // need to handle invalid addresses or routes, but I think i will rewrite this function tomorrow.
    }
}

void printDNS(unordered_map<string, int> table) {
    cout << "DNS query:" << endl;
    for (const auto& entry : table) {
        string name = entry.first;
        int address = entry.second;
        cout << "  Domain Name: " << name << " -> Address: " << address << endl;
    }
    cout << endl;
}

int main() {
    //Terminal initializes frame format
	cout << endl << "Ethernet Frame (abridged):" << endl;
	cout << "  - - - - - - - - - - - - - - - - - - -  " << endl;
	cout << "| Preamble | Dest. | Src. | Data | FCS |" << endl;
	cout << "  - - - - - - - - - - - - - - - - - - -  " << endl << endl;

    cout << "Your machine: " << endl;
    cout << "Address: 52 | Connected to router @ addr: 89" << endl << endl;

    // Initialize routers | POSSIBLE ADDRESSES: CA: 0-49, TX: 50-99, IL: 100-149, FL: 150-199, NY: 200-249
    unordered_map<int, Router> allRouters;  // list of routers for routing tables (addr, Router)

    Router localRouter(89); // Local router addr 89

    localRouter.addDevice(52, 1);  // User's Machine, port 1
    localRouter.addDevice(67, 2);  // Local Machine A: addr 67, port 2
    localRouter.addDevice(71, 3);  // Local Machine B: addr 71, port 3
    
    localRouter.addRoute(0, {4, 10});   // To IPs 0-50, via port 4 to router 10
    localRouter.addRoute(200, {5, 139});    // To IPs 200-250, via port 5 to router 139
    localRouter.addRoute(200, {6, 181});    // Also to 200 range IPs, via port 6 to router 181

    allRouters.insert({localRouter.address, localRouter});  // Add to allRouters map (addr, Router)

    Router CA(10);  // CA Router addr. 10
    CA.addDevice(6, 1); // CA machine A: addr 6, port 1
    CA.addDevice(19, 2); // CA machine B: addr. 19, port 2
    allRouters.insert({CA.address, CA});

    Router IL(139); // IL Router addr. 139
    IL.addRoute(200, {3, 219}); // To IPs 200-249, via port 3 to router 219
    allRouters.insert({IL.address, IL});

    Router FL(181); // FL Router addr. 181
    FL.addRoute(200, {3, 219}); // To IPs 200-249, via port 3 to router 219
    FL.addRoute(250, {4, 11}); // To IPs 250-255, via port 4 to router 11 overseas
    allRouters.insert({FL.address, FL});

    Router NY(219); // NY Router addr. 219
    NY.addDevice(206, 4);   // NY machine A: addr 206 port 4
    NY.addDevice(230, 3);   // NY machine B: addr 230 port 3
    allRouters.insert({NY.address, NY});

    Router PO(11);    // PO (oversea) Router addr. 11
    PO.addRoute(250, {4, 90});  // To IPs 250-255, via port 2 to router 90
    allRouters.insert({PO.address, PO});

    Router SP(90);    // SP (oversea) Router addr. 90
    SP.addRoute(250, {2, 140}); // To IPs 250-255, via port 2 to router 140
    allRouters.insert({SP.address, SP});

    Router FR(140);    // FR (oversea) Router addr. 140
    FR.addRoute(250, {1, 220}); // To IPs 250-255, via port 1 to router 220
    allRouters.insert({FR.address, FR});

    Router UK(220);    // UK (oversea) Router addr. 220
    UK.addDevice(250, 1); // NY machine A: addr 206 port 1
    UK.addDevice(252, 2); // UK machine B: addr 206 port 2
    allRouters.insert({UK.address, UK});

    // Broadcasting to discover who else is connected to the local network
    cout << "Initializing network with broadcast..." << endl;
    Frame BroadcastFrame (0xFFFF, 52, "Hello! Who is connected?");
    BroadcastFrame.printFrame();
    sendFrame(BroadcastFrame, localRouter, allRouters);

    // DNS printout (sometime soon a "query nearest router" will be used)
    unordered_map<string, int> dnsTable = {
        {"CA-server-1", 6},
        {"CA-server-2", 19},
        {"NY-server-1", 206},
        {"NY-server-2", 230},
        {"UK-server-1", 250},
        {"UK-server-2", 252}
    };

    printDNS(dnsTable);

    // Begin loop for user to use program
    bool looping = true;
    int destinationAddress = 0;
    string data;
    while (looping) {

        // User input / error checking
        cout << "Input target destination address: ";
        cin >> destinationAddress;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // cin won't clear newline. stack overflow says this clears out cin before doing getline (since getline ends on \n)

        if (cin.fail()){
            // not valid int input
            cout << "Integers addresses only pls." << endl;
            // for continuous input uncomment these and change break to continue
            // cin.clear();
            // cin.ignore(256,'\n');   // stack overflow, ignore input buffer
            break;
        }
        else if (destinationAddress == 52) {
            cout << "Cannot send data to yourself!" << endl;
            continue;
        }

        cout << "Input data you want to send (text) (q to quit): ";
        getline(cin, data);
        if (data == "q" || data == "quit" || data == "Q" || data == "Quit" || data == "QUIT") {
            break;
        }

        // Frame construction
        Frame userFrame (destinationAddress, 52, data);
        userFrame.printFrame(); 
        
        // send
        sendFrame(userFrame, localRouter, allRouters);
    }

    return 0;
}