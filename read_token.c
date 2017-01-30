#include "read_token.h"

token_t *initTokenStruct(const char *filename) {

	// Allocate memory for token struct
	token_t *newToken = (token_t*) malloc(sizeof(token_t));

	if (newToken == NULL) {
		return NULL;
	}

	// Allocate memory for filename string
	int filename_length = strlen(filename) + 1;
	newToken->filename = (char*) malloc(filename_length * sizeof(char));

	if (newToken->filename == NULL) {
		free(newToken);
		return NULL;
	}

	// Set filename string and initial file position
	strcpy(newToken->filename, filename);
	newToken->position = 0;

	return newToken;
}

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

char *getMessage(int index, messages_t *msg) {

	// If index is outside of the range, return a NULL string
	if (index >= msg->number_of_messages || index < 0) {
		return "\0";
	}

	return msg->messages[index];
}

void printValidIndices(messages_t *msg) {

	int num_messages = msg->number_of_messages;

	if (num_messages <= 0) {
		printf("No messages.\n");
	}
	else {
		printf("Valid indices: 1 to %d\n", num_messages);
	}
}

void getUserMessage(const char *prompt, messages_t *msg) {

	int index;
	char ch;
	char message[MAX_MESSAGE_SIZE] = "";

	printValidIndices(msg);

	if (msg->number_of_messages <= 0) {
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

int readToken(token_t *tkn, messages_t *msg) {

	// Open token file
	FILE *fp = fopen(tkn->filename, "r");

	if (fp == NULL) {
		return 1;
	}

	// Buffer used to read lines from token file
	char line[BUFFER_SIZE];
	// Buffer used to store message before caching
	char message[MAX_MESSAGE_SIZE] = "";
	// Flag set if currently reading message lines
	int isMessage = 0;


	// Seek to last read position in file
	fseek(fp, tkn->position, SEEK_SET);

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
	tkn->position = ftell(fp);

	fclose(fp);

	return 0;
}

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

	// Copy message string into cache
	strcpy(msg->messages[index], message);

	// Update the number of messages
	msg->number_of_messages++;

	return 0;
}

// TODO: Update this function when token ring network is implemented.
//       This should queue a message to be written when token is obtained.
int writeUserMessage(const char *prompt, int num, token_t *tkn) {

	char message[MAX_MESSAGE_SIZE] = "";
	char line[BUFFER_SIZE] = "";

	// Prompt user for message
	printf("%s", prompt);

	// Read user message from stdin
	while (1) {
		fgets(line, MAX_MESSAGE_SIZE, stdin);
		if (!strcmp(line, "\n")) {
			break;
		}
		strcat(message, line);
		memset(line, 0, strlen(line));
	}

	// Write message to token file
	return writeMessage(message, num, tkn);
}

int writeMessage(const char *message, int num, token_t *tkn) {

	// Open token file to append message
	FILE *fp = fopen(tkn->filename, "a");

	if (fp == NULL) {
		return 1;
	}

	// Write message to token file with header and footer included
	fprintf(fp, "<message n=%d>\n", num);
	fputs(message, fp);
	fputs("</message>\n", fp);

	fclose(fp);

	return 0;
}

void freeToken(token_t *tkn) {

	// Free memory allocated to filename string
	free(tkn->filename);
	// Free struct
	free(tkn);
}

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