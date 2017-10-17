//
// Created by Andy Thai, Collin Kinchen, David Madden on 10/12/17.
//
#include "awget.h"

void client(char * address, int port, int index, vector <Stone> & sstones){
    int clientSock;
    //int buffSize = 500;
    //char buff[buffSize];
    
    clientSock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(clientSock < 0){
        cerr << "ERROR CREATING CLIENT SOCKET" << endl;
        exit(EXIT_FAILURE);
    }
    
    cout << "Client socket created" << endl;
    
    struct sockaddr_in ServAddr;
    ServAddr.sin_family = AF_INET;
    ServAddr.sin_addr.s_addr = inet_addr(address);
    ServAddr.sin_port = htons(port);
    
    cout << "Connecting to server..." << endl;
    if(connect(clientSock,(struct sockaddr*)&ServAddr, sizeof(ServAddr)) < 0){
     cout << "ERROR IN CONNECT" << endl;
     close(clientSock);
     exit(EXIT_FAILURE);
    }
    
    cout << "Connected!" << endl;
    sstones.erase(sstones.begin() + index);
}



int main(int argc, char *argv[]) {
	//Argument Checking
	//Are there too many arguments
	if (argc > 3) {
		cout << "Too many arguments given " << endl;
		return -1;
	}
	string arg1 = argv[1];
	string arg2 = argv[2];
	int arg2len = arg2.length();
	//Is arg2 -p
	if (arg1 != "-p") {
		cout << "invalid argument" << endl;
		return -1;
	}
	//Is arg3 a digit
	for (int i = 0; i < arg2len; i++) {
		if (!isdigit(arg2[i])) {
			cout << "invalid port number" << endl;
			return -1;
		}
	}
	int arg2num = atoi(argv[2]);
	//Is arg3 a valid port number
	if (arg2num < 1024 || arg2num > 65535) {
		cout << "Port " << arg2num << " is out of range" << endl;
		return -1;
	}

	//recieve connection from and get open recieve packet to get sstone and URL
	//probably need to declare a struct to recieve info
			//need vector of stones
			//parent stone ip and port
			//URL
	
	vector <Stone> sstones; //will recieve this in struct from prior stone
	
	if (sstones.size() != 0){
		//find random stone to hop to again
		//pick random stone and obtain address and port
    Stone temp;
    char addr[100];
    string address = "";
    int port = 0;
    
    time_t t;
    srand((unsigned) time(&t));
    int randomNum = rand() % sstones.size();
    temp = sstones[randomNum];
    
    address = temp.addr;
    port = temp.port;
    //convert string address to char * addr
    strcpy(addr, address.c_str());
    //call client method here
    
	client(addr,port,randomNum,sstones);

	}else{
		//go out and get URL

		//system("wget -q " + URL);
		
		//get return address to last stone and send the downloaded file
		
	}
	
	
	
	
	return 0;
}