//
// Created by Andy Thai, Collin Kinchen, David Madden on 10/12/17.
//
#include "awget.h"
#include "ss.h"

vector <Stone> unpack(char chain[]) {
	vector <string> tokens;
	char *temp = strtok(chain, ",");
	while (temp != NULL) {
		
		string hello = temp;
		
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
				sstone.addr = temp;
				ip = 0;
			} else {
				int change = atoi(temp);
				sstone.port = change;
				ip = 1;
			}

			temp = strtok(NULL, " ");
		}
		sstones.push_back(sstone);
	}
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

string getFileName(string url) {
	std::size_t found = url.find_last_of("/\\");
	string file = url.substr(found + 1);
	string urlPattern = "^((https?://)|^(www\\.))[^/\n]+(?:/[^\\/%\n]+)*(?:/?\?[^&\n]+(?:&[^&\n]+)*)?/?$";
	regex reg(urlPattern);
	if (regex_match(file, reg) == true) {
		return "index.html";
	} else {
		return file;
	}
}


ReturnPacket client(char *url, char *address, int port, int parentPort, int index, vector <Stone> &sstones) {
	signal(SIGINT, closeServSocks);    //needed for catching '^C'
	ReturnPacket packet;
	char buffer[BUFSIZE];

	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock < 0) {
		cerr << "ERROR CREATING CLIENT SOCKET" << endl;
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in ServAddr;
	ServAddr.sin_family = AF_INET;
	ServAddr.sin_addr.s_addr = inet_addr(address);
	ServAddr.sin_port = htons(port);
	
	if (connect(clientSock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0) {
		cout << "ERROR IN CONNECT" << endl;
		close(clientSock);
		exit(EXIT_FAILURE);
	}

	FD_SET(clientSock, &readfds);
	n = clientSock + 1;
	tv.tv_sec = 3 * 60;
	tv.tv_usec = 0;

	sstones.erase(sstones.begin() + index);
	ConInfo info;
	string temp = address;

	strcpy(info.url, url);
	strcpy(info.sstones, serialize(sstones).c_str());
	
	send(clientSock, &info, sizeof(info), 0);

	sv = select(n, &readfds, NULL, NULL, &tv);
	if (sv == -1) {
		perror("select"); // error occurred in select()
	} else if (sv == 0) {
		printf("Timeout occurred!  No data after 10.5 seconds.\n");
	} else {
		// one or both of the descriptors have data
		if (FD_ISSET(clientSock, &readfds)) {
			int recvd = -1;
			recvd = recv(clientSock, buffer, BUFSIZE, 0);

			if (recvd < 0) {
				fprintf(stderr, "Issue with recv \n");
				printf("errno %d", errno);
				exit(EXIT_FAILURE);
			}
			ofstream myfile;
			filename = getFileName(info.url);

			myfile.open(filename);

			int file_size = atoi(buffer);
			int packnum = (file_size / BUFSIZE) + 1;
			int remain_data = file_size;
			int len;
			while ((remain_data > 0) && ((len = recv(clientSock, buffer, BUFSIZE, 0)) > 0)) {

				myfile.write(buffer, len);
				if (remain_data < BUFSIZE) { remain_data = -1; }
				else {
					remain_data -= len;
				}
				packnum--;
			}

			myfile.close();
			return packet;

		}
	}
	//}
	return packet;
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

		FD_ZERO(&readfds);
		FD_SET(sock_in, &readfds);
        
        cout << "Request: " << packet.url << endl;

		if (sstones.size() != 0) {
            
            cout << "Chainlist: " << endl;
            for(unsigned int i = 0; i < sstones.size(); i++){
                cout << sstones[i].addr << ", " << sstones[i].port << endl;
            }
            
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

            cout << "Next SS is: " << address << " " << port << endl;
            cout << "Waiting for file..." << endl;
            
			//call client method here
			client(packet.url, addr, port, atoi(PORT), randomNum,sstones);    //connect to new sstone

			//send(new_fd, &ret, sizeof(ret), 0);
			int fd = open(filename.c_str(), O_RDONLY);

			if (fd == -1) {
				fprintf(stderr, "Error opening file\n");
				exit(EXIT_FAILURE);
			}

			if (fstat(fd, &file_stat) < 0) {
				fprintf(stderr, "Error pulling file stats.\n");
				exit(EXIT_FAILURE);
			}
			sprintf(file_size, "%d", file_stat.st_size);
			//int size = (atoi(file_size) / BUFSIZE);
			int len = send(new_fd, file_size, sizeof(file_size), 0);
			if (len < 0) {
				fprintf(stderr, "Error on sending file size\n");
				exit(EXIT_FAILURE);
			}
			offset = 0;
			remain_data = file_stat.st_size;

			printf("Relaying file ...\n");

			while (((sent_bytes = sendfile(new_fd, fd, &offset, BUFSIZE)) > 0) && (remain_data > 0)) {
				remain_data -= sent_bytes;
			}
			cout << "Goodbye!" << endl;

		} else {
			cout << "Chainlist is empty" << endl;
            
			string url = packet.url;
			string command = "wget -q " + url;
			int result = system(command.c_str());
			filename = getFileName(url);
            
            cout << "Issuing wget for file: " << filename << endl;
            
			if (result < 0) {
				perror("wget error");
				exit(EXIT_FAILURE);
			}
			
			cout << "File received" << endl;

			int fd = open(filename.c_str(), O_RDONLY);

			if (fd == -1) {
				fprintf(stderr, "Error opening file\n");
				exit(EXIT_FAILURE);
			}

			if (fstat(fd, &file_stat) < 0) {
				fprintf(stderr, "Error pulling file stats.\n");
				exit(EXIT_FAILURE);
			}
			sprintf(file_size, "%d", file_stat.st_size);
			//int size = (atoi(file_size) / BUFSIZE);
			int len = send(new_fd, file_size, sizeof(file_size), 0);
			if (len < 0) {
				fprintf(stderr, "Error on sending file size\n");
				exit(EXIT_FAILURE);
			}
			offset = 0;
			remain_data = file_stat.st_size;

			printf("Relaying file ...\n");

			while (((sent_bytes = sendfile(new_fd, fd, &offset, BUFSIZE)) > 0) && (remain_data > 0)) {
				remain_data -= sent_bytes;
			}
			cout << "Goodbye!" << endl;
			
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
