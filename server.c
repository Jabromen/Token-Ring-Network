#include "UDPLib.h"
#define BUFFER_SIZE 512
#define MAXHOSTS 20


void writeMessage(FILE *messageFile);
void readMessage(FILE *messageFile);
void listMessage(FILE *messageFile);
void leaveBoard(FILE *messageFile);
void writeToFile(FILE *messageFile,char userString[]);

int getPort(char *tokenString);
void getIP(char *tokenString,char IP[32]);
void writeBuffer(char *neighborIP,int neighborPort,char *buffer);


int main(int argc, char** argv)
{
    
    
    int i;
    
    int numHosts;
    char hostsIP[32][MAXHOSTS];
    u_short  hostsPort[5][MAXHOSTS];
    char IP[BUFFER_SIZE];
    char neighborIP[32];
    char sendIP[32];
    int Port;
    int sendPort;
    int neighborPort;
    char buffer[BUFFER_SIZE];

    numHosts=atoi(argv[2]);
    
    //get hotnames and ports from clients, however many were passed
    for(i=0;i<numHosts;i++)
    {
        receiveMessage(60002,buffer);
        getIP(buffer,hostsIP[i]);
        *hostsPort[i]=getPort(buffer);
    }
    
    for(i=0;i<numHosts;i++)
    {
        printf("\nHost %d is %s\n",i,hostsIP[i]);
    }
    
    //send neighbor info to hosts
    for(i=0;i<numHosts-1;i++)
    {
        
        fprintf(stderr,"\ni is %d",i);
        //get IP and port to send to
        strcpy(neighborIP,hostsIP[i+1]);
        neighborPort=*hostsPort[i+1];
        
        strcpy(sendIP,hostsIP[i]);
        sendPort=*hostsPort[i];
     //   printf(stderr,"\nIP is %s, sendIP is %s",hostsIP[i],sendIP);
     //   fprintf(stderr,"\nIP is %s, neighborIP is %s",hostsIP[i+1],neighborIP);
        writeBuffer(neighborIP,neighborPort,buffer);
        fprintf(stderr,"\nSending [%s]\n",buffer);
        sendMessage(sendIP,sendPort,buffer);
    }
    
    //send info to last client (neighbor info is first host)
    strcpy(neighborIP,hostsIP[0]);
    neighborPort=*hostsPort[0];
    
    strcpy(sendIP,hostsIP[i]);
    sendPort=*hostsPort[i];
  //  fprintf(stderr,"\nIP is %s, neighborIP is %s",hostsIP[0],neighborIP);
    writeBuffer(neighborIP,neighborPort,buffer);
 //   fprintf(stderr,"\nSending [%s]\n",buffer);
    sendMessage(sendIP,sendPort,buffer);
    
 //   for(i=0;i<numHosts;i++)
 //   {
 //       printf("\nHost %d is %s\n",i,hostsIP[i]);
 //   }
}

/*
    Pull the IP address/hostname out of the received message
*/
void getIP(char *tokenString, char IP[16])
{
    int i=0;
    printf("\n");
    while(tokenString[i]!=' ')
    {
        IP[i]=tokenString[i];
        i++;
    }
    IP[i]='\0';
}

/*
    Pull the port number out of the received message, then return it as an int
*/
int getPort(char *tokenString)
{
    int port;
    int i=0;
    int j=0;
    
    char tempString[5];
    
    while(tokenString[i]!=' ')
    {
        i++;
    }

    while(tokenString[i]!='\0')
    {

        tempString[j]=tokenString[i];
        j++;
        i++;
    }

    port=atoi(tempString);

    return port;
}


/*
    This function writes the passed IP address and port into the buffer
    to format for sending
*/
void writeBuffer(char *neighborIP,int neighborPort,char *buffer)
{
 //   fprintf(stderr,"\n\nIn writebuffer\n");
    int i=0;
    int j=0;
    char neighborPortString[10];

    snprintf(neighborPortString,6,"%d",neighborPort);

    while(neighborIP[i]!='\0')
    {
//        fprintf(stderr,"\nWriting %c to buffer",neighborIP[i]);
        buffer[i]=neighborIP[i];
        i++;
    }
    buffer[i]=' ';

    while(neighborPortString[j]!='\0')
    {
        i++;
 //       fprintf(stderr,"\nWriting %c to buffer",neighborPortString[i]);
        buffer[i]=neighborPortString[j];
        j++;
    }
    i++;

    buffer[i]='\0';
}