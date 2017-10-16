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
		getline(inFile, line, ' ');
		sstone.addr = line;
		getline(inFile, line, ' ');
		sstone.port = stoi(line);
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

vector<Stone> Awget::getSstones(){
    return sstones;
}

bool Awget::isEmpty(vector<Stone> sstones){
    if(sstones.empty()){
        return true;
    }
    else{
        return false;
    }
}

Stone Awget::findRandStone(vector<Stone> sstones){
    int random;
    int size = sstones.size();
    Stone temp;
    //seed the rand()
    srand(time(NULL));
    random = rand() % size;
    temp = sstones[random];
    
    //remove stone from vector
    sstones.erase(sstones.begin() + random);
    return temp;
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
	
}
