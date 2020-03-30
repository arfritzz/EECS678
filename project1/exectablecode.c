	/*/RUNNING EXECUTABLES - I dont think we need this anymore
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
				job_count++;
				while(wait(NULL) != pid){}
				break;
		}

		printf("[%d] %d Finished %s\n\n", jobs[job_count].id, jobs[job_count].pid, jobs[job_count].cmd);
	}*/