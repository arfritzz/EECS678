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
	char* cmd;
};

static struct Job jobs[64];
int job_count = 0;

int got_response = 0;
char* relPath;
int path_Size = 0;
char* homePath;

//used to keep track of 
//current path for cd
char* cwd;
struct utsname unameData;

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


// not sure if this function is used? maybe delete?
void set_Path(char* newPath){
	char* temp = strtok(newPath, "=");
	char* type = temp;
	temp = strtok(newPath, "\0");
	relPath = temp;

	if((setenv(type, relPath, 1)) == -1){

	}
}

/***************
	< and > 
***************/ 
void executeRedirection(char direction, char* action, char* filename) {
	
	// redirect
	// standard output to the file "filename"

	if (direction == '>') {

		pid_t pid = fork();
		int success; 

		if (pid == 0) {
			//child needs to inherit environment variables
			FILE* fd = freopen(filename, "w", stdout);
			parse_Input(action);

			if (fd != NULL) {
				int val = fileno(fd);
			}
			//execlp(action, filename, NULL);
			// need a way to exit out of the child?
			exit(1);
		}
		
		else {
			waitpid(pid, &success, 0);
			//exit(1);
			// parent doesnt need to do anything
		}
	}

	// direction is false so redirect in
	else {
		//send result of left arg to right arg destination
        int finalfile = dup(0);
		freopen(filename, "r", stdin);
		parse_Input(action);
		stdin = fdopen(finalfile, "r");

	}
}

/******************
PARSING ARGS FOR
REDIRECTION
*******************/
bool redirect (char direction, char* command) {
	//left argument
		char leftArg[1024];
		int position = 0;
		while (command[position] != direction) {
			leftArg[position] = command[position];
			position++;
		}
		leftArg[position] = '\0';
		printf("The left arg is: %s\n", leftArg);
	
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
		printf("The right arg is: %s\n", rightArg);

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
bool pipecommand (char* leftarg, char* rightarg) {
	
	// sends output of left arg as input to right arg

	char** dupleftarg;
	char** duprightarg;
	dupleftarg[0] = leftarg;
	duprightarg[0] = rightarg; 
	int fdpipe[2];
	pipe(fdpipe);

	int pid_t = fork();

	int status;

	if (pid_t == 0) {
		
		close(fdpipe[0]);
		
		// 1 is standard out
		// old file descriptor is fdpipe[1] (write end)
		// new file descriptor is standard out
		dup2(fdpipe[1], 1);
		close(fdpipe[1]);

		execvp(*dupleftarg, dupleftarg);

		exit(0);
	}
	else {
		close(fdpipe[1]);
		// 0 is standard in 
		// old file descriptor is fdpipe[0] (read end)
		// new file descriptor is standard in
		dup2(fdpipe[0], 0);
		close(fdpipe[0]);

		// must wait on child to finish
		waitpid(pid_t,&status, 0);

		execvp(*duprightarg, duprightarg);

	}

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
		printf("./(exec) (args)\n");
		printf("< = redirect input\n");
		printf("> = redirect output\n");
		printf("| used for piping\n\n\n");
	}

		//INPUT OUTPUT REDIRECTION
        //redirect input
	else if (index(command, '<') != NULL) {
		redirect(60, command);	
	}

	//redirect output
	else if (index(command, '>') != NULL) {
		redirect(62, command);
	}

	//PIPING
	else if (strstr(command, "|")) {
		redirect('|', command);
	}

	else if(strncmp(command,"pwd",3) == 0){
		command+=3;
		printf("The current working directory is: \n\n");
		printf("%s\n\n", cwd);
	}

	/* else if(strncmp(command, "ls", 2) == 0){
		DIR *p;
		struct dirent *d;

		p = opendir(cwd);
		if(p==NULL){
			printf("Directory Error");
		}
		else{
			int count = 0;
			while(d=readdir(p)){
				printf("%s\t",d->d_name);
				if(count == 4){
					printf("\n");
					count=0;
				}
				else {
					count++;
				}
			}
			printf("\n\n");
		}
	}

	else if(strncmp(command,"uname",5) == 0){
		if(uname(&unameData) == 0){
			printf("%s\n", unameData.sysname);
			printf("%s\n", unameData.nodename);
			printf("%s\n", unameData.release);
			printf("%s\n", unameData.version);
			printf("%s\n", unameData.machine);
		}
		else {
			printf("Error retrieving system information\n\n");
		}
	}

	else if(strncmp(command,"printenv",8) == 0){
		command+=8;
		while(hasSpaces(command[0])){
			command++;
		}

		if(strncmp(command,"HOME",4) == 0){
			printf("HOME: %s\n", homePath);
		}

		else if(strncmp(command,"PATH",4) == 0){
			printf("PATH: %s\n", relPath);
		}
	}
	*/

	// FIRST COMMAND - SET
	else if(strncmp(command,"set",3) == 0){
	//do ++ on command
	//printf("%s\n", command);
	command += 4;

		// change PATH location
		if(strncmp(command,"PATH",4) == 0){
			// used to get locations of all paths
			//remove PATH from command, remove following spaces
			command += 4;
			while(hasSpaces(command[0])){
				command++;
			}
			//remove actual pathway from command, characters
			//between spaces
			printf("command:\n %s\n", command);
			if(strncmp(command,"=",1) == 0){
				//printf("command: %s\n", command);
				char* newPath = strstr(command, "/");
				//printf("new Path: %s\n", newPath);
				strcat(relPath,":");

				// why do you string concat here?
				// just thinking about if the new path is different then the old path...
				strcat(relPath,newPath);

				if((setenv("PATH",relPath,0))< 0){
					printf("path set error\n");
				}
				else {
					relPath = getenv("PATH");
					printf("new PATH variable: %s\n", relPath);
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

		// change HOME location
		else if(strncmp(command,"HOME",4) == 0){
			
			command += 4;
			while(hasSpaces(command[0])){
				command++;
			}

			//GET NEW HOME PATH
			if(strncmp(command,"=",1) == 0){

				char* newPath = strstr(command,"/");
				//printf("command: %s\n", command);
				//printf("new Path: %s\n", newPath);

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
			printf("Invalid parameter\n");
		}

		command = "\0";

	}

	//NEXT COMMAND - CHANGE DIRECTORY
	else if(strncmp(command,"cd",2) == 0){
		//chdir(const char *path) changes current directory to 
		//the directory specified in path

		//account for the cd and the space
		command += 2;

		//if no command given, return
		//to home directory
		
		
		if (command[1] == '\0' || command[1] == '<' || command[1] == '>') {
			if(chdir(getenv("HOME")) != 0){
				printf("the home directory is: %s\n", getenv("HOME"));
				printf("Failed to change to HOME, change HOME path\n");
			}
			else {
				printf("You are now at: %s\n", getenv("HOME"));
			}
		}

		//otherwise set the real path
		else {
			//find the previous directory
			//this will become the new working directory
			char* prevDirectory = getcwd(NULL,1024);
			
			//add a / where the space was
			//mark the end of the string	
			command[0] = '/';
			command[strlen(command)] = '\0';
			
			strcat(prevDirectory, command);	

			int ret = chdir(prevDirectory);
			printf("%s\n", prevDirectory);
                
			// if the directory doesnt exist then error
			if (ret == -1) {
                                printf("Change directory unsuccessful.\nPlease try again\n");
                        }
			else {
				printf("You changed directories to:\n%s\n", prevDirectory);
			}

		}
	}

	// QUIT
	else if(strncmp(command,"quit",4) == 0){
		printf("Exiting Quash... \n");
		exit(0);
	}

	//EXIT
	else if(strncmp(command,"exit",4) == 0){
		printf("Exiting Quash... \n");
		exit(0);
	}

	// JOBS
	else if(strncmp(command,"jobs",4) == 0){
		printf("\n Current Jobs:\n");
		printf("Job ID, PID, Command\n\n");

		for(int i=0; i < job_count; i++){
			if(kill(jobs[i].pid, 0) == 0){
				printf(" (%d) %d || %s\n\n", jobs[i].id, jobs[i].pid, jobs[i].cmd);
			}
		}
	}
	
	// PRINT WORKING DIRECTORY
	else if(strncmp(command, "pwd", 3) == 0) {
		char* prevDirectory = getcwd(NULL,1024);
		printf("%s\n", prevDirectory);
	}

	//RUNNING EXECUTABLES
	else if(strncmp(command,"./",2) == 0){
		//command+=2;

		while(hasSpaces(command[0])){
			command++;
		}

		printf("%s\n",command);
		char** tokens;
		int token_count = 0;
		tokens[token_count] = strtok(command, " ");
		while(tokens[token_count]){
			printf("token: %s\n", tokens[token_count]);
			tokens[token_count+1] = strtok(NULL, " ");
			token_count++;
		}

		tokens[token_count+1] = '\0';

		pid_t pid = fork();
		int child_status;

		switch(pid){
			case -1:
				printf("Error in Fork\n\n");
				exit(0);
				break;
			case 0:
				execvp(tokens[0], tokens);
				printf("Failure in child Function\n\n");
				exit(0);
				break;
			default: ;
				/*struct Job new_job = {.pid = pid, .id = job_count, .cmd = command};
				jobs[job_count] = new_job;
				job_count++;*/
				while(wait(NULL) != pid){}
				break;
		}

		printf("[%d] %d Finished %s\n\n", jobs[job_count].id, jobs[job_count].pid, jobs[job_count].cmd);
	}

	else{
		pid_t pid;
		pid = fork();
		int status;

		if (pid == 0) {
			// check to see if there is an execuatable 
			while ((command[0]) == ' ') {
				command++;
			}

			printf("command: %s\n", command);

			char* inputPath = "/bin/";
			//char* finalChar = '\0'; 
			strcat(inputPath,command);
			//strcat(inputPath, finalChar);

			printf("HERE");

			int success = execlp("/bin/", command, NULL);

			if (success = -1) {
				printf("\nInvalid Command\n\n");
				exit(0);
			}
		}
		else {
			wait(&status);
		}

		
	}
}


/************************
MAIN FUNCTION
*************************/
int main(int argc, char **argv, char **envp) {
	char input[64];
	char* inputBuffer[32];
	//pid_t my_pid;
	//my_pid = getpid();
	// for(int i=0; i<sizeof(envp); i++){
	// 	printf("%s\n", envp[i]);
	// }

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

			//relPath = getenv("PATH");
			//printf("%s\n", relPath);
			//homePath = getenv("HOME");
			//printf("%s\n", homePath);

			//Ensure input is good
			if(strlen(input) > 0 && input[strlen(input)-1] == '\n'){
				input[strlen(input)-1] = '\0';
			}

			parse_Input(input);
		}
	}
}
