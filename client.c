#include "UDPLib.h"
#define BUFFER_SIZE 512


void writeMessage(FILE *messageFile);
void readMessage(FILE *messageFile);
void listMessage(FILE *messageFile);
void leaveBoard(FILE *messageFile);
void writeToFile(FILE *messageFile,char userString[]);
void writeInitialBuffer(char *IP,int Port,char *buffer);


int main(int argc, char** argv)
{

    char hostname[32];
    
    char buffer[BUFFER_SIZE];
    
    char newPeer[32];
    char thisPort[6];
    char hostIP[32];
    char hostPort[6];
    char fileName[32];
    fprintf(stderr,"\nbefore strcpy of arg1\n");
    strcpy(newPeer,argv[1]);
    fprintf(stderr,"\nbefore strcpy of arg2\n");
    strcpy(thisPort,argv[2]);
    fprintf(stderr,"\nbefore strcpy of arg3\n");
    strcpy(hostIP,argv[3]);
    fprintf(stderr,"\nbefore strcpy of arg4\n");
    strcpy(hostPort,argv[4]);
    fprintf(stderr,"\nbefore strcpy of arg5\n");
    strcpy(fileName,argv[5]);
    fprintf(stderr,"\nafter strcpy of arguments\n");
    gethostname(hostname,32);
    
    //send host info to server
    writeInitialBuffer(hostname,atoi(thisPort),buffer);
    fprintf(stderr,"\nafter writebuffer\n");
    sendMessage(hostIP,(u_short)atoi(hostPort),buffer);
    fprintf(stderr,"\nafter sendmessage\n");
    
    receiveMessage((u_short)atoi(thisPort),buffer);
    fprintf(stderr,"\nafter receivemessage\n");
    printf("\nReceived: [%s]\n",buffer);
    
}

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