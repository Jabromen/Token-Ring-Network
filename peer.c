#include "udpsockets.h"
#include "queue.h"
#include "read_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#ifdef __APPLE__
#include <mach/semaphore.h>
#include <mach/task.h>
#define sem_init(a,b,c)     semaphore_create(mach_task_self(), (semaphore_t *)a, SYNC_POLICY_FIFO, c)
#define sem_destroy(a)      semaphore_destroy(mach_task_self(), *((semaphore_t *)a))
#define sem_post(a)         semaphore_signal(*((semaphore_t *)a))
#define sem_wait(a)         semaphore_wait(*((semaphore_t *)a))
#define sem_t               semaphore_t
#else
#include <semaphore.h>
#endif

void *uiThread(void *param);
void *networkThread(void *param);

void printMenu();
void writeUserMessage(const char *prompt);

messages_t *message_cache;
queue_t *message_queue;
queue_t *peer_queue;

sem_t message_queue_lock;

udpsocket_t *sckt;

file_t *file;

int running;
int new;

int main(int argc, char **argv) {

	if (argc < 5) {
		printf("ERROR: Not enough arguments. Use format:\n"
			   "\"[-new] portNum hostIP hostPort filenameBulletinBoard\"\n");
		exit(EXIT_FAILURE);
	}

	int err;

	// Set offset and new flag depending on if "-new" option was entered.
	int arg_offset = !strcmp(argv[1], "-new") ? 1 : 0;
	new = arg_offset;

	u_short my_port   = (u_short) atoi(argv[1 + arg_offset]);
	char *hostname    = argv[2 + arg_offset];
	u_short dest_port = (u_short) atoi(argv[3 + arg_offset]);
	char *filename    = argv[4 + arg_offset];

	// Initialize struct used to handle file IO
	if(!(file = initFileStruct(filename)))
		exit(EXIT_FAILURE);

	// Create UDP socket
	if(!(sckt = initUdpSocketClient(hostname, dest_port, my_port)))
		exit(EXIT_FAILURE);

	// Initialize data structures

	// Holds all cached messages read from bulletin board file
	message_cache = initMessagesStruct(1);
	// Queue for passing messages from UI thread to network thread
	message_queue = initQueue();
	// Queue for handling ring network changes
	peer_queue = initQueue();

	pthread_t ui_thread;
	pthread_t network_thread;

	if(sem_init(&message_queue_lock, 0, 1) != 0) {
		printf("Sem init failed.\n");
		exit(EXIT_FAILURE);
	}

	running = 1;

	if ((err = pthread_create(&network_thread, NULL, &networkThread, NULL))) {
		fprintf(stderr, "Can't create Network Thread: [%s]\n", strerror(err));
		exit(EXIT_FAILURE);
	}

	if ((err = pthread_create(&ui_thread, NULL, &uiThread, NULL))) {
		fprintf(stderr, "Can't create UI thread: [%s]\n", strerror(err));
		exit(EXIT_FAILURE);
	}

	// Wait until threads exit
	pthread_join(ui_thread, NULL);
	pthread_join(network_thread, NULL);

	// Free allocated memory
	freeMessages(message_cache);
	freeQueue(message_queue);
	freeQueue(peer_queue);
	freeFile(file);

	sem_destroy(&message_queue_lock);

	// Close socket
	closeSocket(sckt);

	exit(EXIT_SUCCESS);
}

/*
	The purpose of this function is to run a thread to display
	and receive input from the user on the menu, then call the appropriate
	function to run that menu option.
	
	Takes a null pointer to an args struct to pass in the command line
	arguments
	
	Calls writeUserMessage, getUserMessage, printValidIndices
*/
void *uiThread(void *param) {

	int option;
	char ch;

	printMenu();

	while(1) {
		printf("\nSelect an option: ");
		scanf("%d", &option);
		// Prevents scanf input from bleeding over into its next call
		while ((ch = getchar()) != EOF && ch != '\n') {;}
		printf("\n");

		switch(option) {
			case 1:
				writeUserMessage("Enter a message:\n");
				break;

			case 2:
				getUserMessage("Enter an index: ", message_cache);
				break;

			case 3:
				printValidIndices(message_cache);
				break;

			case 4:
				// Signal network thread to shutdown by setting running flag to 0
				running = 0;
				pthread_exit(NULL);
				break;

			default:
				printMenu();
				break;
		}
	}
}

/*
	The purpose of this function is to run a thread to handle initial
	setup of the ring, or add to the ring then initialize and pass the token
	around the ring
	
	Takes a null pointer to an args struct to pass in the command line
	arguments
	
	Calls initUdpSocketClient, sendMessage, initToknMessageStruct,
	clearToknMessage, receiveMessage, tokenizeMessage, parseMessage,
	checkDestination, makeAddrString, compareAddresses, putQueue, popQueue,
	isEmpty
*/

void *networkThread(void *param) {

	// Struct used to hold address and port values for comparison
	addrport_t ap;

	// Flag indicating if exiting
	int exiting = 0;

	// Current number of messages in cache
	int numMessages;

	// Used to hold address/port comparison result
	int compAddr;

	// Number of bytes received
	int recvlen;

	// Buffers used to send and receive network messages
	char *recv_buffer = (char *) malloc(sizeof(char) * NETWORK_BUFF_SIZE);
	char *send_buffer = (char *) malloc(sizeof(char) * NETWORK_BUFF_SIZE);

	// Buffer used to write to bulletin board file
	char *write_buffer = (char *) malloc(sizeof(char) * MAX_MESSAGE_SIZE);

	// Struct that holds tokenized received network message
	tokn_message_t *tokn_message = initToknMessageStruct();

	// Send initial join request to server (if "-new" option was entered) or peer
	sendMessage("JOIN", sckt);

	while (1) {
		// Clear buffers
		memset(send_buffer, 0, NETWORK_BUFF_SIZE);
		memset(recv_buffer, 0, NETWORK_BUFF_SIZE);
		clearToknMessage(tokn_message);

		// Receive network message
		recvlen = receiveMessage(recv_buffer, NETWORK_BUFF_SIZE, sckt);
		
		if (recvlen > 0) {

			// Tokenize received message
			if (tokenizeMessage(recv_buffer, tokn_message))
				// Drop message if received too many arguments
				continue;

			// Case for initial peer assignment
			if (!strcmp(tokn_message->argv[0], "INIT-PEER") && new) {
				// Parse message and store addresses and ports into ap struct.
				if (parseMessage(tokn_message, &ap)) {
					// Apply address and port changes.
					checkDestination(sckt, &ap);
					// Generate message used for deciding who creates the first token.
					makeAddrString(send_buffer, "INIT-GO", &sckt->myaddr, &sckt->remaddr);
					sendMessage(send_buffer, sckt);
				}
			}
			// Case for initial token creation
			else if (!strcmp(tokn_message->argv[0], "INIT-GO") && new) {
				// Parse message and store address and port into ap struct
				if (parseMessage(tokn_message, &ap)) {
					// Result of comparing own address/port with received address/port
					compAddr = compareAddresses(sckt, &ap);

					// If own address/port is lower, send own address/port
					if (compAddr < 0)
						makeAddrString(send_buffer, "INIT-GO", &sckt->myaddr, &sckt->remaddr);
					// If own address/port is higher, send received address/port
					else if (compAddr > 0)
						strcpy(send_buffer, recv_buffer);
					// If received own address/port, create and send token.
					else {
						strcpy(send_buffer, "GO");
						// Set new flag to 0
						new = 0;
					}

					sendMessage(send_buffer, sckt);
				}
			}
			// Case for receiving a join request
			else if (!strcmp(tokn_message->argv[0], "JOIN")) {
				// Put address/port of joining peer into queue to be sent later.
				makeAddrString(send_buffer, "PEER", &sckt->myaddr, &sckt->remaddr);
				putQueue(send_buffer, peer_queue);
			}
			// Case for receiving token
			else if (!strcmp(tokn_message->argv[0], "GO") || !strcmp(tokn_message->argv[0], "PEER")) {
				new = 0;
				// Parse message to check if a change to the ring is made
				if (parseMessage(tokn_message, &ap)) {
					// Check if destination address needs to be changed
					if (checkDestination(sckt, &ap)) {
						// Send default token message
						strcpy(send_buffer, "GO");
					}
					else {
						// Else forward ring change to next peer
						strcpy(send_buffer, recv_buffer);
					}
				}
				// If no other change is being made to ring, and a change is queued up, send it to next peer.
				else if (!isEmpty(peer_queue)) {
					popQueue(send_buffer, peer_queue);
				}
				// Else send default token message
				else {
					strcpy(send_buffer, "GO");
				}

				// Update cache 
				readFile(file, message_cache);

				numMessages = message_cache->number_of_messages;

				sem_wait(&message_queue_lock);

				// Write all queued messages to bulletin board file
				while (!isEmpty(message_queue)) {
					memset(write_buffer, 0, MAX_MESSAGE_SIZE);
					popQueue(write_buffer, message_queue);
					writeMessage(write_buffer, ++numMessages, file);
				}

				sem_post(&message_queue_lock);

				sendMessage(send_buffer, sckt);

				// Stop listening for new messages if exiting
				if (exiting && isEmpty(peer_queue))
					break;
			}
		}

		// If signaled to exit from UI thread, enqueue peer change and set exiting flag
		if (!running) {
			makeAddrString(send_buffer, "PEER", &sckt->myaddr, &sckt->destaddr);
			putQueue(send_buffer, peer_queue);
			exiting = 1;
		}
	}

	// Free allocated memory

	free(send_buffer);
	free(recv_buffer);
	free(write_buffer);

	freeToknMessage(tokn_message);

	// Close thread
	pthread_exit(NULL);
}


/*
	The purpose of this function is to run display a prompt to the user,
	then receive the message and place it into the queue.
	
	Takes a string passed in as the prompt to display
	
	Calls putQueue
*/

void writeUserMessage(const char *prompt) {

	char message[MAX_MESSAGE_SIZE] = "";
	char line[BUFFER_SIZE] = "";

	// Prompt user for message
	printf("%s", prompt);

	// Read user message from stdin
	while (1) {
		fgets(line, BUFFER_SIZE, stdin);
		if (!strcmp(line, "\n")) {
			break;
		}
		strcat(message, line);
		memset(line, 0, strlen(line));
	}

	// If no message was entered, don't save it.
	if (!strcmp(message, "\0"))
		return;

	sem_wait(&message_queue_lock);
	// Write message to token file
	putQueue(message, message_queue);

	sem_post(&message_queue_lock);
}


/*
	The purpose of this function is to display the menu to the user
*/

void printMenu() {
    printf("    Please select an action by entering the number by the action\n\n"
           "    Action         Description\n\n"
           "1)  Write          Appends a new message to the end of board\n\n"
           "2)  Read #         Read a particular message from the message\n"
           "                   board using a sequence number. # is the \n"
           "                   sequence number of the message on the board\n\n"
           "3)  List           Displays the range of valid sequence numbers\n"
           "                   posted to the board\n\n"
           "4)  Exit           Closes the message board\n");
}