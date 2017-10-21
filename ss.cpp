//
// Created by Andy Thai, Collin Kinchen, David Madden on 10/12/17.
//
#include "awget.h"
#include "ss.h"

vector <Stone> unpack(char chain[]) {
	vector <string> tokens;
	char *temp = strtok(chain, ",");
	while (temp != NULL) {
		//cout << temp << endl;
		// char test[256];
		string hello = temp;
		//cout << hello << endl;
		// strcpy(test,tempc_str());
		tokens.push_back(hello);
		temp = strtok(NULL, ",");
	}

	int size = tokens.size();
	vector <Stone> sstones;
	for (int i = 0; i < size; i++) {
		string index = tokens[i];
		char test[256];
		strcpy(test, index.c_str());
		char *temp = strtok(test, " ");
		int ip = 1;
		Stone sstone;
		while (temp != NULL) {
			if (ip) {
				// cout << "IP: " << temp << endl;
				sstone.addr = temp;
				ip = 0;
			} else {
				// cout << "PORT: " << temp << endl;
				int change = atoi(temp);
				sstone.port = change;
				ip = 1;
			}

			temp = strtok(NULL, " ");
		}
		sstones.push_back(sstone);
		// cout << "NEXT" << endl;
	}

	// size = sstones.size();
	// for (int i = 0; i < size; i++){
	// cout << "IP: " << sstones[i].addr << endl;
	// cout << "PORT: " << sstones[i].port << endl;
	// cout << "NEXT" << endl;

	// }
	return sstones;
}

string serialize(vector <Stone> sstones) {
	string serStones;
	int size = sstones.size();
	for (int i = 0; i < size; ++i) {
		serStones += (sstones[i].addr + " " + to_string(sstones[i].port));
		if (i != size - 1) {
			serStones += ",";
		}
	}
	return serStones;
}

void closeServSocks(int sig) {
	FD_ZERO(&readfds);
	close(clientSock);
	close(new_fd);
	close(sock_in);
	exit(0);
}

void client(char *url, char *address, int port, int parentPort, int index, vector <Stone> &sstones) {
	signal(SIGINT, closeServSocks);    //needed for catching '^C'
//	int clientSock;
	//int buffSize = 500;
	//char buff[buffSize];

	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock < 0) {
		cerr << "ERROR CREATING CLIENT SOCKET" << endl;
		exit(EXIT_FAILURE);
	}

//	cout << "Client socket created" << endl;

	struct sockaddr_in ServAddr;
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = inet_addr(address);
	ServAddr.sin_port = htons(port);
//	cout << "trying to connect to: " << address << endl;
//	cout << "Connecting to server..." << endl;
	if (connect(clientSock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0) {
		cout << "ERROR IN CONNECT" << endl;
		close(clientSock);
		exit(EXIT_FAILURE);
	}

	FD_SET(clientSock, &readfds);
	n = clientSock + 1;
	tv.tv_sec = 3 * 60;
	tv.tv_usec = 0;
	//sv = select(n, &readfds, NULL, NULL, &tv);
	//		cout << "MADE IT HERE" << endl;


	cout << "Connected!" << endl;
	sstones.erase(sstones.begin() + index);
	ConInfo info;
	string temp = address;
//	info.parentPort = parentPort;
//	info.selfPort = port;
	strcpy(info.url, url);
	strcpy(info.sstones, serialize(sstones).c_str());
	//if (FD_ISSET(clientSock,&readfds)){
	send(clientSock, &info, sizeof(info), 0);

	sv = select(n, &readfds, NULL, NULL, &tv);
	if (sv == -1) {
		perror("select"); // error occurred in select()
	} else if (sv == 0) {
		printf("Timeout occurred!  No data after 10.5 seconds.\n");
	} else {
		cout << "HERE BITCHES" << endl;
		// one or both of the descriptors have data
		if (FD_ISSET(clientSock, &readfds)) {
			ConInfo packet;
			recv(clientSock, &packet, sizeof(packet), 0);

//			cout << "OUTPUT: " << packet.parentPort << endl;
		}
	}
	//}
}

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *) sa)->sin_addr);
	}
	return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

int checkArguments(int argc, char *argv[]) {
	//Argument Checking
	//Are there too many arguments
	if (argc > 3) {
		cerr << "Too many arguments given " << endl;
		return -1;
	}
	string arg1 = argv[1];
	string arg2 = argv[2];
	int arg2len = arg2.length();
	//Is arg2 -p
	if (arg1 != "-p") {
		cerr << "invalid argument" << endl;
		return -1;
	}
	//Is arg3 a digit
	for (int i = 0; i < arg2len; i++) {
		if (!isdigit(arg2[i])) {
			cerr << "invalid port number" << endl;
			return -1;
		}
	}
	int arg2num = atoi(argv[2]);
	//Is arg3 a valid port number
	if (arg2num < 1024 || arg2num > 65535) {
		cout << "Port " << arg2num << " is out of range" << endl;
		return -1;
	}
	return 0;
}

void establishConnection() {
	signal(SIGINT, closeServSocks);    //needed for catching '^C'

	//sockaddr_in is for socket that a sstone will listen to for incoming connection
	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(atoi(PORT));    //PORT is the sstones own port to listen to

	sock_in = socket(PF_INET, SOCK_STREAM, 0);    //incoming socket

	if (sock_in < 0) {
		perror("socket fail");
		exit(EXIT_FAILURE);
	}

	if (bind(sock_in, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(sock_in, MAXPENDING) < 0) {
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	struct hostent *he;    //for getting incoming connections ip address
	struct in_addr **addr_list;
	char hostname[128];

	char inIpAddress[INET6_ADDRSTRLEN];    //stores incoming connections ip address

	gethostname(hostname, sizeof hostname);
	he = gethostbyname(hostname);
	addr_list = (struct in_addr **) he->h_addr_list;

	cout << "My Ip Address: " << inet_ntoa(*addr_list[0]) << endl;
	cout << "Listening to PORT: " << ntohs(servAddr.sin_port) << endl;

	sockaddr_in their_addr;    //for connecting to incoming connections socket
	socklen_t sin_size = sizeof(their_addr);

	vector <Stone> sstones; //will recieve this in struct from prior stone

	//accept connection
	while (1) {
		sin_size = sizeof their_addr;
		new_fd = accept(sock_in, (struct sockaddr *) &their_addr, &sin_size);    //socket to recieve on

		if (new_fd == -1) {
			perror("new socket fail");
			exit(EXIT_FAILURE);
		}

		inet_ntop(their_addr.sin_family,
				  get_in_addr((struct sockaddr *) &their_addr),
				  inIpAddress, sizeof inIpAddress);

		int numbytes;

		//recieve packet
		ConInfo packet;

		if ((numbytes = recv(new_fd, &packet, sizeof(packet), 0)) == -1) {
			perror("recv");
			exit(EXIT_FAILURE);
		}

		sstones = unpack(packet.sstones);

		cout << "MY PARENTS IP IS: " << inIpAddress << endl;
//################################################################################
		FD_ZERO(&readfds);
		FD_SET(sock_in, &readfds);
		// n = new_fd;
		// tv.tv_sec = 3*60;
		// tv.tv_usec = 0;
		// sv = select(n, &readfds, NULL, NULL, &tv);
		// cout << "MADE IT HERE" << endl;

//################################################################################
		cout << "STONE SIZE: " << sstones.size() << endl;
		if (sstones.size() != 0) {
			cout << "SIZE ISNT 0" << endl;
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
			client(packet.url, addr, port, atoi(PORT), randomNum, sstones);    //connect to new sstone


		} else {
			cout << "got to last sstone" << endl;
			//go out and get URL
			//#############keepthis###############
			// string url = packet.url;
			// string command = "wget -q " + url;
			// cout << command << endl;
			// int result = system(command.c_str());
			// if (result < 0) {
			// perror("wget error");
			// exit(EXIT_FAILURE);
			// }
			//###########keepthis###################
			//if (FD_ISSET(new_fd,&readfds)){
			cout << "time to send things back" << endl;
			string temp = "SENDING THINGS PLACES!!!!";
			strcpy(buf1, temp.c_str());
			ConInfo test;
			test.parentPort = 69;
			send(new_fd, &test, sizeof(test), 0);
			//}
			//send(clientSock, &info, sizeof(info), 0);
			//get return address to last stone and send the downloaded file

		}
	}
}

int main(int argc, char *argv[]) {

	if (checkArguments(argc, argv) == -1) {
		return -1;
	}

	//open socket for SS server to listen on
	PORT = argv[2];

	establishConnection();

	return 0;
}