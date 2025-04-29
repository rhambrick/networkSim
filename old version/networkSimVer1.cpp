// See whiteboard for system map. Note that IPs are truncated to the first 8 bits: 192.168.0.0 is just 192.
// Compile in terminal with: clang++ networkSimVer1.cpp -o out

/*
	Steps: 1) local devices connect to their local routers
	2) router routing tables initialized (this is manually done on a small scale, but in real life this is automated via sharing)
	3) Terminal outputs frame format, user can address stuff and indicates data to send, output is human readable
	4) each hop is printed out to terminal
	5) once data is recieved, other computer acks by "sending it back", just printing it.
	6) Can implement packets later, but for now, frames only.

	Protocols: 1) user's machine needs to discover mac addresses of local devices. Broadcast to FFFF can be done.
	2) for simplicity, we will assume there's a DNS, so the user will be provided with the IPs of routers and such.
	3) also for simplicity, ethernet frames will carry only IP packets, so "type" will always be "0x0800".
	4) later, we will implement a max packet size, and some TCP type stuff to simulate re-arranging packets and verifying all data is recieved.
	5) traffic is not an issue, but can be implemented later, so hops with multiple paths will be randomly chosen.
*/


#include <iostream>
#include <string>
#include <map>
using namespace std;

class Machine {       
    private:
        int addr;
        string deviceType;
    public:
        void setDevice(int a, string t) {
            addr = a;
            deviceType = t;
       }
    	int getDeviceMAC() {
            return addr;
       }
        string getDeviceType() {
            return deviceType;
       }
  };

class Router {
	private:
		string name;
		int IP;
		map<int, vector<int> > routingTable;	// Routing table: IP, possible port list. Hashmap for quick lookups.
		map<int, int> connectedDevices; // List of who's connected: index, MAC
		int numConnectedDevices;
	public:
		void setRouter(int i, string n) {
			name = n;
			IP = i;
		}
		void connectMachineToRouter(Machine m) {
			connectedDevices.insert(make_pair(numConnectedDevices++,m.getDeviceMAC()));
			// cout << "MAC " << hex << m.getDeviceMAC() << " successfully connected to " << name << endl;
		}
		void setRoutingTable(map<int, vector<int> > inputRoutingTable) {
			routingTable = inputRoutingTable;
		}
		void getConnectedDevices() {
			for (const auto& pair : connectedDevices) {
				cout << pair.second << endl;
			}
		}
		void getRoutingTable() {
			cout << "Routing Table for " << name << ":\n";
        	for (const auto& entry : routingTable) {
            	cout << "Prefix " << entry.first << ": Ports ";
            	for (int port : entry.second) {
                	cout << port << " ";
            	}
            	cout << endl;
        	}
		}
  };

class Frame {	// I don't want to encapsulate so much, so the the IP packet stuff will be inside this class as well.
	public:
		int preamble;
		int destination;
		int source;
		int type;
		// begin DATA section, IP packet
			int timeToLive;
			int sourceIP;
			int destinationIP;
			string data;
		int checkSequence; // I'm gonna be honest, this is a simulation and this frame check sequence is not needed. But it's there for the heck of it.
		
		Frame(int dest, int src, int srcIP, int destIP, string payload) {	// Constructor
			preamble = 0b10101010;
			destination = dest;
			source = src;
			type = 0x0800;
			timeToLive = 5;
			sourceIP = srcIP;
			destinationIP = destIP;
			data = payload;
			checkSequence = destination + source; // I made this up, but it can be a simple check
		}
};

void send(Frame outFrame, Router nearestRouter) {
	/*
	Steps: 1) look at destination MAC
	2) consult routing tables
	3) print out hops
	4) adjust values as needed
	*/
	if (outFrame.destination == 0xFFFF)	{ // broadcast
		cout << "LAN devices:" << endl;
		nearestRouter.getConnectedDevices();
		cout << "WAN routers:" << endl;
		nearestRouter.getRoutingTable();
	}
  }
  
  int main() {
	// Initialize machines and their MACs
    Machine localUser;
	Machine localA;
	Machine localB;

	Machine CA1;
	Machine CA2;

	Machine NY1;
	Machine NY2;

    localUser.setDevice(0xA3F1, "your PC"); 
    localA.setDevice(0x2B8D, "smart TV");
	localB.setDevice(0xC509, "printer");

	CA1.setDevice(0xE76A, "CA server 1");
	CA2.setDevice(0x19C4, "CA server 2");

	NY1.setDevice(0xD2E6, "NY server 1");
	NY2.setDevice(0x70B3, "NY server 2");
  
    // Initialize device connections to their local routers
	Router TX;
	Router CA;
	Router IL;
	Router FL;
	Router NY;

	TX.setRouter(223, "Local Router");
	TX.connectMachineToRouter(localUser);
	TX.connectMachineToRouter(localA);
	TX.connectMachineToRouter(localB);

	CA.setRouter(101, "CA Router");
	CA.connectMachineToRouter(CA1);
	CA.connectMachineToRouter(CA2);

	NY.setRouter(790, "NY Router");
	NY.connectMachineToRouter(NY1);
	NY.connectMachineToRouter(NY2);

	IL.setRouter(409, "IL Router");
	FL.setRouter(681, "FL Router");

	// Initialize routing tables
	// TX: Port 1 goes to IP prefixe ranges 100-199, port 2 && port 4 go to ranges 400-799 (once we implement traffic, these two can be split to improve flow).
	// TX.setRoutingTable({ {100, {1}}, {400, {2}}, {600, {4}}, {700, {2, 4}} });
	
	// CA: port 2 goes to all IP ranges.
	// CA.setRoutingTable({ {200, {2}}, {400, {2}}, {600, {2}}, {700, {2}} });

	// IL: port 1: 100-299, port 2: 600-699, port 3: 700-799
	// IL.setRoutingTable({ {100, {1}}, {200, {1}}, {600, {2}}, {700, {3}} });

	// FL: port 1: 400-499, port 2: 200-299, port 3: 700-799
	// FL.setRoutingTable({ {400, {1}}, {600, {2}}, {700, {3}} });

	// NY: port 1: 400 200 100, port 2: 600 200 100
	// NY.setRoutingTable({ {400, {1}}, {200, {1}}, {100, {1}}, {600, {2}}, {200, {2}}, {100, {2}} });

	//Terminal initializes frame format
	cout << endl << "Ethernet Layer:" << endl;
	cout << "  - - - - - - - - - - - - - - - - - - - - - - - - - - - -  " << endl;
	cout << "| Preamble | Dest. | Src. | Type | Data (IP Packet) | FCS |" << endl;
	cout << "  - - - - - - - - - - - - - - - - - - - - - - - - - - - -  " << endl;
	cout << "                                /                    /     " << endl;
	cout << "                               /                    /      " << endl;
	cout << "                              /                    /       " << endl;
	cout << "                             /                    /        " << endl;
	cout << "                            /                    /         " << endl;
	
	cout << "IP Layer (abridged):" << endl;
	cout << "  - - - - - - - - - - - - - - - - - - - - - " << endl;
	cout << "| Time To Live | Src. IP | Dest. IP | Data |" << endl;
	cout << "  - - - - - - - - - - - - - - - - - - - - - " << endl << endl;

	// Broadcast step: figure out who else is connected locally. Source and dest IP must be machine's closest router.
	// Frame format: Frame(int dest, int src, string srcIP, string destIP, string payload)
	Frame Broadcast(0xFFFF, 0xA3F1, 223, 223, "Hello, who is connected?");
	send(Broadcast, TX); // have to send nearest router. Could use hashmap for this to map IPs to router object, but at this point idc, hard coding it.

	// Prompt user for input
	cout << "What MAC address are you sending to?";

    return 0;
  }