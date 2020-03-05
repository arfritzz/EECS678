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


int main(int argc, char **argv, char **envp) {
	pid_t my_pid;
	my_pid = getpid();
	// for(int i=0; i<sizeof(envp); i++){
	// 	printf("%s\n", envp[i]);
	// }

	while(1){
		char input[64];
		int inputStart = 0;
		char delim[] = " ";
		char delim2[] = "=";
		char delim3[] = ":";
		char delim4[] = "/";

		printf("#> ");
		fflush(stdout);
		fgets(input, sizeof(input), stdin);

		relPath = getenv("PATH");
		printf("%s\n", relPath);

		homePath = getenv("HOME");
		printf("%s\n", homePath);


		if(strlen(input) > 0 && input[strlen(input)-1] == '\n'){
			input[strlen(input)-1] = '\0';
		}

		command = input;

		while(hasSpaces(command[0])){
			command++;
		}

		// check Commands
		if(strncmp(command,"set",3) == 0){
			//do ++ on command
			printf("%s\n", command);
			command += 4;

			// change PATH location
			if(strncmp(command,"PATH",4) == 0){
				// used to get locations of all paths
				printf("HERE\n");

			}

			// change HOME location
			else if(strncmp(command,"HOME",4) == 0){

			}

			// INVALID set parameter
			else {
				printf("Invalid parameter\n");
			}


		}

		//other Commands
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

		else{
			printf("ALSOPOOP\n");
		}
	}
}
