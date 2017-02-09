#ifndef _UDPLIB_H
#define _UDPLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#define BUFFER_SIZE 512

int sendMessage(char *hostIP,u_short hostport,char * buffer);

int receiveMessage(u_short port,char *buffer);


#endif