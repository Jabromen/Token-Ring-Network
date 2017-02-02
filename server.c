#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#define BUFFER_SIZE 512


void writeMessage(FILE *messageFile);
void readMessage(FILE *messageFile);
void listMessage(FILE *messageFile);
void leaveBoard(FILE *messageFile);
void writeToFile(FILE *messageFile,char userString[]);


int main(int argc, char** argv)
{
    char hostname[32];
    int listensockfd;
    struct sockaddr_in servaddr;
    listensockfd=socket (AF_INET, SOCK_DGRAM,0);
    if(listensockfd<0)
    {
        perror("sendUDP:socket");
        return -1;
    }
    gethostname(hostname,32);
    struct hostent *hostptr=gethostbyname(hostname);
    printf("\nHostname is %s\n",hostname);
    
    memset((void *) &servaddr, 0, (size_t)sizeof(servaddr));
    servaddr.sin_family=(short)(AF_INET);
    memcpy((void*)&servaddr.sin_addr,(void*)hostptr->h_addr,hostptr->h_length);
    servaddr.sin_port=htons((u_short)60000);
    
    bind(listensockfd,(struct sockaddr*)&servaddr,(socklen_t)sizeof(servaddr));
    char buffer[BUFFER_SIZE];
    int bytesRcvd;
    
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
   // for(;;)
    {
        bytesRcvd=recvfrom(listensockfd,buffer,BUFFER_SIZE,0,
                (struct sockaddr *) &clientAddr,clientAddrLen);
        printf("\nBuffer is %s\n",buffer);
        sendto(listensockfd,buffer,BUFFER_SIZE,0,
                 (struct sockaddr *) &clientAddr,clientAddrLen);
    }
    close (listensockfd);
    
    
}