//
// Created by Andy Thai, Collin Kinchen, David Madden on 10/12/17.
//

//executed via: $./awget <URL> [-c chaingang.txt]

#include "awget.h"

/*
 * Reads file in and stores information inside Stone struct
 * IP stored as string
 * Port stored as int
 * Vector sstones holds all stepping stones
 */
void Awget::readFile(ifstream &inFile) {
	string line;
	getline(inFile, line);
	int numStones = stoi(line);
	for (int i = 0; i < numStones; ++i) {
		Stone sstone;
		getline(inFile, line);
		int index = line.find(" ");
		sstone.addr = line.substr(0, index);
		sstone.port = stoi(line.substr(index));
		sstones.push_back(sstone);
	}
}

/*
 * Regex check for url
 * Must begin with "www." or "https://"
 * Returns true for valid regex, false otherwise
 */
bool Awget::isValid(string url) {
	string urlPattern = "^((https?://)|^(www\\.))[^/\n]+(?:/[^\\/%\n]+)*(?:/?\?[^&\n]+(?:&[^&\n]+)*)?/?$";
	regex reg(urlPattern);

	if (regex_match(url, reg) == true) {
		return true;
	} else {
		return false;
	}
}

/*
 * Creates string to store all sstone information
 * String is [IP Address Port,IP Address Port...]
 * Allows for sstone information to be sent over socket
 */
string Awget::serialize() {
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

/*
 * Returns filename for sprintf
 * If no file at end of url, "index.html" will be used
 */
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

/*
 * Main program to create sockets and accept returning file
 * cerr will print out any error messages
 */
void Awget::client(char *address, int port, int index) {
	int clientSock;

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
		cerr << "ERROR IN CONNECT" << endl;
		close(clientSock);
		exit(EXIT_FAILURE);
	}

	sstones.erase(sstones.begin() + index);
	ConInfo info;
	string temp = address;

	strcpy(info.url, url.c_str());
	strcpy(info.sstones, serialize().c_str());
	send(clientSock, &info, sizeof(info), 0);

	char buffer[BUFSIZE];
	int recvd = -1;
	recvd = recv(clientSock, buffer, BUFSIZE, 0);

	if (recvd < 0) {
		fprintf(stderr, "Issue with recv \n");
		printf("errno %d", errno);
		exit(EXIT_FAILURE);
	}
	ofstream myfile;
	string fileName = getFileName(url);
	myfile.open(fileName);

	int file_size = atoi(buffer);
	int remain_data = file_size;
	int len;
	while ((remain_data > 0) && ((len = recv(clientSock, buffer, BUFSIZE, 0)) > 0)) {
		myfile.write(buffer, len);
		remain_data -= len;
	}
	cout << "Received file: " << fileName << endl;
	cout << "Goodbye!" << endl;

	myfile.close();
	
}

/*
 * Will pick a random stone to use in connection
 * Will call client method to connect to sstone
 */
void Awget::initService() {
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
    
    cout << "Next SS is: " << address << " " << port << endl;
    cout << "Waiting for file..." << endl;
	//convert string address to char * addr
	strcpy(addr, address.c_str());
	client(addr, port, randomNum);
}

int main(int argc, char *argv[]) {
	Awget awget;
	//if only URL is specified
	if (argc == 2) {
		awget.url = argv[1];
		if (!awget.isValid(awget.url)) {
			cerr << "URL INVALID" << endl;
			exit(EXIT_FAILURE);
		}
		
		cout << "Request: " << awget.url << endl;
		
		//Read from local file "chaingang.txt"
		ifstream reader("chaingang.txt");
		if (!reader.good()) {
			cerr << "FILE COULD NOT BE REACHED/INVALID" << endl;
			exit(EXIT_FAILURE);
		} else {
			awget.readFile(reader);
		}
		reader.close();
	}
		//if URL and chaingang file given and -c flag is used
	else if (argc > 2 && (strcmp(argv[2], "-c") == 0)) {
		awget.url = argv[1];
		if (!awget.isValid(awget.url)) {
			cerr << "URL INVALID" << endl;
			exit(EXIT_FAILURE);
		}
		ifstream reader(argv[3]);
		if (!reader.good()) {
			cerr << "FILE COULD NOT BE REACHED/INVALID" << endl;
			exit(EXIT_FAILURE);
		} else {
			awget.readFile(reader);
		}
		reader.close();
	} else {
		cerr << "DID NOT RECEIVE CORRECT ARGUMENTS" << endl;
		exit(EXIT_FAILURE);
	}
	
	cout << "Chainlist: " << endl;
    for(unsigned int i = 0; i < awget.sstones.size(); i++){
        cout << awget.sstones[i].addr << ", " << awget.sstones[i].port << endl;
    }
	
	awget.initService();
}
