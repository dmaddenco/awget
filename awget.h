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

struct stone {
	int addr;
	int port;
};

class Awget {
public:
	stone sstones[];
	void readFile(char *file);
};

#endif //AWGET_AWGET_H
