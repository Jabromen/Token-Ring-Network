#include "UDPLib.h"
#include "client.h"
#define BUFFER_SIZE 512


void writeMessage(FILE *messageFile);
void readMessage(FILE *messageFile);
void listMessage(FILE *messageFile);
void leaveBoard(FILE *messageFile);
void writeToFile(FILE *messageFile,char userString[]);
void writeInitialBuffer(char *IP,int Port,char *buffer);

/*
    This function handles initial communication with server, sending
    own hostname and port number then receiving the initial neighbor information
*/
void initialize(struct args *arguments)
{

    fprintf(stderr,"newPeer is [%s]",arguments->newPeer);
    //send host info to server
    fprintf(stderr,"\nIn initialize\n");
    writeInitialBuffer(arguments->hostname,atoi(arguments->thisPort),arguments->buffer);
    fprintf(stderr,"\nafter writebuffer\n");
    
    fprintf(stderr,"\nsending to [%s] on port [%s] with message [%s]",arguments->hostIP,arguments->hostPort,arguments->buffer);
    
    sendMessage(arguments->hostIP,(u_short)atoi(arguments->hostPort),arguments->buffer);
    fprintf(stderr,"\nafter sendmessage\n");
    
    receiveMessage((u_short)atoi(arguments->thisPort),arguments->buffer);
    fprintf(stderr,"\nafter receivemessage\n");
    printf("\nReceived: [%s]\n",arguments->buffer);
}

/*
    This function creates a send buffer with own hostname and port to send to server
*/
void writeInitialBuffer(char *IP,int Port,char *buffer)
{
    int i=0;
    int j=0;
    char PortString[10];

    snprintf(PortString,6,"%d",Port);

    while(IP[i]!='\0')
    {
        buffer[i]=IP[i];
        i++;
    }
    buffer[i]=' ';

    while(PortString[j]!='\0')
    {
        i++;
        buffer[i]=PortString[j];
        j++;
    }
    i++;

    buffer[i]='\0';
}