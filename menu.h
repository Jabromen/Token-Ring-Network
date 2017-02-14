#ifndef _MENU_H
#define _MENU_H
#include <semaphore.h>



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int menu();
void writeMessage(FILE *messageFile);
void readMessage(FILE *messageFile);
void listMessage(FILE *messageFile);
void leaveBoard(FILE *messageFile);
void writeToFile(FILE *messageFile,char userString[]);


#endif