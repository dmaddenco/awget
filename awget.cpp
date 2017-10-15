//
// Created by Andy Thai, Collin Kinchen, David Madden on 10/12/17.
//

//executed via: $./awget <URL> [-c chaingang.txt]

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

//Check if URL is valid
bool isValid(string url) {
	string urlPattern = "^((https?://)|^(www\\.))[^/\n]+(?:/[^\\/%\n]+)*(?:/?\?[^&\n]+(?:&[^&\n]+)*)?/?$";
	regex reg(urlPattern);

	if (regex_match(url, reg) == true) {
		return true;
	} else {
		return false;
	}
}

int main(int argc, char *argv[]) {
	Awget awget;

	//if only URL is specified
	if (argc == 2) {
		string url(argv[1]);
		if (isValid(url)) {
			//cout << "URL VALID" << endl;
		} else {
			cerr << "URL INVALID" << endl;
			exit(EXIT_FAILURE);
		}

		//Read from local file "chaingang.txt"
		ifstream reader("chaingang.txt");
		if (reader.good()) {
			//file is valid
			//cout << "FILE READABLE" << endl;
		} else {
			cerr << "FILE COULD NOT BE REACHED/INVALID" << endl;
			exit(EXIT_FAILURE);
		}
		reader.close();
	}
		//if URL and chaingang file given
	else if (argc == 3) {
		string url(argv[1]);
		if (isValid(url)) {
			//cout << "URL VALID" << endl;
		} else {
			cerr << "URL INVALID" << endl;
			exit(EXIT_FAILURE);
		}

		ifstream reader(argv[2]);
		if (reader.good()) {
			//cout << "FILE READABLE" << endl;
		} else {
			cerr << "FILE COULD NOT BE REACHED/INVALID" << endl;
			exit(EXIT_FAILURE);
		}
		reader.close();
	} else {
		cerr << "DID NOT RECEIVE CORRECT ARGUMENTS" << endl;
		exit(EXIT_FAILURE);
	}
}
