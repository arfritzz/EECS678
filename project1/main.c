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
#include <string.h> 		/*needed for mitch reasons */

int got_response = 0;
char* relPath;
int path_Size = 0;
char* homePath;
char* command;
//char* command_SET = "set";

bool hasSpaces(char c){
	return(c == ' ' || c == '\n' || c == '\t');
}

void set_Path(char* newPath){
	char* temp = strtok(newPath, "=");
	char* type = temp;
	temp = strtok(newPath, "\0");
	relPath = temp;

	if((setenv(type, relPath, 1)) == -1){

	}
}

void parse_Input(char* command){
	while(hasSpaces(command[0])){
		command++;
	}

	// check Commands

	// FIRST COMMAND - SET
	if(strncmp(command,"set",3) == 0){
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
			if(strncmp(command,"/",1) == 0){

				char* tempCommand = strcpy(tempCommand, command);
				char* newPath = strtok(tempCommand," ");
				command += strlen(newPath);
				command++;
				printf("command: %s\n", command);
				printf("new Path: %s\n", newPath);
				command = '\0';
				strcat(relPath,":");
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
			if(strncmp(command,"/",1) == 0){

				char* tempCommand = strcpy(tempCommand, command);
				char* newPath = strtok(tempCommand," ");
				command += strlen(newPath);
				command++;
				printf("command: %s\n", command);
				printf("new Path: %s\n", newPath);

				setenv("HOME",newPath,0);
				homePath = getenv("HOME");
				printf("%s\n", homePath);
			}

			else {
				printf("Invalid path syntax, try again\n");
			}
		}

		// INVALID set parameter
		else {
			printf("Invalid parameter\n");
		}

		command = "\0";

	}

		//NEXT COMMAND - CHANGE DIRECTORY
	else if(strncmp(command,"cd",2) == 0){

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
		while(1){
			int inputStart = 0;

			printf("#> ");
			fflush(stdout);
			fgets(input, sizeof(input), stdin);

			relPath = getenv("PATH");
			//printf("%s\n", relPath);
			homePath = getenv("HOME");
			//printf("%s\n", homePath);

			//Ensure input is good
			if(strlen(input) > 0 && input[strlen(input)-1] == '\n'){
				input[strlen(input)-1] = '\0';
			}

			parse_Input(input);
		}
	}
}
