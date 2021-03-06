// Anna Fritz and Mitch Simmons
// Project 1
// EECS 678
// Main.c

#include <stdio.h>     /* standard I/O functions                         */
#include <stdlib.h>    /* exit                                           */
#include <stdbool.h>   /* using bool data type			                     */
#include <unistd.h>    /* standard unix functions, like getpid()         */
#include <signal.h>    /* signal name macros, and the signal() prototype */
#include <sys/types.h> /* needed for pid_t */
#include <sys/utsname.h> /* needed for system information */
#include <dirent.h>		/* for ls */
#include <string.h>    /* needed for mitch reasons */
#include <fcntl.h>     /* needed for output redirection */
#include <wait.h>

struct Job {
	pid_t pid;
	int id;
	char cmd [300];
};

static struct Job jobs[64];
int job_count = 0;

int got_response = 0;
char* relPath;
int path_Size = 0;
char* homePath;
extern char **environ;

//used to keep track of 
//current path for cd
char* cwd;

char* command;
void parse_Input(char* command);
bool pipecommand (char* leftarg, char* rightarg);

/************************
Ensures the beginning of the input 
command is space deliminated 
************************/ 
bool hasSpaces(char c){
	return(c == ' ' || c == '\n' || c == '\t');
}

char** mygetenv () {

}


/***************
	< and > 
***************/ 
void executeRedirection(char direction, char* action, char* filename) {

	pid_t pid = fork();
	int success, done; 

	if (pid == 0) {
		action[strlen(action)] ='\0';

		// redirects standard input
		if (direction == '<') {
			int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			dup2(fd,0);
			close(fd);
			parse_Input(action);
			exit(1);
		}

		// redirects standard output
		else {
			printf("\n	Checkout %s for your output!\n\n", filename);
			int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
			dup2(fd,1);
			close(fd);
			parse_Input(action);
			exit(1);
		}
	}

	// must wait so command line 
	// prompt prints correctly
	else {
		wait(&done);

	}
}

/******************
PARSING ARGS FOR
REDIRECTION
*******************/
// this function is just a bunch
// of parsing paramaters, 
// nothing special 
bool redirect (char direction, char* command) {
	//left argument
	char leftArg[1024];
	int position = 0;
	while (command[position] != direction) {
			leftArg[position] = command[position];
		position++;
	}
	leftArg[position] = '\0';
	
	//right argument
	char* rightArg = strrchr(command, direction);
	rightArg++;
	position = 0;
	while (rightArg[position] == 32) {
		rightArg++;
	}

	while(rightArg[position] != ' ') {
		position++;
	}
	rightArg[position] = '\0';

	// if direction is pipe
	if (direction == '|') {
		pipecommand(leftArg, rightArg);
	}
	else {
		// pass to function
		executeRedirection(direction, leftArg, rightArg);		
	}
}

/*************
	PIPING
*************/
bool pipecommand (char* leftArg, char* rightArg) {
	leftArg[strlen(leftArg)-1] = '\0';
	rightArg[strlen(rightArg)] = '\0';


	// sends output of left arg as input to right arg
	int pipefd[2];
	pipe(pipefd);

	pid_t pid1;
	pid1 = fork();

	int status1, status2;

	// child is the left side of the pipe
	// child's output directed to parent
	if (pid1 == 0) {
		dup2(pipefd[1],1);
		close(pipefd[1]);
		close(pipefd[0]);
		parse_Input(leftArg);
		exit(1);
	}
	else {
		pid1 = fork();

		// within the parent, new process created
		// the child of this process reads the 
		// output of the first child and uses
		// it as input
		if(pid1 == 0) {
			dup2(pipefd[0], 0);
			close(pipefd[0]);
			close(pipefd[1]);
			parse_Input(rightArg);
			exit(1);
		}

		wait(NULL);
	}

	close(pipefd[0]);
	close(pipefd[1]);
	wait(NULL);

}

/*---------------------------
PIPING MULTIPLE
*----------------------------*/
void multipipe(char* command) {
	int i,args = 0;

	while(command[i] != '\0') {
		if(command[i] == '|') {
			args++;
		}
		i++;
	}

	printf("this many %d\n", args);
}

/******************************
		BACKGROUND FUNCTION
******************************/

void execBackgroundFunction(char* command){
	pid_t pid;
	int status;

	pid = fork();

	//remove the &
	int j, commandlen = strlen(command);
	for (int i=j=0; i < commandlen; i++){
		if(command[i] != '&'){
			command[j++] = command[i];
		}
	}
	command[j-1] = '\0';

	if(pid < 0){
		printf("Error in executing background function");
		exit(0);
	}
	if(pid == 0){	
		//child process
		printf("\n[%d] %d running in background\n", job_count, getpid());
		parse_Input(command);
		printf("\n[%d] %d finished %s\nQUASH: %s : ", job_count, getpid(), command, getcwd(NULL,1024));
		exit(0);
	}
	else {

		struct Job new_job;
		new_job.pid = pid;
		new_job.id = job_count;
		strcpy(new_job.cmd,command);

		jobs[job_count] = new_job;
		job_count++;

	}
	//waitpid(-1,NULL,WNOHANG);
	waitpid(pid, &status, WNOHANG);	
}


/******************************
		PARSING INPUT
*******************************/

void parse_Input(char* command){
	while(hasSpaces(command[0])){
		command++;
	}

	// check Commands
	if(strncmp(command,"help",4) == 0){
		printf("\n\n COMMAND HELP\n\n");
		printf("set (PATH | HOME) (args) \n");
		printf("cd (args)\n");
		printf("(exec) (args)\n");
		printf("< = redirect input\n");
		printf("> = redirect output\n");
		printf("| used for piping\n\n\n");
	}

	/*----------
		<
	-----------*/
	else if (index(command, '<') != NULL) {
		redirect(60, command);	
	}

	/*----------
		>
	-----------*/
	else if (index(command, '>') != NULL) {
		redirect(62, command);
	}

	/*----------
		PIPE
	-----------*/
	else if (strstr(command, "|")) {
		//int args, i = 0;
		//while(i < strlen(command)) {
		//	if(command[i] == '|') {
		//		args++;
		//	}
		//	i++;
		//}
		//if (args > 1) {
		//	multipipe(command);
		//}
		//else {
			redirect('|', command);
		//}
	}

	/*----------
		SET
	-----------*/ 
	else if(strncmp(command,"set",3) == 0){
	
	//do ++ on command
	//printf("%s\n", command);
	command += 3;
	while(hasSpaces(command[0])){
		command++;
	}

		/*----------
			SET PATH
		-----------*/ 
		if(strncmp(command,"PATH",4) == 0){
			// used to get locations of all paths
			//remove PATH from command, remove following spaces
			command += 4;

			while(hasSpaces(command[0])){
				command++;
			}

			//remove actual pathway from command, characters
			//between spaces
			//printf("command:\n %s\n", command);
			if(strncmp(command,"=",1) == 0){
			
				char* newPath = strstr(command, "/");

				if (newPath == NULL) {
					printf("Incorrect path name. Try again\n\n");
					exit(0);
				}
				else{
					//execve(command,&command,getenv("PATH"));
					//relPath = getenv("PATH");
					//printf("new PATH variable: %s\n", relPath);
					if((setenv("PATH",newPath,0))< 0){
						printf("path set error\n");
					}
					else {
						relPath = getenv("PATH");
						printf("new PATH variable: %s\n", relPath);
					}
				}
			}

			// Invalid path syntax
			else{
				printf("Invalid path syntax\n");
			}
		}
	

		else if(strncmp(command,"path",4) == 0){
			printf("Invalid syntax, perhaps: set PATH\n");
		}

		/*----------
			SET HOME
		-----------*/ 
		else if(strncmp(command,"HOME",4) == 0){
			
			command += 4;
			while(hasSpaces(command[0])){
				command++;
			}

			//GET NEW HOME PATH
			if(strncmp(command,"=",1) == 0){

				char* newPath = strstr(command,"/");

				if(setenv("HOME",newPath,1) < 0){
					printf("path set error\n");
				}
				else {
					homePath = getenv("HOME");
					printf("New HOME variable: %s\n", homePath);
				}
			}

			else {
				printf("Invalid path syntax, try again\n");
			}
		}

		else {
			char leftArg[200];
			char rightArg[200];

			int i,j = 0;

			while (command[i] != '=') {
				leftArg[i] = command[i];
				i++;
			}
			leftArg[i] = '\0';
			while (command[i] == ' ' || command[i] == '=') {
				i++;
			}

			while (i < strlen(command)) {
				rightArg[j] = command[i];
				i++;
				j++;
			}
			rightArg[j] = '\0';
			
			printf("rightArg: %s\n", rightArg);
			printf("leftArg: %s\n", leftArg);

			command[strlen(command)] = '\0';

			setenv(leftArg, rightArg, 1);
		}

		command[strlen(command)] = '\0';
	}

	/*----------
		CD
	-----------*/ 
	else if(strncmp(command,"cd",2) == 0){
		//chdir(const char *path) changes current directory to 
		//the directory specified in path

		//account for the cd and the space
		command += 2;

		//if no command given, return
		//to home directory
		
		
		if (command[1] == '\0' || command[1] == '<' || command[1] == '>') {
			if(chdir(getenv("HOME")) != 0){
				//printf("the home directory is: %s\n", getenv("HOME"));
				printf("Failed to change to HOME, change HOME path\n");
			}
			else {
				printf("\n");
			}
		}

		//otherwise set the real path
		else {
			//find the previous directory
			//this will become the new working directory
			char* prevDirectory = getcwd(NULL,1024);
			
			//add a / where the space was
			//mark the end of the string
			if (command[1] == '/') {
				command++;
			}
			else {
				command[0] = '/';
			}	
			command[strlen(command)] = '\0';
			
			strcat(prevDirectory, command);	

			int ret = chdir(prevDirectory);
                
			// if the directory doesnt exist then error
			if (ret == -1) {
                printf("Change directory unsuccessful.\n");
            }
			else {
				printf("\n");
			}

		}
	}

	/*----------
		QUIT
	-----------*/ 
	else if(strncmp(command,"quit",4) == 0){
		printf("Exiting Quash... \n");
		exit(0);
	}

	/*----------
		EXIT
	-----------*/ 
	else if(strncmp(command,"exit",4) == 0){
		printf("Exiting Quash... \n");
		exit(0);
	}

	/*----------
		JOBS
	-----------*/ 
	else if(strncmp(command,"jobs",4) == 0){
		printf("\nCurrent Jobs:\n");
		printf("Job ID, PID, Command\n\n");

		for(int i=0; i < job_count; i++){
			if(waitpid(jobs[i].pid, NULL, WNOHANG) == 0 || (kill(jobs[i].pid, 0) == 0)){
				printf("[%d] %d || %s\n\n", jobs[i].id, jobs[i].pid, jobs[i].cmd);
			}
		}
		printf("\n");
	}


	/*-----------
	   KILL CMD
	-------------*/
	else if(strncmp(command, "kill", 4) == 0) {
		command+=4;
		//command has format kill SIGNUM JOBID
		int signum;
		pid_t jobid;
		
		while (hasSpaces(command[0])) {
			command++;
		}

		char charsignum[100];
		char charjobid[500];

		int i,j = 0;

		while(command[i] != 32) {
			charsignum[i] = command[i];
			i++;
		}
		charsignum[i] = '\0';

		i++;

		while(command[i] != '\0') {
			charjobid[j] = command[i];
			j++;
			i++;
		}
		charjobid[j]= '\0';
		//printf("signum %s, jobid %s\n", charsignum, charjobid);

		sscanf(charsignum, "%d", &signum);
		sscanf(charjobid, "%d", &jobid);

		kill(jobid, signum);
	}

	/*----------
		RUNNING
		EXECUTABLES
	-----------*/ 
	else{

		/*------------------------
		Check for background process
		--------------------------*/

		if(index(command, '&') != NULL){
			execBackgroundFunction(command);
		}
	
		else {

			pid_t pid;
			pid = fork();
			int status;
			char* args;

			if (pid == 0) {
				while ((command[0]) == ' ') {
					command++;
				}

				command[strlen(command)] = '\0';

				// check to see if there is an execuatable 
				//args = index(command, ' ');
				if (index(command, ' ') == NULL) {
					args = NULL;
				}
				else {
					args = index(command, ' ');
					args++;

					// if there is an accedential space
					if (args[0] == '\0') {
						command[strlen(command)-1] ='\0';
						args = NULL;
					}
					else {
						args[strlen(args)] = '\0';
					}
					

				}

				if (args == NULL) {

					int success = execlp(command, command, NULL);
					if (success == -1) {
						printf("\nInvalid Command\nExecutable not found.\n");
						exit(0);
					}
				}

				/*----------
					WITH 
					ARGS
				-----------*/ 
				else {
					char* action;
					//char space[] = ' ';
					int i = 0;
					
					action = strtok(command, " ");
					action[strlen(action)] = '\0';
					args[strlen(args)] = '\0';

					int success = execlp(action, action, args, (char *)NULL);
					//char* enviro = getenv("PATH");
					//int success = execve(action, *args, envp[i]);

					if (success == -1) {
						printf("\nInvalid Command\nPerhaps the wrong arguments\nOr executable not found\n");
						exit(0);
					}
				}
			}
			else {
				wait(&status);
			}
			waitpid(pid, &status, 0);
		}	
	}
}


/************************
MAIN FUNCTION
*************************/
int main(int argc, char **argv, char **envp) {
	char input[1024];
	char* inputBuffer[32];

	printf("\n\n\n		Welcome to Quash		\n");
	printf("	'help' for command options\n\n\n\n");

		for(int i=0; i<argc-1; i++){
			printf("%s\n", argv[i]);
		}

	if(argv[1] != NULL){
		


		if(strncmp(argv[1], "<",1) == 0){
			printf("Running commands from: %s\n", argv[2]);
			FILE* inputFile = fopen(argv[2], "r");

			if(inputFile == 0){
				printf("Invalid input file, try again\n");
				exit(1);
			}

			int count = 0;
			while(count < 1024 && fgets(command, 1024, inputFile)){
				printf("FULL COMMAND: %s\n\n", command);
				parse_Input(command);
				command = NULL;
				count++;
			}

			fclose(inputFile);
		}
	}


	else {
	    homePath = getenv("HOME");
		relPath = getenv("PATH");
		while(1){
			int inputStart = 0;

			cwd = getcwd(NULL,1024);
			printf("QUASH: %s : ", cwd);
			fflush(stdout);
			fgets(input, sizeof(input), stdin);

			//Ensure input is good
			if(strlen(input) > 0 && input[strlen(input)-1] == '\n'){
				input[strlen(input)-1] = '\0';
			}


			parse_Input(input);

		}
	}
}