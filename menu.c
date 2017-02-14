#include "menu.h"


int menu()
{
    int menuSelection=-1;
    char menuString[500];
    FILE *messageFile;
    
    
    
    while(menuSelection!=4)
    {
        printf("\n    Please select an action by entering the number by the action\n\n");
        printf("    Action         Description\n\n");
        printf("1)  Write          Appends a new message to the end of board\n\n");
        printf("2)  Read #         Read a particular message from the message\n");
        printf("                   board using a sequence number. # is the \n");
        printf("                   sequence number of the message on the board\n\n");
        printf("3)  List           Displays the range of valid sequence numbers\n");
        printf("                   posted to the board\n\n");
        printf("4)  Exit           Closes the message board\n");
        printf("\n\nWhat is your selection? ");
        fgets(menuString,500,stdin);
        menuSelection=atoi(menuString);
        switch(menuSelection)
        {
            case 1:
                writeMessage(messageFile);
                break;
            case 2:
                readMessage(messageFile);
                break;
            case 3:
                listMessage(messageFile);
                break;
            case 4:
                return 1;
            default:
                menuSelection=-1;
                
        }
    }
}

int getCurrentMessageNumber(FILE *messageFile)
{
    sem_t *tokenLock = sem_open("tokenLock", 0);
    char testStr[600];
    char EOMStr[15]="</message>\n";
    sem_wait(tokenLock);
    messageFile= fopen("messages.txt","r");
    int messageCount=0;
    do
    {
        fgets(testStr,600,messageFile);
        if(strcmp(testStr,EOMStr)==0)
        {
            messageCount++;
        }
    } while (!feof(messageFile));
    fclose(messageFile);
    sem_post(tokenLock);
    if(messageCount==0)
        messageCount=1;
    return messageCount;
}

void writeMessage(FILE *messageFile)
{
    char *userString=malloc(500);
    printf("\nPlease enter your message.  The maximum message size is 500 characters\n\n");
    fgets(userString,500,stdin);
    
    printf("\n in Writemessage, message is %s\n",userString);
    
    writeToFile(messageFile,userString);
    
}

void writeToFile(FILE *messageFile,char userString[])
{
    sem_t *tokenLock = sem_open("tokenLock", 0);
    //wait for token
    sem_wait(tokenLock);
    int num=getCurrentMessageNumber(messageFile);
    printf("\nnum is %d\n",num);
    messageFile= fopen("messages.txt","a");
    fprintf(messageFile,"<message n=%d>\n",num);
    fprintf(messageFile,"%s",userString);
    fprintf(messageFile,"</message>\n");
    fflush(messageFile);
    fclose(messageFile);
    sem_post(tokenLock);
}


void readMessage(FILE *messageFile)
{
    char *inputStr=malloc(500);
    char *testStr=malloc(500);
    int messageNum=0;
    char c;
    int i=0;
    int msgdone=0;
    
    sem_t *tokenLock = sem_open("tokenLock", 0);
    
    int currentMessages=getCurrentMessageNumber(messageFile)-1;
    
    
    while((messageNum<=0)||(messageNum>currentMessages))
    {
        printf("\nWhich message would you like to read? \n\n");
        fgets(inputStr,500,stdin);
        messageNum=atoi(inputStr);
    }
    
    while(inputStr[i]!='\0')
    {
        if(inputStr[i]=='\n')
            inputStr[i]='\0';
        i++;
    }
    strcpy(testStr,"<message n=");
    strcat(testStr,inputStr);
    strcpy(inputStr,">\n");
    strcat(testStr,inputStr);
    sem_wait(tokenLock);
    messageFile=fopen("messages.txt","r");
    do
    {
        fgets(inputStr,500,messageFile);
        
    } while (strcmp(testStr,inputStr)!=0);
    
    strcpy(testStr,"</message>\n");
    printf("\nMessage %d is:\n[",messageNum);
    do
    {
        fgets(inputStr,500,messageFile);
        if(strcmp(inputStr,testStr)!=0)
        {
            printf("%s",inputStr);
        }
        else(msgdone=1);
    } while (msgdone==0);
    printf("]\n");
    fclose(messageFile);
    sem_post(tokenLock);
}


void listMessage(FILE *messageFile)
{
    int numMessages=getCurrentMessageNumber(messageFile);
    printf("The current range of messages is 1 to %d",numMessages-1);
}