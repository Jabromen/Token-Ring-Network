#include "read_token.h"

void printOptions() {
	printf("1: Write new message to token file.\n");
	printf("2: Read message at index.\n");
	printf("3: Quit.\n");
}

int main(int argc, char **argv) {

	if (argc < 2) {
		return 1;
	}

	char *filename = argv[1];

	token_t *tkn = initTokenStruct(filename);

	messages_t *msg = initMessagesStruct(8);

	readToken(tkn, msg);

	int option;
	char ch;

	printOptions();

	while(1) {
		printf("\nSelect an option: ");
		scanf("%d", &option);
		while ((ch = getchar()) != EOF && ch != '\n') {;}
		printf("\n");

		switch(option) {
			case 1:				
				writeUserMessage("Enter a message:\n", msg->number_of_messages + 1, tkn);
				break;

			case 2:
				getUserMessage("Enter an index: ", msg);
				break;

			case 3:
				printf("Exiting...\n");
				freeToken(tkn);
				freeMessages(msg);
				break;

			default:
				printOptions();
		}

		if (option == 3) {
			break;
		}

		readToken(tkn, msg);
	}
	return 0;
}
