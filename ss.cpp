//
// Created by Andy Thai, Collin Kinchen, David Madden on 10/12/17.
//
#include "awget.h"

int main(int argc, char *argv[]) {
//Argument Checking
//Are there too many arguments
if (argc > 3) {cout << "Too many arguments given " << endl; return -1;}
string arg1 = argv[1];
string arg2 = argv[2];
int arg2len = arg2.length();
//Is arg2 -p
if (arg1 != "-p") {cout << "invalid argument" << endl; return -1;}
//Is arg3 a digit
for (int i = 0; i < arg2len; i++){
	if(!isdigit(arg2[i])){cout << "invalid port number" << endl; return -1;}
}
int arg2num = atoi(argv[2]);
//Is arg3 a valid port number
if (arg2num < 1024 || arg2num > 65535){ cout << "Port " << arg2num << " is out of range" << endl; return -1;}

return 0;
}