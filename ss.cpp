//
// Created by Andy Thai, Collin Kinchen, David Madden on 10/12/17.
//
#include "awget.h"

#define BACKLOG 10
#define MAXDATASIZE 100

void client(char *address, int port, int index) {
	int clientSock;
	//int buffSize = 500;
	//char buff[buffSize];

	clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSock < 0) {
		cerr << "ERROR CREATING CLIENT SOCKET" << endl;
		exit(EXIT_FAILURE);
	}

//	cout << "Client socket created" << endl;

	struct sockaddr_in ServAddr;
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = inet_addr(address);
	ServAddr.sin_port = htons(port);

//	cout << "Connecting to server..." << endl;
	if (connect(clientSock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0) {
		cerr << "ERROR IN CONNECT" << endl;
		close(clientSock);
		exit(EXIT_FAILURE);
	}

//	cout << "Connected!" << endl;
	sstones.erase(sstones.begin() + index);
	ConInfo info;
	info.parent = address;
	info.url = url;
	info.sstones = sstones;
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

	//accept connection
	while (1) {
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
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
		struct ConInfo packet;
		if ((numbytes = recv(new_fd, &packet, sizeof(packet), 0)) == -1) {
			perror("recv");

		}
		cout << "got packet" << endl;
		//printf("URL: %s\n",packet.url);
		cout << "URL: " << packet.url << endl;
		cout << "packet.sstones[0].addr: " << packet.sstones[0].addr << endl;
	}
//	}
	//need to print packet

	//end socket opening

	//recieve connection from and get open recieve packet to get sstone and URL
	//probably need to declare a struct to recieve info
	//need vector of stones
	//parent stone ip and port
	//URL

	vector <Stone> sstones; //will recieve this in struct from prior stone

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
		cout << "going to go get another ss" << endl;
		client(addr, port, randomNum);

	} else {
		//go out and get URL
		string wget = "wget -q " + packet.url 
		system(wget);

		//get return address to last stone and send the downloaded file

	}


	return 0;
}