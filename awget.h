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

struct Stone {
	string addr;
	int port;
};

struct ConInfo {
	string parent;
	string child;
	string url;
	vector<Stone> sstones;
};

class Awget {
public:
	vector <Stone> sstones;
	string url;
	void readFile(ifstream &inFile);
	bool isValid(string url);
    void initService();
    void client(char * address, int port, int index);
    
};

#endif //AWGET_AWGET_H
