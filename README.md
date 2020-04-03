# EECS678

### Installation Instructions

To install, download main.c and the Makefile. Then, type 'make test' which creates and runs the executable 'quash'. You should now be inside the quash program.  

To read a command interactiely (with the prompt) or to read commands stored in a file, please run "./quash < filename". This will redirect from standard input from filename. 

### Report

Quite a Shell (Quash) implements many feature similar to the bash shell program. The process of building quash began with supporting the set command. First, "set" sets the value of some variable in an environment. An example of this is if the user types "set MYVAR=arf" then MYVAR=arf will appear in the enviornment. It was also critical that the child process inherit the parent's enviornment. This was tested using "set MYVAR=arf" then running file7 in a child directory which printed MYVAR: arf. We used the setenv() command to accomplish this, which we know is not correct but appeared to produce the desired output. Next, quash can set the HOME and PATH directories. The variable HOME sets the user's home directory while the variable PATH sets the path to search for executables. In implementing this, if the arguement(args) to quash was 'set PATH args' then args was set to the global variable relPath. Then, the UNIX system call setenv("PATH", relPath, 0) was envoked to change the path the searches for executables. Similarly, if the argument was 'set HOME args' the  gloabl variable homePath was changed using setenv("HOME", homePath, 1). 

Quash also implements cd, quit, and exit commands. The cd command, or change directories was implemented using the UNIX system call chdir(). If the cd command has no arguments(args) then the directory is changed to the home directory using chdir(getenv("HOME")). Otherwise, if the cd has args then the incomming arguement is added to the current path using the variable prevDirectory and the directory is changed using chdir(prevDirectory). Finally, the 'quit' and 'exit' commands were implemented with the code "exit(0)" which forces program termination.  

Next, we implemented input output redirection and pipes. The character '<' will redirect standard input while the '>' character will redirect standard output. In completing this task, first, the command was parsed to determine a filename and an action. This is done with the "redirect" command. Redirect then calls "executeRedirection" where a parent and child process are created. In the child, a file descriptor, fd, opens the file called filename. Then, dup2 was used to duplicate the file descriptors so the file could either be redirected from standard input (0) or redirected to standard output(1). The parent just waits until the child is finished. Similarly, piping uses the parent/child relationship to redirect the input from the left argument to the right argument. The first child, child1, calles dup2 to redirect the output to the write end of the pipe. Then the parent calls fork again to create a child of the child, child2. This creates two processes where child2 duplicates the input file description, 0, to the write end of the pipe. The output of the pipe command is then printed to the screen. 

Quash also runs executables with arguments and without arguments. For both executables with and without arguments, we created a child process which calls the execlp() version of exec. This call to exec replaces the parent program's image with the child program's image. If the call to exec fails, it means the executable was not found in the PATH. This occurs when exec() returns a value of -1. In this case, we printed the error message "Executable not found."

Finally, quash implements foreground and background processes with the use of &. Background processes were created using a the struct Jobs which includes a process identifier (pid), a integer identifier (id), and the command it is processing. The id is used to keep track of how many jobs are running in the background. When a job is invoked with the & command, it is added to the job list. Then, running the "jobs" command prints all the current jobs running in the background. If the job is finished, then it will not print with the "jobs" command. For example, if the user runs "sleep 30 &" then jobs they will see [JOBID] PID sleep 30. Once 30 seconds has passed, the user will no longer see the sleep 30 job in the jobs list. 

We developed many methods to test quash before sumbitting. First, as we wrote pieces of the program for each command, we tested them individually with all the edge cases we could think about. We generally used the command line interface for testing. But, we also ran the examples posted on blackboard to ensure our script could support reading commands from a file. For example, if the command was 'cd' we ensured both of us could naviage our personal directories while also testing for edges cases like if the user tries to change directories to a directory that does not exist. 

When testing, we did find a few bugs. First, when invoking the command to run a background process such as "sleep 30 &" The project guidelines state that we must print "[JOB ID] PID running in background" when a background process begins running. This causes our problem as the "QUASH>" prompt is printed before the "[JOB ID] PID running in background". But, once you type a command into the blank line, the prompt goes back to the correct line. It doesn't change anything in the execution, the prompt line is just out of order. Also, we tried the command "ls | grep "a"" and we got an error for grep. I'm not sure if I was not using the command properly but the error was not in piping, it was with grep.

Finally, for extra credit, we also implemented the kill command using kill(pid, signum). We tested this using "sleep 100 &" then "jobs" then "kill 1 PID". Also, I tested a few commands with multiple pipes like "cat test1.txt | more | wc" and quash responded with the correct word count. Therefore, I think multiple pipes may work based on the way we implemented piping.  
