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

/// <summary>
/// expands instances of $$ in user input to PID
/// </summary>
/// <param name=""></param>

char* expandInput(char* userInput) {

	char newCommand[2048];
	char command[2048];
	char* letter;
	int i = 0;

	//ensure newCommand and command are clear for new input
	newCommand[0] = '\0';				
	command[0] = '\0';

	printf("%s", userInput);
	
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

	return newCommand;
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
	char* userInput;
	//char* inputCopy;
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
		//inputCopy = '\0';


		printf(":");

		// get user input
		chars = getline(&userInput, &buflen, stdin);
		char* savePtr;		

		if (userInput[0] == '\n') {
			//blank input
			continue;
		}

		expand = strstr(userInput, "$$");
	
		if (expand != NULL) {
			// $$ found in input and must be converted
			userInput = expandInput(userInput);
			printf("User input is now: %s\n", userInput);
		}
		
		if (strlen(userInput) > 1 && userInput[strlen(userInput)-2] == '&')
		{
			// symbol to run in background
			background = 1;
		}
		

		if (userInput[0] == '#')
		{
			// comment, ignore this line
			continue;
		}

		userInput[strlen(userInput) - 1] = '\0';								// clear out new line
	
		char* token = strtok_r(userInput, " ", &savePtr);

		// compare to token to next commands  **************************** without getting segfault

		//if (userInput == 'exit') {
		/*
		if (token == 'exit'){
			exit(0);
		}
		*/

		printf("TOKEN: %s", token);

		if (userInput == 'cd') {
			//changes to the directory specified in the HOME environment variable 
			printf("CD!");
			//cdCommand();
		}

		if (token == 'cd') {
			//changes to the directory specified in the HOME environment variable
			printf("CD TO PATH!");
			char* token = strtok_r(NULL, " ", &savePtr);
			cdCommandArg(token);
		}

		// parse user input and store in commandLine struct
		// createCommand(token, userInput, savePtr);

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
	char cwd[256];

	getcwd(cwd, sizeof(cwd));
	printf("STarts: %s", cwd);
	chdir(getenv("HOME"));
	getcwd(cwd, sizeof(cwd));
	printf("Ends: %s", cwd);
	return;
}

/// <summary>
/// cd built in command with an argument
/// changes to the directory specified in the HOME environment variable 
/// </summary>
/// Parameters: path of directory to change to (absolute or relative)
///Returns: None
void cdCommandArg(char* path) {
	char cwd;


	getcwd(cwd, sizeof(cwd));
	printf("STarts: %s", cwd);
	chdir(path);
	getcwd(cwd, sizeof(cwd));
	printf("Ends: %s", cwd);
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




