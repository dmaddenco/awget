all: onion

onion: awget.cpp awget.h ss.cpp
	g++ -I. -Wall -std=c++11 -g ss.cpp -o ss
	g++ -I. -Wall -std=c++11 -g awget.cpp -o awget
clean:
	rm -f *.o *.tar ss awget
tar:
	tar -cvf onion.tar awget.cpp awget.h ss.cpp makefile README.txt
