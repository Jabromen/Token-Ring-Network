#include "UDPLib.h"


int sendMessage(char *hostIP,u_short hostport,char * buffer)
{
    int sockfd;
    struct sockaddr_in dest;
    
    sockfd=socket (AF_INET, SOCK_DGRAM,0);
    if(sockfd<0)
    {
        perror("sendUDP:socket");
        return -1;
    }
    
    struct hostent *hostptr=gethostbyname(hostIP);

    
    memset((void *) &dest, 0, (size_t)sizeof(dest));
    
    dest.sin_family=(short)(AF_INET);
    
    memcpy((void*)&dest.sin_addr,(void*)hostptr->h_addr,hostptr->h_length);
    dest.sin_port=htons((u_short)hostport);
    

    strcpy(buffer,"test send");
    
    printf("\n\nSending %s\n",buffer);
    
    sendto(sockfd,buffer,BUFFER_SIZE,0,
            (struct sockaddr*) &dest,(socklen_t)sizeof(dest));
            
    bzero(buffer,BUFFER_SIZE);
    
    close(sockfd);
    return 0;
}

int receiveMessage(u_short port,char *buffer)
{
    int listensockfd;
    int sockfd;
    int bytesrcvd=0;
    int bindtest;
    char hostname[32];
    struct sockaddr_in servaddr;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    
    gethostname(hostname,32);
    
    struct hostent *hostptr=gethostbyname(hostname);
    
    listensockfd=socket (AF_INET, SOCK_DGRAM,0);
    if(listensockfd<0)
    {
        perror("sendUDP:socket");
        return -1;
    }
    
    //print own hostname
    printf("\nHostname is %s\n",hostname);
    
    //set up socket
    memset((void *) &servaddr, 0, (size_t)sizeof(servaddr));
    servaddr.sin_family=(short)(AF_INET);
    memcpy((void*)&servaddr.sin_addr,(void*)hostptr->h_addr,hostptr->h_length);
    servaddr.sin_port=htons((u_short)60002);
    fprintf(stderr,"\nport is set up\n");
    //bind listen socket
    bindtest=bind(listensockfd,(struct sockaddr*)&servaddr,(socklen_t)sizeof(servaddr));
    if(bindtest<0)
        fprintf(stderr,"\nError binding socket\n");
    else
        fprintf(stderr,"\nbound socket\n");
        
    
        recvfrom(listensockfd,buffer,BUFFER_SIZE,0,
                (struct sockaddr *) &clientAddr,clientAddrLen);

        printf("\nBuffer is %s\n",buffer);
                
    fprintf(stderr,"\nClosing socket,bytesrcvd is %d\n",bytesrcvd);
    close(listensockfd);
}