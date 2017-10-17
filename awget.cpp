//
// Created by Andy Thai, Collin Kinchen, David Madden on 10/12/17.
//

//executed via: $./awget <URL> [-c chaingang.txt]

#include "awget.h"

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

void Awget::client(char *address, int port, int index, vector <Stone> &sstones) {
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
	info.url = "www.google.com";
//	info.sstones = sstones;
	send(clientSock, &info, sizeof(info), 0);
}

//Drives awget application
void Awget::initService(vector <Stone> &sstones) {
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
	client(addr, port, randomNum, sstones);
}

int main(int argc, char *argv[]) {
	Awget awget;

	//if only URL is specified
	if (argc == 2) {
		string url(argv[1]);
		if (!awget.isValid(url)) {
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
		string url(argv[1]);
		if (!awget.isValid(url)) {
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
	awget.initService(awget.sstones);

}
