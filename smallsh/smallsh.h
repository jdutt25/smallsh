#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/* struct for command line*/
struct commandLine
{
	char* command;
	char arguments[512];
	char* inputFile;
	char* outputFile;
	_Bool runBg;						// true if & and false otherwise. if true, command runs in background (unless built in)
	
};

struct commandLine* createCommand(char* token, char* userInput, char* savePtr)
{
	struct commandLine* currCommand = malloc(sizeof(struct commandLine));

	// command
	token = strtok_r(NULL, " ", &savePtr);
	currCommand->command = calloc(strlen(token) + 1, sizeof(char));
	strcpy(currCommand->command, token);

	// arguments
	token = strtok_r(userInput, " ", &savePtr);
	if (token == NULL) {
		// no arguments, input or output file, process the command

	}

	// check if contains $$ and expand to PID if so								// IMPLEMENT

		// input file

		// output file

		// run in background (ends in &)

		// expand $$ in a command into PID of smallsh
	return;
}

/// <summary>
/// prompt in command line with 
/// </summary>
/// Parameters: None
///Returns: None
void commandPrompt() {
	char* userInput;
	size_t buflen;
	size_t chars;
	_Bool exitProgram = 0;	
	

	while (exitProgram == 0)
	{
		// prompt for command

		printf(":");

		// get user input
		chars = getline(&userInput, &buflen, stdin);
		char* savePtr;

		char *token = strtok_r(userInput, " ", &savePtr);

		if (userInput[0] == '\n') {
			//blank input
			continue;
		}

		if (token[0] == '#')
		{
			// comment, ignore this line
			continue;
		}

		// parse user input and store in commandLine struct
		

	}
		return;
}

/// <summary>
/// Exit built in command
/// Shell will kill any other processes or jobs that your shell has started before it terminates itself
/// </summary>
/// Parameters: None
///Returns: None
void exitCommand() {
	return;
}

/// <summary>
/// cd built in command without argument
/// changes to the directory specified in the HOME environment variable 
/// </summary>
/// Parameters: None
///Returns: None
void cdCommand() {
	return;
}

/// <summary>
/// cd built in command with an argument
/// changes to the directory specified in the HOME environment variable 
/// </summary>
/// Parameters: path of directory to change to (absolute or relative)
///Returns: None
void cdCommandArg(char* path) {
	return;
}

/// <summary>
/// status built in command
/// prints out either the exit status or the terminating signal of the last foreground process ran by your shell
/// ignores built-in commands
/// </summary>
/// Parameters: None
///Returns: None
void statusCommand() {
	// if ran before any foreground command is run, return exit status 0


	return;
}




