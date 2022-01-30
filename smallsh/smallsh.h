#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


/* struct for command line*/
struct commandLine
{
	char* command;
	char arguments[512]
	char* inputFile;
	char* outputFile;
	_Bool runBg;						// true if & and false otherwise. if true, command runs in background (unless built in)
	
};


/// <summary>
/// prompt in command line with 
/// </summary>
/// Parameters: None
///Returns: None
void commandPrompt() {
	char* command[2048];
	size_t buflen;
	int chars;

	// prompt for command
	printf(":");

	// get user input
	chars = getline(&command, &buflen, stdin);
	command[chars - 1] = '\0';										// clear out new line

	// if user input blank or starts with #, loop
	
	// parse user input and store in commandLine struct

	// expand $$ in a command into PID of smallsh

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




