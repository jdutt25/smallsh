#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

char* userInput;


/* struct for command line*/
struct commandLine
{
	//char* command;
	char* arguments[256];
	char* inputFile;
	char* outputFile;
	_Bool runBg;						// true if & and false otherwise. if true, command runs in background (unless built in)
	
};


/// <summary>
/// performs input & output redirection & executes command
/// </summary>
/// <param name=""></param>
void childProcess(struct commandLine* currCommand) {

	if (currCommand->inputFile) {

		// input file provided
		int input = open(currCommand->inputFile, O_RDONLY, 0777);
		if (input == -1) {
			// failed to open
			printf("Error, cannot open input file %s\n", currCommand->inputFile);
			fflush(stdout);
			exit(1);
			//return;
		}

		int result = dup2(input, 0);
		if (result == -1) {
			perror("dup2\n");
			exit(2);
			//return;
		}
	}


	if (currCommand->outputFile) {
		//output file provided
		char fileName[256];
		strcpy(fileName, currCommand->outputFile);
		sprintf(fileName, "%s%s", fileName, "\0");
		int output = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0777);

		if (output == -1) {
			// failed to open
			perror("output open()");
			exit(1);
		}

		int result = dup2(output, 1);
		if (result == -1) {
			perror("output dup2\n");
			exit(2);
		}
	}

	execvp(currCommand->arguments[0], currCommand->arguments);
	perror("execvp");
	printf("Cannot run command %s\n", currCommand->arguments[0]);
	fflush(stdout);
	
	return;
}



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
/// runs a command in background or foreground
/// </summary>
/// refered to Exploration: Process API – Creating and Terminating Processes, Exploration: Files
void runCommand(struct commandLine* currCommand) {
	pid_t spawnpid = -5;
	int childStatus;
	spawnpid = fork();
	
	switch (spawnpid) {
		case -1:
			// fork failed
			perror("fork() failed!\n");
			exit(1);
			break;
		case 0:
			// spawnpid is 0, child will execute the code in this branch
			childProcess(currCommand);
			break;
		
		default:
		// parent
			if (currCommand->runBg == 0) {
				// run in foreground, must wait for completion
				waitpid(spawnpid, &childStatus, 0);
			}
			else {
				printf("background pid is %d\n", spawnpid);
				fflush(stdout);
				// else, return right away
				waitpid(spawnpid, &childStatus, WNOHANG);
				printf("background pid %d is done", spawnpid);

			}
		
	}

	return;
}


/// <summary>
/// populates comand line struct
/// </summary>
/// <param name="token">token for strtok_r of input</param>
/// <param name="userInput">user input</param>
/// <param name="savePtr">pointer for strtok_r of input</param>
/// <returns></returns>
struct commandLine* createCommand(char* token, char* userInput, char* savePtr, _Bool background)
{
	struct commandLine* currCommand = malloc(sizeof(struct commandLine));
	int i = 0;								// index of arg array

	// command
	//currCommand->command = calloc(strlen(token) + 1, sizeof(char));
	//strcpy(currCommand->command, token);

	// arguments

	currCommand->arguments[i] = token;
	i++;

	token = strtok_r(NULL, " ", &savePtr);

	while (token != NULL) {

		if (strcmp(token, "<") == 0)
		{
			// input file
			token = strtok_r(NULL, " ", &savePtr);
			currCommand->inputFile = calloc(strlen(token) + 1, sizeof(char));
			strcpy(currCommand->inputFile, token);

		}
		else if (strcmp(token, ">") == 0) {
			// output file
			token = strtok_r(NULL, " ", &savePtr);
			if (strcmp(token, "\0") != 0) {
				currCommand->outputFile = calloc(strlen(token) + 1, sizeof(char));
				strcpy(currCommand->outputFile, token);
			}
		}
		else if (strcmp(token, "\0") != 0) {
			// arguments
			currCommand->arguments[i] = token;
			i++;
			token = strtok_r(NULL, " ", &savePtr);
		}
	}

	currCommand->arguments[i] = '\0';
	runCommand(currCommand);
	return currCommand;
}


/// <summary>
/// prompt in command line with 
/// </summary>
/// Parameters: None
///Returns: None
void commandPrompt() {
	char *expand;
	size_t buflen;
	//size_t chars;
	_Bool exitProgram = 0;	
	_Bool background = 0;
	char* token;
	

	while (exitProgram == 0)
	{
		// prompt for command

		//ensure variables are clear for new input
		userInput = '\0';

		printf(":");
		fflush(stdout);

		// get user input
		getline(&userInput, &buflen, stdin);
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
			userInput[strlen(userInput) - 2] = "\0";			// clear out & from end of input
		}
		
		if (userInput[0] == '#')
		{
			// input is a comment, ignore 
			continue;
		}

		inputCopy[strlen(inputCopy) - 1] = '\0';								// clear new line
		
		token = "\0";
		token = strtok_r(inputCopy, " ", &savePtr);

		if (strcmp(token, "exit") == 0){
			exitProgram = 1;
			exit(0);
		}	

		if (strcmp(token, "status") == 0) {
			statusCommand();
		}
		

		if (strcmp(token, "cd")==0){
			//changes to the directory specified in the HOME environment variable 
			char* token = strtok_r(NULL, " ", &savePtr);

			if (token == NULL) {
				//cd without path
				void cdCommand();

			}
			else {
				// cd with path
				void cdCommandArg(char* token);
			}
		}

		// parse user input and store in commandLine struct
		else {
			createCommand(token, inputCopy, savePtr, background);
			continue;
		}

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

