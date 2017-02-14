#include "menu.h"
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <mach/task.h>
#include <semaphore.h>
#define sem_init(a,b,c)     semaphore_create(mach_task_self(), (semaphore_t *)a, SYNC_POLICY_FIFO, c)
#define sem_destroy(a)      semaphore_destroy(mach_task_self(), *((semaphore_t *)a))
#define sem_post(a)         semaphore_signal(*((semaphore_t *)a))
#define sem_wait(a)         semaphore_wait(*((semaphore_t *)a))
#define sem_t               semaphore_t
#define SNAME "tokenLock"


void initializeArray();
int computeTime();
void printArray();
void createThreads(int threadcount, pthread_t threads[],struct args *arguments);
void *threadCompute(void *);
void joinThreads(int threadCount,pthread_t threads[]);
void printTime(struct timespec begin,struct timespec end,int n,int threadCount);
void writeArgs(struct args *arguments,char **argv);
void waitForMessage(char *buffer);
int exitCode=0;





int main(int argc, char** argv){
    int threadCount = 2;
    struct args *arguments;
    arguments=malloc(sizeof(struct args));
    
    sem_t *sem = sem_open(SNAME, O_CREAT, 0644, 0);
    
    writeArgs(arguments,argv);
    fprintf(stderr,"newPeer is [%s]",arguments->newPeer);
    
    pthread_t threads[threadCount];
    
    createThreads(threadCount,threads,arguments);
    joinThreads(threadCount,threads);

    sem_destroy(&tokenLock);
    return 0;
}

void writeArgs(struct args *arguments,char **argv)
{

    fprintf(stderr,"\nIn writeArgs before newPeer");
    strcpy(arguments->newPeer,argv[1]);
    fprintf(stderr,"\nIn writeArgs before thisPort,newPeer is [%s]",arguments->newPeer);
    strcpy(arguments->thisPort,argv[2]);
    fprintf(stderr,"\nIn writeArgs before hostIP,thisPort is [%s]",arguments->thisPort);
    strcpy(arguments->hostIP,argv[3]);
    fprintf(stderr,"\nIn writeArgs before hostPort");
    strcpy(arguments->hostPort,argv[4]);
    fprintf(stderr,"\nIn writeArgs before fileName");
    strcpy(arguments->fileName,argv[5]);
    fprintf(stderr,"\nIn writeArgs before hostName");
    gethostname(arguments->hostname,32);
    fprintf(stderr,"\nleaving writeArgs\n");
    
}

void *menuthread(void *nptr)
{
    while(exitCode==0)
    {
	    exitCode=menu();
    }

}

void *tokenthread(void *nptr)
{
    struct args *tokenArgs = (struct args*)nptr;
    fprintf(stderr,"\nIn tokenThread, newPeer is [%s]",tokenArgs->newPeer);
	initialize(tokenArgs);
	fprintf(stderr,"newPeer is %s\nhostIP is %s\nfileName is %s\n",tokenArgs->newPeer,tokenArgs->hostIP,tokenArgs->fileName);
	while(exitCode==0)
	{
	    waitForMessage(tokenArgs->buffer);
	    checkIfWinner(buffer);
	}
}


void createThreads(int threadCount, pthread_t threads[],struct args *arguments)
{
	
	pthread_create(&threads[0],NULL,menuthread,(void *)arguments);
	pthread_create(&threads[0],NULL,tokenthread,(void *)arguments);

}


void joinThreads(int threadCount,pthread_t threads[])
{
    int i=0;
    for (i=0;i<threadCount;i++)
    {
        pthread_join(threads[0],NULL);
    }
}

void waitForMessage(char *buffer,struct args *arguments)
{
    receiveMessage((u_short)atoi(arguments->thisPort),arguments->buffer)
}
