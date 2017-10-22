//
// Created by David Madden on 10/20/17.
//

#ifndef AWGET_SS_H
#define AWGET_SS_H

#define MAXPENDING 10

char *PORT;
int sock_in, clientSock;
int new_fd;
int sv, n;
fd_set readfds;
struct timeval tv;

#endif //AWGET_SS_H
