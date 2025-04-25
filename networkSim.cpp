// See whiteboard for system map. Note that IPs are truncated to the first 8 bits: 192.168.0.0 is just 192.
// Compile in terminal with: clang++ networkSim.cpp -o out

/*
	Steps: 1) local devices connect to their local routers
	2) router routing tables initialized (this is manually done on a small scale, but in real life this is automated via sharing)
	3) Terminal outputs frame format, user can address stuff and indicates data to send, output is human readable
	4) each hop is printed out to terminal
	5) once data is recieved, other computer acks by "sending it back", just printing it.
	6) Can implement packets later, but for now, frames only.
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
		map<int, int> routingTable;	// Routing table: IP, port
		map<int, int> connectedDevices; // List of who's connected: index, MAC
		int numConnectedDevices;
	public:
		void setRouter(int i, string n) {
			name = n;
			IP = i;
		}
		void connectMachineToRouter(Machine m) {
			connectedDevices.insert(make_pair(numConnectedDevices++,m.getDeviceMAC()));
			cout << "MAC " << hex << m.getDeviceMAC() << " successfully connected to " << name << endl;
		}
		void addRoute (int IP, int port) {
			routingTable.insert(make_pair(IP,port));
		}
  };
  
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

	TX.setRouter(245, "Local Router, TX");
	TX.connectMachineToRouter(localUser);
	TX.connectMachineToRouter(localA);
	TX.connectMachineToRouter(localB);

	CA.setRouter(164, "CA Router");
	CA.connectMachineToRouter(CA1);
	CA.connectMachineToRouter(CA2);

	NY.setRouter(165, "NY Router");
	NY.connectMachineToRouter(NY1);
	NY.connectMachineToRouter(NY2);

	IL.setRouter(92, "IL Router");
	FL.setRouter(218, "FL Router");

	// Initialize routing tables
	TX.addRoute(0xA3F1, 3.1);
	TX.addRoute(0x2B8D, 3.2);
	TX.addRoute(0xC509, 3.3);
	TX.addRoute(164, 1);
	TX.addRoute(92, 2);
	TX.addRoute(218, 4);

	CA.addRoute(0xE76A, 1);

    return 0;
  }