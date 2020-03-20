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
#include <string.h>    /* needed for mitch reasons */
#include <fcntl.h>     /* needed for output redirection */
#include <wait.h>

struct Job {
	int pid;
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

char* command;
void parse_Input(char* command);
//char* command_SET = "set";

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

void executeRedirection(char* action, char* args, char* filename) {
	pid_t pid = fork();
	
	//FILE* outputFile;

	//if(filename != NULL) {
	//	outputFile = freopen(filename, "w", stdout);
	//}

	if (pid == 0) {
		int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

		parse_Input(action);

		close(fd);

	}
	else {
		wait(NULL);
		exit(0);
		// parent doesnt need to do anything
	}
}

bool redirectOut (char* command) {
	//left argument
		char leftArg[64];
		int position = 0;
		while (command[position] != '>') {
			leftArg[position] = command[position];
			position++;
		}
		leftArg[position-1] = '\0';
		printf("The left arg is: %s\n", leftArg);

	//right argument
		char* rightArg = strrchr(command, '>');
		rightArg += 2;
		int length = strlen(rightArg);
		rightArg[length] = '\0';
		printf("The right arg is: %s\n", rightArg);

	// pass to function
		executeRedirection(leftArg, NULL, rightArg);	
	
}

bool redirectIn (char* command) {
	//left argument
        char leftArg[64];
        int position = 0;
        while (command[position] != '<') {
                leftArg[position] = command[position];
                position++;
        }       
        leftArg[position-1] = '\0'; 
        printf("The left arg is: %s\n", leftArg);

    //right argument
        char* filename = strrchr(command, '<');
        filename += 2;
        printf("The right arg is: %s\n", filename);

     //send result of left arg to right arg destination
        int finalfile = dup(0);
		freopen(filename, "r", stdin);
		parse_Input(leftArg);
		stdin = fdopen(finalfile, "r");

}

bool pipeing (char* command) {
	//left argument
        char leftArg[64];
        int position = 0;
        while (command[position] != '|') {
                leftArg[position] = command[position];
                position++;
        }       
        leftArg[position-1] = '\0'; 
        printf("The left arg is: %s\n", leftArg);

        //right argument
        char* rightArg = strrchr(command, '|');
        rightArg += 2;
        printf("The right arg is: %s\n", rightArg);

	int fdpipe[2];
	pipe(fdpipe);

}



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
		redirectIn(command);	
	}

	//redirect output
	else if (index(command, '>') != NULL) {
		redirectOut(command);
	}

	//PIPING
	else if (strstr(command, "|")) {

	}

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
			printf("%s\n", command);
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
				printf("command: %s\n", command);
				printf("new Path: %s\n", newPath);

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
		
		
		if (command[1] == '\0') {
			
			if(chdir(getenv("HOME")) != 0){
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

	else if(strncmp(command,"quit",4) == 0){
		exit(0);
	}

	else if(strncmp(command,"exit",4) == 0){
		exit(0);
	}

	else if(strncmp(command,"jobs",4) == 0){

	}

	//RUNNING EXECUTABLES
	else if(strncmp(command,"./",2) == 0){
		command+=2;

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

		pid_t pid;

		switch((pid = fork())){
			case -1:
				printf("Error in Fork\n\n");
				exit(0);
				break;
			case 0:
				execvp(tokens[0], tokens);
				printf("Failure in child Function\n\n");
				exit(0);
				break;
			default:
				printf("parent doing stuff\n");
				break;
		}

		printf("End of parent program\n\n");
	}

	else{
		printf("ALSOPOOP\n");
	}
}


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


	if(argv[1] != NULL){
		if(strcmp(argv[1], "<")){
			printf("Running commands from: %s\n", argv[2]);
			FILE* inputFile = fopen(argv[2], "r");

			if(inputFile == 0){
				printf("Invalid input file, try again\n");
				exit(1);
			}
		}
	}


	else {
	    homePath = getenv("HOME");
		//initalHome = homePath;
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
