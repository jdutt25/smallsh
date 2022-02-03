#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* userInput;


/* struct for command line*/
struct commandLine
{
	char* command;
	char arguments[512];
	char* inputFile;
	char* outputFile;
	_Bool runBg;						// true if & and false otherwise. if true, command runs in background (unless built in)
	
};

/// <summary>
/// expands instances of $$ in user input to PID
/// </summary>
/// <param name=""></param>

char* expandInput(char* userInput) {

	char newCommand[2048];
	char command[2048];
	int i = 0;

	//ensure newCommand and command are clear for new input
	newCommand[0] = '\0';				
	command[0] = '\0';
	
	while (i < strlen(userInput)) {
		if ((userInput[i] == '$') && (i < (strlen(userInput) + 2)) && (userInput[i + 1] == '$')) {
			// found $$, replace with pid
			sprintf(newCommand, "%s%d", command, getpid());
			strcpy(command, newCommand);
			i += 2;
		}
		else {
			sprintf(newCommand, "%s%c", command, userInput[i]);
			strcpy(command, newCommand);
			i += 1;
		}
	}
	userInput = newCommand;
	return userInput;
	}


/// <summary>
/// populates comand line struct
/// </summary>
/// <param name="token">token for strtok_r of input</param>
/// <param name="userInput">user input</param>
/// <param name="savePtr">pointer for strtok_r of input</param>
/// <returns></returns>
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
		printf("end of token");
	}

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
	char *expand;
	size_t buflen;
	size_t chars;
	_Bool exitProgram = 0;	
	_Bool background = 0;
	

	while (exitProgram == 0)
	{
		// prompt for command

		//ensure variables are clear for new input
		userInput = '\0';

		printf(":");

		// get user input
		chars = getline(&userInput, &buflen, stdin);
		char* savePtr;		

		if (userInput[0] == '\n') {
			//blank input
			continue;
		}

		// make a copy of the input with space in memory
		char* inputCopy = malloc(2048);
		int i = 0;

		while (i < strlen(userInput)) {
			sprintf(inputCopy, "%s%c", inputCopy, userInput[i]);
			i++;
		}

		// check input for instance of $$
		expand = strstr(userInput, "$$");
	
		if (expand != NULL) {
			// $$ found in input and must be converted
			inputCopy = expandInput(userInput);
			printf("User input is now: %s\n", inputCopy);
		}
		
		if (strlen(userInput) > 1 && userInput[strlen(userInput)-2] == '&')
		{
			// symbol to run in background
			background = 1;
		}
		
		if (userInput[0] == '#')
		{
			// input is a comment, ignore 
			continue;
		}

		inputCopy[strlen(inputCopy) - 1] = '\0';								// clear new line
		
		char* token = strtok_r(inputCopy, " ", &savePtr);

		if (strcmp(token, "exit") == 0){
			exitProgram = 1;
			exit(0);
		}	
		

		if (strcmp(token, "cd")==0){
			//changes to the directory specified in the HOME environment variable 
			char* token = strtok_r(NULL, " ", &savePtr);

			if (token == NULL) {
				//cd without path
				cdCommand();
			}
			else {
				// cd with path
				cdCommandArg(token);
			}
		}


		// parse user input and store in commandLine struct
		// createCommand(token, userInput, savePtr);

	}

		return;
}

/// <summary>																				DELETE???????????????????????????????????
/// Exit built in command
/// Shell will kill any other processes or jobs that your shell has started before it terminates itself
/// </summary>
/// Parameters: None
///Returns: None
void exitCommand() {
	exit(0);
	return;
}

/// <summary>
/// cd built in command without argument
/// changes to the directory specified in the HOME environment variable 
/// </summary>
/// Parameters: None
///Returns: None
void cdCommand() {
	chdir(getenv("HOME"));
	return;
}

/// <summary>
/// cd built in command with an argument
/// changes to the directory specified in the HOME environment variable 
/// </summary>
/// Parameters: path of directory to change to (absolute or relative)
///Returns: None
void cdCommandArg(char* path) {
	chdir(path);
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




