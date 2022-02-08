/*
	Author:	Jessica Dutton
	Class:	OSU CS 344
	Last updated: 2/7/22
	Smallsh
*/

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

char* userInput;
int status = 0;
int termSignal = -5;		// placeholder value
int childArr[200];
int indx = 0;
_Bool foregroundOnly = 0;		// boolean for foreground only mode


/* signal handler for SIGTSTP */
void handle_SIGTSTP(int signo) {
	if (foregroundOnly == 0){
		//change to foreground only mode
		char* message = "\nEntering foreground-only mode (& is now ignored)\n";
		write(STDOUT_FILENO, message, 50);
		foregroundOnly = 1;
	}
	else {
		// in foreground only mode currently, change back
		char* message = "\nExiting foreground-only mode\n";
		write(STDOUT_FILENO, message, 29);
		foregroundOnly = 0; 
	}
}

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

	if (termSignal != -5) {
		printf("terminated by signal %d\n", termSignal);
		fflush(stdout);
	}
	else {
		printf("exit value %d\n", status);
		fflush(stdout);
	}

}

/// <summary>
/// performs input & output redirection & executes command
/// </summary>
/// <param name=""></param>
void childProcess(struct commandLine* currCommand) {

	//  ignore SIGTSTP
	struct sigaction ignore_action = { { 0 } };
	ignore_action.sa_handler = SIG_IGN;
	sigaction(SIGTSTP, &ignore_action, NULL);

	if (currCommand->inputFile) {

		// input file provided
		char inFileName[256];
		strcpy(inFileName, currCommand->inputFile);

		int input = open(inFileName, O_RDONLY, 0777);
		if (input == -1) {
			// failed to open
			printf("Error, cannot open %s for input\n", currCommand->inputFile);
			fflush(stdout);
			status = 1;
			termSignal = -5;		// reset term signal for new status
			return;
		}

		int result = dup2(input, 0);
		if (result == -1) {
			perror("dup2");
			status = 1;
			termSignal = -5;		// reset term signal for new status
			return;
		}
	}


	if (currCommand->outputFile) {
		//output file provided
		char fileName[256];
		strcpy(fileName, currCommand->outputFile);
		int output = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0777);

		if (output == -1) {
			// failed to open
			perror("output open()");
			fflush(stdout);
			status = 1;
			termSignal = -5;		// reset term signal for new status
			return;
		}

		int result = dup2(output, 1);
		if (result == -1) {
			perror("output dup2");
			fflush(stdout);
			status = 1; 
			termSignal = -5;		// reset term signal for new status
			return;
		}

	}

	else
	{
		// no output file provided
		if (currCommand->runBg == 1) {
			// background command - direct to /dev/null
			int output = open("/dev/null", O_WRONLY, 0777);

			if (output == -1) {
				// failed to open
				perror("output open()");
				fflush(stdout);
				status = 1;
				termSignal = -5;		// reset term signal for new status
				return;
			}

			int result = dup2(output, 1);
			if (result == -1) {
				perror("output dup2");
				fflush(stdout);
				status = 1;
				termSignal = -5;		// reset term signal for new status
				return;
			}
		}

	}

	if (currCommand->runBg == 0) {
		// foreground process, child must terminate itself if SIGINT
		struct sigaction SIGINT_default_action = { { 0 } };
		SIGINT_default_action.sa_handler = SIG_DFL;
		sigaction(SIGINT, &SIGINT_default_action, NULL);
	}

	execvp(currCommand->arguments[0], currCommand->arguments);
	fflush(stdout);
	perror("execvp");
	fflush(stdout);

	status = 1;
	termSignal = -5;		// reset term signal for new status
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
		status = 1;
		termSignal = -5;		// reset term signal for new status
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
			if (WIFEXITED(childStatus))
				{
				// terminated normally
					status = WEXITSTATUS(childStatus);
					termSignal = -5;		// reset term signal for new status
				}
				else
				{
					//terminated abnormally
					termSignal = WTERMSIG(childStatus);
					statusCommand();

				}
		}
		else {
			// else, return right away & waitpid will be before next prompt
			printf("background pid is %d\n", spawnpid);
			fflush(stdout);

			// add spawnpid to array of background processes
			childArr[indx] = spawnpid;
			indx = indx + 1;
			
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
/// <returns>currCommand</returns>
struct commandLine* createCommand(char* token, char* userInput, char* savePtr, _Bool background)
{
	struct commandLine* currCommand = malloc(sizeof(struct commandLine));
	int i = 0;								// index of arg array

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
			token = strtok_r(NULL, " ", &savePtr);

		}
		else if (strcmp(token, ">") == 0) {
			// output file
			token = strtok_r(NULL, " ", &savePtr);
			if (strcmp(token, "\0") != 0) {
				currCommand->outputFile = calloc(strlen(token) + 1, sizeof(char));
				strcpy(currCommand->outputFile, token);
				token = strtok_r(NULL, " ", &savePtr);
			}
		}
		else if (strcmp(token, "\0") != 0) {
			// arguments
			currCommand->arguments[i] = token;
			i++;
			token = strtok_r(NULL, " ", &savePtr);
		}
	}

	currCommand->runBg = background;
	currCommand->arguments[i] = '\0';
	runCommand(currCommand);
	return currCommand;
}


/// <summary>
/// prompt in command line with 
/// </summary>
/// Parameters: None
///Returns: None
/// Referenced: Exploration: Process API - Monitoring Child Processes, Exploration: Signal Handling API
void commandPrompt() {
	char* expand;
	size_t buflen;
	//size_t chars;
	_Bool exitProgram = 0;
	_Bool background = 0;
	char* token;

	// SIGINT_action struct - ignore SIGINT
	struct sigaction ignore_action = {{ 0 }};
	ignore_action.sa_handler = SIG_IGN;
	sigaction(SIGINT, &ignore_action, NULL);


	while (exitProgram == 0)
	{
		//ensure variables are clear for new input
		userInput = '\0';
		background = 0;			// reset bool to 0

		// iterate through background child pids to check for ones that have completed
		for (int i = 0; i < 200; i++) {
			int childStatus;
			int childSignal;
			int childPid = childArr[i];
			if (childPid)
			{
				if (childPid != -5 && waitpid(childPid, &childStatus, WNOHANG) != 0)
				{
					if (WIFEXITED(childStatus))
					{
						// terminated normally
						childSignal = WEXITSTATUS(childStatus);
					}
					else
					{
						//terminated abnormally
						childSignal = WTERMSIG(childStatus);
					}

					printf("background pid %d is done: exit value %d\n", childArr[i], childSignal);
					fflush(stdout);
					
					childArr[i] = -5;	// clear out value
				}
			}
			i++;
		}

		// signal handler for foreground only mode
		struct sigaction SIGTSTP_action = { { 0 } };
		SIGTSTP_action.sa_handler = handle_SIGTSTP;
		sigfillset(&SIGTSTP_action.sa_mask);
		SIGTSTP_action.sa_flags = SA_RESTART;
		sigaction(SIGTSTP, &SIGTSTP_action, NULL);
		
		printf(": ");
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
		}

		if (foregroundOnly != 1 && strlen(inputCopy) > 1 && inputCopy[strlen(inputCopy) - 2] == '&')
		{
			// symbol to run in background
			background = 1;
			inputCopy[strlen(inputCopy) - 2] = '\0';			// clear out & from end of input
		}

		if (inputCopy[0] == '#')
		{
			// input is a comment, ignore 
			continue;
		}

		inputCopy[strlen(inputCopy) - 1] = '\0';								// clear new line

		token = '\0';
		token = strtok_r(inputCopy, " ", &savePtr);

		if (strcmp(token, "status") == 0) {
			statusCommand();
		}

		else if (strcmp(token, "exit") == 0) {
			exit(0);
		}

		else if (strcmp(token, "cd") == 0) {
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
		else {
			createCommand(token, inputCopy, savePtr, background);
			continue;
		}

	}

	return;
}
