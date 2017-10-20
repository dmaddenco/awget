//
// Created by Andy Thai, Collin Kinchen, David Madden on 10/12/17.
//

//executed via: $./awget <URL> [-c chaingang.txt]

#include "awget.h"

#define AWGET_PORT 5000

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

//Check if URL is valid
bool Awget::isValid(string url) {
	string urlPattern = "^((https?://)|^(www\\.))[^/\n]+(?:/[^\\/%\n]+)*(?:/?\?[^&\n]+(?:&[^&\n]+)*)?/?$";
	regex reg(urlPattern);

	if (regex_match(url, reg) == true) {
		return true;
	} else {
		return false;
	}
}

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

void Awget::client(char *address, int port, int index) {
	int clientSock;
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

//	cout << "Connecting to server..." << endl;
	if (connect(clientSock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0) {
		cerr << "ERROR IN CONNECT" << endl;
		close(clientSock);
		exit(EXIT_FAILURE);
	}

//	cout << "Connected!" << endl;
	sstones.erase(sstones.begin() + index);
	ConInfo info;
//	info.parent = address;
//	info.url = url;
//	string temp = serialize();
	string temp = address;
//	strcpy(info.parent, temp.c_str());
	info.parentPort = AWGET_PORT;
	strcpy(info.url, url.c_str());
	strcpy(info.sstones, serialize().c_str());
//	info.sstones = serialize().c_str();
//	cout << info.sstones << endl;
	send(clientSock, &info, sizeof(info), 0);
}

//Drives awget application
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
	//convert string address to char * addr
	strcpy(addr, address.c_str());
	//call client method here
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
	awget.initService();

}
