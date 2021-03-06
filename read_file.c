#include "read_file.h"


/*
	The purpose of this function is to initialize the file_t structure
	
	Takes pointer to the filename of the bulletin board
	
	Returns file_t pointer
	
*/
file_t *initFileStruct(const char *filename) {

	// Allocate memory for file struct
	file_t *newFile = (file_t*) malloc(sizeof(file_t));

	if (!newFile) {
		return NULL;
	}

	// Allocate memory for filename string
	int filename_length = strlen(filename) + 1;
	newFile->filename = (char*) malloc(filename_length * sizeof(char));

	if (!newFile->filename) {
		free(newFile);
		return NULL;
	}

	// Set filename string and initial file position
	strcpy(newFile->filename, filename);
	newFile->position = 0;

	return newFile;
}


/*
	The purpose of this function is to initialize the messages structure
	
	Takes integer length
	
	Returns message pointer
	
*/
messages_t *initMessagesStruct(int length) {

	// If length is invalid, default to a length of 1
	int len = 1;
	if (length > 0) {
		len = length;
	}

	// Allocate memory for messages struct
	messages_t *newMessages = (messages_t*) malloc(sizeof(messages_t));

	if (newMessages == NULL) {
		return NULL;
	}

	// Set initial array length and number of messages
	newMessages->length = len;
	newMessages->number_of_messages = 0;

	// Allocate memory for message array
	newMessages->messages = (char**) malloc(len * sizeof(char*));

	if (newMessages->messages == NULL) {
		free(newMessages);
		return NULL;
	}

	return newMessages;
}


/*
	The purpose of this function is extract the message out of the
	message struct
	
	Takes index and message struct pointer
	
	Returns message string
	
*/
char *getMessage(int index, messages_t *msg) {

	// If index is outside of the range, return a NULL string
	if (index >= msg->number_of_messages || index < 0) {
		return "\0";
	}

	return msg->messages[index];
}


/*
	The purpose of this function is obtain and display the current valid 
	message	index
	
	Takes message struct pointer
	
*/
void printValidIndices(messages_t *msg) {

	int num_messages = msg->number_of_messages;

	if (num_messages <= 0) {
		printf("No messages.\n");
	}
	else {
		printf("Valid indices: 1 to %d\n", num_messages);
	}
}


/*
	The purpose of this function is retrieve a message and display it for the
	user
	
	Takes prompt to display and message struct pointer
	
*/
void getUserMessage(const char *prompt, messages_t *msg) {

	int index;
	char ch;
	char message[MAX_MESSAGE_SIZE] = "";

	if (msg->number_of_messages <= 0) {
		printf("No messages.\n");
		return;
	}

	// Prompt user to enter an index
	printf("%s", prompt);

	scanf("%d", &index);
	while ((ch = getchar()) != EOF && ch != '\n') {;}

	// Get message from index
	strcpy(message, getMessage(index - 1, msg));

	// Print error if an invalid index was entered
	if (message[0] == '\0') {
		printf("ERROR: Invalid index.\n");
	}
	// Print message if a valid index was entered
	else {
		printf("--------------------------------------\n");
		printf("%s", message);
		printf("--------------------------------------\n");
	}
}


/*
	The purpose of this function is update the messages cache on receipt
	of the token.
	
	Takes file struct pointer and message struct pointer
	
	Calls addMessage
	
	Returns int boolean to error check
	
*/
int readFile(file_t *file, messages_t *msg) {

	// Open bulletin board file
	FILE *fp = fopen(file->filename, "r");

	if (fp == NULL) {
		return 1;
	}

	// Buffer used to read lines from file
	char line[BUFFER_SIZE];
	// Buffer used to store message before caching
	char message[MAX_MESSAGE_SIZE] = "";
	// Flag set if currently reading message lines
	int isMessage = 0;


	// Seek to last read position in file
	fseek(fp, file->position, SEEK_SET);

	// Read everything that has been appended to file since last read
	while (fgets(line, BUFFER_SIZE, fp) != NULL) {
		// If first char in a line is '<' then parse the line
		if (line[0] == '<') {
			char *token;

			token = strtok(line, DELIM);

			// Start of a message
			if(!strcmp(token, "message")) {
				isMessage = 1;
			}
			// End of a message
			else if (!strcmp(token, "/message")) {
				// Cache the current message buffer
				addMessage(message, msg);
				// Clear the message buffer
				memset(message, 0, strlen(message));
				isMessage = 0;
			}

		}
		// While reading message lines, store lines in message buffer
		else if (isMessage) {
			strcat(message, line);
		}
	}
	// Update the last read position in file
	file->position = ftell(fp);

	fclose(fp);

	return 0;
}


/*
	The purpose of this function is to grow the messages cache
	
	Takes message struct pointer
	
	Returns int boolean to error check
	
*/
int increaseNumMessages(messages_t *msg) {

	// Double the message array length
	msg->length *= 2;
	char **newMem = (char**) realloc(msg->messages, msg->length * sizeof(char*));

	if (newMem == NULL) {
		freeMessages(msg);
		return 1;
	}

	msg->messages = newMem;

	return 0;
}


/*
	The purpose of this function is to add a message to the message struct
	
	Takes message pointer and message struct pointer
	
	Calls increaseNumMessages
	
	Returns int boolean to error check
	
*/
int addMessage(const char *message, messages_t *msg) {

	// Index where the new message will be cached at.
	int index = msg->number_of_messages;

	// If index is outside of the range, increase the length of the message array
	if (index >= msg->length) {
		if (increaseNumMessages(msg)) {
			return 1;
		}
	}

	// Allocate memory for message string
	int message_size = strlen(message) + 1;
	msg->messages[index] = (char*) malloc(message_size * sizeof(char));
	
	if (msg->messages[index] == NULL) {
		return 1;
	}

	// Copy message string into cache
	strcpy(msg->messages[index], message);

	// Update the number of messages
	msg->number_of_messages++;

	return 0;
}


/*
	The purpose of this function is to write a message to the bulletin
	board file
	
	Takes message pointer, message number int, and file struct pointer
	
	Returns int boolean to error check
	
*/
int writeMessage(const char *message, int num, file_t *file) {

	// Open bulletin board file to append message
	FILE *fp = fopen(file->filename, "a");

	if (fp == NULL) {
		return 1;
	}

	// Write message to bulletin board file with header and footer included
	fprintf(fp, "<message n=%d>\n", num);
	fputs(message, fp);
	fputs("</message>\n", fp);

	fclose(fp);

	return 0;
}


/*
	The purpose of this function is to free memory used by the file struct
	to avoid memory leaks
	
	Takes file struct pointer
	
*/
void freeFile(file_t *file) {

	// Free memory allocated to filename string
	free(file->filename);
	// Free struct
	free(file);
}


/*
	The purpose of this function is to free memory used by the messages struct
	to avoid memory leaks
	
	Takes message struct pointer
	
*/
void freeMessages(messages_t *msg) {

	// Free memory allocated to each message string
	int i;
	for (i = 0; i < msg->number_of_messages; i++) {
		free(msg->messages[i]);
	}
	// Free memory allocated to message string array
	free(msg->messages);
	// Free struct
	free(msg);
}
