#ifndef _UDPLIB_H
#define _UDPLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#define BUFFER_SIZE 512


/*
    This function sends a UDP message to passed hostIP at hostport of 
    whatever is in the buffer
*/
int sendMessage(char *hostIP,u_short hostport,char * buffer);

/*
    This function receives a UDP message on the passed port then
    places the message in the buffer, as well as prints it to console
*/
int receiveMessage(u_short port,char *buffer);


#endif