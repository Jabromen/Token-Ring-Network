#ifndef _READ_TOKEN_H
#define _READ_TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Size of buffer used to read lines from token file
#define BUFFER_SIZE 512
// Maximum size that messages can be
#define MAX_MESSAGE_SIZE 1024
// Delimiters used to parse token file headers
#define DELIM "<> n="

// Struct used to manage operations on the token file
typedef struct token_t {
	// File name for token file
	char *filename;
	// Last read position in token file
	long position;
} token_t;

// Struct used to hold cached messages
typedef struct messages_t {
	// Length of message string array.
	int length;
	// Current number of messages stored in array.
	int number_of_messages;
	// Message string array.
	char **messages;
} messages_t;

/*
 * Initializes and allocates memory for token struct.
 * @param: filename: File name of the token file.
 * @return: Pointer to token struct.
 */
token_t *initTokenStruct(const char *filename);

/*
 * Initializes and allocates memory for messages struct.
 * @param: length: Initial length of messages string array. Should be 1 or greater.
 * @return: Pointer to messages struct.
 */
messages_t *initMessagesStruct(int length);

/*
 * Gets a message from cache at an index.
 * @param: index: Index of message to be retrieved.
 * @param: msg: Messages struct.
 * @return: Returns message string. Returns "\0" if invalid index.
 */
char *getMessage(int index, messages_t *msg);

/*
 * Prints the valid message indices.
 * @param: msg: Messages struct.
 */
void printValidIndices(messages_t *msg);

/*
 * Prompts the user for an index to retrieve a message from cache.
 * @param: prompt: Displayed prompt for user input.
 * @param: msg: Messages struct.
 */
void getUserMessage(const char *prompt, messages_t *msg);

/*
 * Reads the token file and caches new messages.
 * @param: tkn: Token struct.
 * @param: msg: Messages struct.
 * @return: Returns 0 if success, 1 if failed.
 */
int readToken(token_t *tkn, messages_t *msg);

/*
 * Increases the memory allocated to message string array.
 * Should be called when message string array is full.
 * @param: msg: Messages struct.
 * @return: Returns 0 if success, 1 if failed.
 */
int increaseNumMessages(messages_t *msg);

/*
 * Adds a message string to the cached messages.
 * @param: message: Message string to be cached.
 * @param: msg: Messages struct.
 * @return: Returns 0 is success, 1 if failed.
 */
int addMessage(const char *message, messages_t *msg);

/*
 * Prompts the user for a message to append to token file.
 * @param: prompt: Displayed prompt for user input.
 * @param: num: Number that will be written in message header.
 * @param: tkn: Token struct.
 * @return: Returns 0 if success, 1 if failed.
 */
int writeUserMessage(const char *prompt, int num, token_t *tkn);

/*
 * Appends a message string to token file.
 * @param: message: Message string to be written to token file.
 * @param: num: Number that will be written in message header.
 * @param: tkn: Token struct.
 * @return: Returns 0 if success, 1 if failed.
 */
int writeMessage(const char *message, int num, token_t *tkn);

/*
 * Frees all memory used by token struct.
 * @param: tkn: Token struct to be freed.
 */
void freeToken(token_t *tkn);

/*
 * Frees all memory used by messages struct.
 * @param: msg: Messages struct to be freed.
 */
void freeMessages(messages_t *msg);

#endif
