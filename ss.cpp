//
// Created by Andy Thai, Collin Kinchen, David Madden on 10/12/17.
//
#include "awget.h"

#define BACKLOG 10
#define MAXDATASIZE 100

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

void client(char *url, char *address, int port, int index, vector <Stone> &sstones) {
	int clientSock;
	//int buffSize = 500;
	//char buff[buffSize];

	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock < 0) {
		cerr << "ERROR CREATING CLIENT SOCKET" << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Client socket created" << endl;

	struct sockaddr_in ServAddr;
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = inet_addr(address);
	ServAddr.sin_port = htons(port);
	cout << "trying to connect to: " << address << endl;
	cout << "Connecting to server..." << endl;
	if (connect(clientSock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0) {
		cout << "ERROR IN CONNECT" << endl;
		close(clientSock);
		exit(EXIT_FAILURE);
	}

	cout << "Connected!" << endl;
	sstones.erase(sstones.begin() + index);
	ConInfo info;
	string temp = address;
	strcpy(info.parent, temp.c_str());
	strcpy(info.url, url);
	strcpy(info.sstones, serialize(sstones).c_str());
	send(clientSock, &info, sizeof(info), 0);
}

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *) sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

void sigchld_handler(int s) {
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while (waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
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

	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
//	struct sigaction sa;
	//TODO: handle sigaction stuff?
	int yes = 1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;



	//open socket for SS server to listen on
	char *PORT = argv[2];

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
							 p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
					   sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
/*
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
*/

	struct hostent *he;
	struct in_addr **addr_list;
	char hostname[256];
	gethostname(hostname, 256);
	he = gethostbyname(hostname);
	addr_list = (struct in_addr **) he->h_addr_list;

	printf("Waiting for connections on...\n");
	printf("%s port %s\n", inet_ntoa(*addr_list[0]), PORT);
	vector <Stone> sstones; //will recieve this in struct from prior stone

	//accept connection
//	while (1) {
	sin_size = sizeof their_addr;
	new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
	if (new_fd == -1) {
		perror("accept");
//			continue;
	}

	inet_ntop(their_addr.ss_family,
			  get_in_addr((struct sockaddr *) &their_addr),
			  s, sizeof s);
	printf("Found a friend! got connection from %s\n", s);
	printf("You recieve first.\n");
	int numbytes;

	//recieve packet
	//TODO: figure out if we still need this while loop
//	while(true){
	ConInfo packet;
	if ((numbytes = recv(new_fd, &packet, sizeof(packet), 0)) == -1) {
		perror("recv");

	}
//		cout << "got packet" << endl;
//		string parent = packet.parent;
//		string url = packet.url;
//		string chaingain = packet.sstones;
//		cout << parent << endl;
//		cout << url << endl;
//		cout << chaingain << endl;
	sstones = unpack(packet.sstones);
//	}
//	}

	cout << "made it past where you want to be" << endl;
	//need to print packet

	//end socket opening

	//recieve connection from and get open recieve packet to get sstone and URL
	//probably need to declare a struct to recieve info
	//need vector of stones
	//parent stone ip and port
	//URL



	if (sstones.size() != 0) {
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

		client(packet.url, addr, port, randomNum, sstones);

	} else {
		cout << "got to last sstone" << endl;
		//go out and get URL
//		string url = packet.url;
//		system("wget -q " + packet.url);

		//get return address to last stone and send the downloaded file

	}

	return 0;
}