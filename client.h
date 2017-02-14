#ifndef _CLIENT_H
#define _CLIENT_H

#include "UDPLib.h"
#define BUFFER_SIZE 512

typedef struct args
{
    char hostname[32];
    char buffer[BUFFER_SIZE];
    char newPeer[32];
    char thisPort[6];
    char hostIP[32];
    char hostPort[6];
    char fileName[32];
} sizes;


void writeToFile(FILE *messageFile,char userString[]);
void writeInitialBuffer(char *IP,int Port,char *buffer);
void initialize(struct args *arguments);


#endif