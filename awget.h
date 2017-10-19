//
// Created by Andy Thai, Collin Kinchen, David Madden on 10/12/17.
//

#ifndef AWGET_AWGET_H
#define AWGET_AWGET_H

#include <iostream>

using std::cout;
using std::endl;
using std::cin;
using std::cerr;

#include <fstream>

using std::ifstream;

#include <string.h>

using std::string;
using std::stoi;
using std::to_string;

#include <sstream>

using std::stringstream;

#include <vector>

using std::vector;

#include <regex>

using std::regex;
using std::regex_match;

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAX_PACKET_SIZE 1400

struct Stone {
	string addr;
	int port;
};

struct ConInfo {
	char parent[MAX_PACKET_SIZE];
	char url[MAX_PACKET_SIZE];
	char sstones[MAX_PACKET_SIZE];
};

struct ResultGet {
	int seqNum;
	char parent[MAX_PACKET_SIZE];
	char file[MAX_PACKET_SIZE];
};

class Awget {
public:
	vector <Stone> sstones;
	string url;
	void readFile(ifstream &inFile);
	bool isValid(string url);
    void initService();
    void client(char * address, int port, int index);
    string serialize();
};

#endif //AWGET_AWGET_H
