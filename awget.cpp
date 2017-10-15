//
// Created by Andy Thai, Collin Kinchen, David Madden on 10/12/17.
//

//executed via: $./awget <URL> [-c chainfile]

#include "awget.h"

void Awget::readFile(char *file) {
	ifstream inFile(file);
	string line;
	getline(inFile, line);
	int numStones = stoi(line);
	for (int i = 0; i < numStones; ++i) {
		Stone sstone;
		getline(inFile, line, ' ');
		sstone.addr = line;
		getline(inFile, line, ' ');
		sstone.port = stoi(line);
		sstones.push_back(sstone);
	}
}

int main(int argc, char *argv[]) {
	Awget awget;

	if (argc > 2) {
		if (strcmp(argv[2], "-c") == 0) {
			awget.readFile(argv[3]);
		}
	}
}