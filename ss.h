//
// Created by David Madden on 10/20/17.
//

#ifndef AWGET_SS_H
#define AWGET_SS_H

#define MAXDATASIZE 100
#define MAXPENDING 10

char *PORT;
int sock_in;
int new_fd;
int sv, n;
fd_set readfds;
struct timeval tv;
char buf1[256], buf2[256];

#endif //AWGET_SS_H
