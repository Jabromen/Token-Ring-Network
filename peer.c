#include "udpsockets.h"
#include "queue.h"
#include "read_token.h"

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

struct args_t {

	u_short my_port;
	char *hostname;
	u_short dest_port;
	char *filename;

} args_t;

void *uiThread(void *param);
void *networkThread(void *param);

void printMenu();
void writeUserMessage(const char *prompt);

messages_t *message_cache;
queue_t *message_queue;
queue_t *peer_queue;
int running;

int main(int argc, char **argv) {

	if (argc < 5) {
		printf("ERROR: Not enough arguments. Use format:\n"
			   "\"[-new] portNum hostIP hostPort filenameBulletinBoard\"\n");
		exit(EXIT_FAILURE);
	}

	int err;

	int arg_offset = !strcmp(argv[1], "-new") ? 1 : 0;

	struct args_t args;
	args.my_port   = (u_short) atoi(argv[1 + arg_offset]);
	args.hostname  = argv[2 + arg_offset];
	args.dest_port = (u_short) atoi(argv[3 + arg_offset]);
	args.filename  = argv[4 + arg_offset];

	message_cache = initMessagesStruct(1);
	message_queue = initQueue();
	peer_queue = initQueue();

	pthread_t ui_thread;
	pthread_t network_thread;

	running = 1;

	if ((err = pthread_create(&ui_thread, NULL, &uiThread, (void *) &args))) {
		fprintf(stderr, "Can't create UI thread: [%s]\n", strerror(err));
		exit(EXIT_FAILURE);
	}

	if ((err = pthread_create(&network_thread, NULL, &networkThread, (void *) &args))) {
		fprintf(stderr, "Can't create Network Thread: [%s]\n", strerror(err));
		exit(EXIT_FAILURE);
	}

	pthread_join(ui_thread, NULL);
	pthread_join(network_thread, NULL);

	exit(EXIT_SUCCESS);
}

void *uiThread(void *param) {

	int option;
	char ch;

	printMenu();

	while(1) {
		printf("\nSelect an option: ");
		scanf("%d", &option);
		while ((ch = getchar()) != EOF && ch != '\n') {;}
		printf("\n");

		switch(option) {
			case 1:
				writeUserMessage("Enter a message: ");
				break;

			case 2:
				getUserMessage("Enter an index: ", message_cache);
				break;

			case 3:
				printValidIndices(message_cache);
				break;

			case 4:
				running = 0;
				pthread_exit(NULL);
				break;

			default:
				printMenu();
				break;
		}
	}
}

void *networkThread(void *param) {

	struct args_t *args = (struct args_t *) param;

	token_t *tkn = initTokenStruct(args->filename);

	udpsocket_t *sckt = initUdpSocketClient(args->hostname, args->dest_port, args->my_port);

	addrport_t ap;

	int recvlen;
	char *recv_buffer = (char *) malloc(sizeof(char) * 256);
	char *send_buffer = (char *) malloc(sizeof(char) * 256);
	char *tokn_buffer = (char *) malloc(sizeof(char) * 256);
	char *write_buffer = (char *) malloc(sizeof(char) * 512);
	const int bufferSize = 128;

	sendMessage("JOIN", sckt);

	while (running) {

		memset(send_buffer, 0, bufferSize);
		memset(recv_buffer, 0, bufferSize);
		memset(tokn_buffer, 0, bufferSize);

		recvlen = receiveMessage(recv_buffer, bufferSize, sckt);
		
		if (recvlen > 0) {

			if (!strcmp(recv_buffer, "JOIN")) {
				makeAddrString(send_buffer, "PEER", &sckt->myaddr, &sckt->remaddr);
				putQueue(send_buffer, peer_queue);
			}
			else {

				strcpy(tokn_buffer, recv_buffer);

				if (parseMessage(tokn_buffer, &ap)) {
					if (checkDestination(sckt, &ap)) {
						strcpy(send_buffer, "GO");
					}
					else {
						strcpy(send_buffer, recv_buffer);
					}
				}
				else if (!isEmpty(peer_queue)) {
					popQueue(send_buffer, peer_queue);
				}
				else {
					strcpy(send_buffer, "GO");
				}

				readToken(tkn, message_cache);

				while (!isEmpty(message_queue)) {
					memset(write_buffer, 0, 512);
					popQueue(write_buffer, message_queue);
					writeMessage(write_buffer, message_cache->number_of_messages, tkn);
				}

				sendMessage(send_buffer, sckt);
			}
		}
	}

	pthread_exit(NULL);
}

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

	// Write message to token file
	putQueue(message, message_queue);
}

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