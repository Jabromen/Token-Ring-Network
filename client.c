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
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in dest;
//    struct hostent *hostptr;
    printf("\nIP is %s\n",argv[1]);
    
    sockfd=socket (AF_INET, SOCK_DGRAM,0);
    if(sockfd<0)
    {
        perror("sendUDP:socket");
        return -1;
    }
    struct hostent *hostptr=gethostbyname(argv[1]);


    fprintf(stderr,"\nStarting memset\n");
    
    memset((void *) &dest, 0, (size_t)sizeof(dest));
    
    dest.sin_family=(short)(AF_INET);
    
    memcpy((void*)&dest.sin_addr,(void*)hostptr->h_addr,hostptr->h_length);
    dest.sin_port=htons((u_short)60000);
    
    fprintf(stderr,"\ngetting message\n");
    printf("\nWhat is the message? ");
    fgets(buffer,500,stdin);
    printf("\n\nSending %s\n",buffer);
    sendto(sockfd,buffer,BUFFER_SIZE,0,
            (struct sockaddr*) &dest,(socklen_t)sizeof(dest));
            
    bzero(buffer,BUFFER_SIZE);
    recvfrom(sockfd,buffer,BUFFER_SIZE,0,NULL,NULL);
    printf("\nReceived %s\n",buffer);
    close(sockfd);
    
    
}