#include "UDPLib.h"

/*
    This function sends a UDP message to passed hostIP at hostport of 
    whatever is in the buffer
*/
int sendMessage(char *hostIP,u_short hostport,char * buffer)
{
    
    int sockfd;
    struct sockaddr_in dest;
    sockfd=socket (AF_INET, SOCK_DGRAM,0);
    
    //Testing for valid socket creation
    if(sockfd<0)
    {
        perror("sendUDP:socket");
        return -1;
    }
    
    struct hostent *hostptr=gethostbyname(hostIP);

    //Setting up socket
    memset((void *) &dest, 0, (size_t)sizeof(dest));
    dest.sin_family=(short)(AF_INET);
    
    memcpy((void*)&dest.sin_addr,(void*)hostptr->h_addr,hostptr->h_length);
    dest.sin_port=htons((u_short)hostport);
    
    //Send UDP datagram to server
    sendto(sockfd,buffer,BUFFER_SIZE,0,
            (struct sockaddr*) &dest,(socklen_t)sizeof(dest));
            
    //Zero the buffer to prep for next operation
    bzero(buffer,BUFFER_SIZE);
    
    close(sockfd);
    return 0;
}


/*
    This function receives a UDP message on the passed port then
    places the message in the buffer, as well as prints it to console
*/
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
    
    //Retrieve own hostname
    gethostname(hostname,32);
    
    struct hostent *hostptr=gethostbyname(hostname);
    
    
    //Create listen socket and test for successful creation
    listensockfd=socket (AF_INET, SOCK_DGRAM,0);
    if(listensockfd<0)
    {
        perror("sendUDP:socket");
        return -1;
    }
    
    
    //set up socket
    memset((void *) &servaddr, 0, (size_t)sizeof(servaddr));
    servaddr.sin_family=(short)(AF_INET);
    memcpy((void*)&servaddr.sin_addr,(void*)hostptr->h_addr,hostptr->h_length);
    servaddr.sin_port=htons((u_short)60002);
    
    //bind listen socket and test for successful bind
    bindtest=bind(listensockfd,(struct sockaddr*)&servaddr,(socklen_t)sizeof(servaddr));
    if(bindtest<0)
        fprintf(stderr,"\nError binding socket\n");
        
    //Receive one UDP datagram and store in buffer
    recvfrom(listensockfd,buffer,BUFFER_SIZE,0,
            (struct sockaddr *) &clientAddr,clientAddrLen);
            
    //Print received message to console
    printf("\nReceived Message is %s\n",buffer);
                
    close(listensockfd);
}