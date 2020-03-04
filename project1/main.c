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
char** PATH;
int path_Size = 0;
char* HOME;
char* commandChecker;
//char* command_SET = "set";

int main(int argc, char **argv, char **envp) {
	pid_t my_pid;
	my_pid = getpid();
	// for(int i=0; i<sizeof(envp); i++){
	// 	printf("%s\n", envp[i]);
	// }

	while(1){
		char command[64];
		char delim[] = " ";
		char delim2[] = "=";
		char delim3[] = ":";
		char delim4[] = "/";

		printf("#> ");
		fflush(stdout);
		fgets(command, sizeof(command), stdin);

		commandChecker = strtok(command, delim);

		printf("%s\n", commandChecker);


		// check Commands
		if(strcmp(commandChecker,"set") == 0){
			char* parameterChecker = commandChecker;
			parameterChecker = strtok (NULL, delim2);
			printf("%s\n", parameterChecker);


			// change PATH location
			if(strcmp(parameterChecker,"PATH") == 0){
				char* pathChecker = parameterChecker;
				pathChecker = strtok (NULL, delim3);

				// used to get locations of all paths
				do {
					printf("%s\n", pathChecker);
					if(pathChecker = strtok (NULL, delim3)){
						//continue
					}
					else{
						pathChecker = NULL;
					}
				} while(pathChecker != NULL);


			}

			// change HOME location
			else if(strcmp(parameterChecker,"HOME") == 0){

			}

			// INVALID set parameter
			else {
				printf("Invalid parameter\n");
			}


		}

		//other Commands
		else if(strcmp(commandChecker,"cd") == 0){

		}

		else if(strcmp(commandChecker,"quit") == 0){
			exit(0);
		}

		else if(strcmp(commandChecker,"exit") == 0){
			exit(0);
		}

		else if(strcmp(commandChecker,"jobs") == 0){

		}

		else{
			printf("ALSOPOOP\n");
		}
	}
}
