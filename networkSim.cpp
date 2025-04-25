// See whiteboard for system map.
// Compile in terminal with: clang++ networkSim.cpp -o out

/*
	Steps: 1) local devices connect to their local routers
	2) router routing tables initialized surface level
	2.5) router tables fully mapped (need to figure this out on paper)
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
		map<int, int> routingTable;	// Routing table: IP, port
		map<int, int> connectedDevices; // List of who's connected: index, MAC
		int numConnectedDevices;
	public:
		void setRouterName(string n) {
			name = n;
		}
		void connectMachineToRouter(Machine m) {
			connectedDevices.insert(make_pair(numConnectedDevices++,m.getDeviceMAC()));
			cout << "MAC " << hex << m.getDeviceMAC() << " successfully connected to " << name << endl;
		}
  };
  
  int main() {
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
  
    Router localRouter;
	Router TX;
	Router CA;
	Router IL;
	Router FL;
	Router NY;

	localRouter.setRouterName("Local Router");
	localRouter.connectMachineToRouter(localUser);
	localRouter.connectMachineToRouter(localA);
	localRouter.connectMachineToRouter(localB);

	CA.setRouterName("CA Router");
	CA.connectMachineToRouter(CA1);
	CA.connectMachineToRouter(CA2);

	NY.setRouterName("NY Router");
	NY.connectMachineToRouter(NY1);
	NY.connectMachineToRouter(NY2);

    return 0;
  }