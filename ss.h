//
// Created by David Madden on 10/20/17.
//

#ifndef AWGET_SS_H
#define AWGET_SS_H

//#define MAXPENDING 10
//#define BUFSIZE 1024

char *PORT;
int sock_in, clientSock;
int new_fd;
int sv, n;
fd_set readfds;
struct timeval tv;
struct stat file_stat;
off_t offset;
int remain_data;
int sent_bytes = 0;
char file_size[256];
string filename;

#endif //AWGET_SS_H
