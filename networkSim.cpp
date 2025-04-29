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
            cout << " | " << bitset<8>(frameCheck) << " |" << endl << endl;
        }
  };

class Router {
    public:
        int address;
        unordered_map<int, int> connectedDevices; // Hashmap for seeing what devices are connected: device addr, port
        unordered_map<int, vector<int> > routingTable; // Map for routing table: router addr, [list of possible ports]

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
            routingTable[destRouterAddr] = ports;
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
                cout << "  Destination Router Address: " << entry.first << " -> Ports: ";
                for (int port : entry.second) {
                    cout << port << " ";
                }
                cout << endl;
            }
            cout << endl;
        }
};

void sendFrame(Frame outboundFrame, Router nearestRouter) {
    // Broadcast
    if (outboundFrame.destinationAddress == 0xFFFF) {
        nearestRouter.printConnectedDevices();
    }
    else {
        if (nearestRouter.connectedDevices.find(outboundFrame.destinationAddress) == nearestRouter.connectedDevices.end()) {    // TODO: REMOVE THIS ONCE ROUTING TABLE SET UP
            cout << outboundFrame.destinationAddress << " destination is not connected!" << endl;
            return;
        }

    }
}

int main() {
    //Terminal initializes frame format
	cout << endl << "Ethernet Frame (abridged):" << endl;
	cout << "  - - - - - - - - - - - - - - - - - - -  " << endl;
	cout << "| Preamble | Dest. | Src. | Data | FCS |" << endl;
	cout << "  - - - - - - - - - - - - - - - - - - -  " << endl << endl;

    cout << "Your machine: " << endl;
    cout << "Address: 52 | Connected to router @ addr: 89" << endl << endl;

    // Initialize routers
    Router localRouter(89);
    localRouter.addDevice(52, 1);  // User's Machine, port 1
    localRouter.addDevice(67, 2);  // Local Machine A, port 2
    localRouter.addDevice(71, 3);  // Local Machine B, port 3

    // Broadcasting to discover who else is connected to the local network
    cout << "Initializing network..." << endl;
    Frame BroadcastFrame (0xFFFF, 52, "Hello! Who is connected?");
    sendFrame(BroadcastFrame, localRouter);

    // Begin loop for user to use program
    bool looping = true;
    int destinationAddress = 0;
    string data;
    while (looping) {

        // User input / error checking
        cout << "Input target destination address: ";
        cin >> destinationAddress;

        if (cin.fail()){
            // not valid int input
            cout << "Integers addresses only pls." << endl;
            // for continuous input uncomment these
            // cin.clear();
            // cin.ignore(256,'\n');   // stack overflow, ignore input buffer
            break;
        }
        else if (destinationAddress == 52) {
            cout << "Cannot send data to yourself!" << endl;
            continue;
        }

        cout << "Input data you want to send (text) (q to quit): ";
        cin >> data;
        if (data == "q" || data == "quit" || data == "Q" || data == "Quit" || data == "QUIT") {
            break;
        }

        // Frame construction
        Frame userFrame (destinationAddress, 52, data);
        userFrame.printFrame(); 
        
        // send
        sendFrame(userFrame, localRouter);
    }

    return 0;
}