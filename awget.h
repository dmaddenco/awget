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

class Awget {
public:
	vector <Stone> sstones;

	void readFile(ifstream &inFile);
	bool isValid(string url);
    vector<Stone> getSstones();
    bool isEmpty(vector<Stone> sstones);
    Stone findRandStone(vector<Stone> sstones);
};

#endif //AWGET_AWGET_H
