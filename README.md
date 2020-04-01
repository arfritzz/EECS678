# EECS678

### Installation Instructions

To install, download main.c and the Makefile. Then, type 'make test' which creates and runs the executable 'quash'. You should now be inside the quash program.  

To read a command interactiely (with the prompt) or to read commands stored in a file, please run "./quash < filename". This will redirect from standard input. 

### Report

Quite a Shell (Quash) implements many feature similar to the bash shell program. The process of building quash began with supporting the set, cd, quit, and exit commands. First, "set" sets the value of some variable in an environment. The variable HOME sets the user's home directory while the variable PATH sets the path to search for executables. In implementing this, if the arguement(args) to quash was 'set PATH args' then args was set to the global variable relPath. Then, the UNIX system call setenv("PATH", relPath, 0) was envoked to change the path the searches for executables. Similarly, if the argument was 'set HOME args' the  gloabl variable homePath was changed using setenv("HOME", homePath, 1). Next, the cd command, or change directories was implemented using the UNIX system call chdir(). If the cd command has no arguments(args) then the directory is changed to the home directory using chdir(getenv("HOME")). Otherwise, if the cd has args then the incomming arguement is added to the current path using the variable prevDirectory and the directory is changed using chdir(prevDirectory). Finally, the 'quit' and 'exit' commands were implemented by returning 0 to main when called. 

Next, we implemented input output redirection and pipes. The character '<' will redirect standard input while the '>' character will redirect standard output. In completing this task, first, the command was parsed to determine a filename and an action. This is done with the "redirect" command. Redirect then calls "executeRedirection" where a file descriptor, fd, opens the file called filename. Then duplicated the file descriptor 0 for standard input redirection and 1 for standard output redireciton. Before executing redirection, the process must be forked where the child process executes the redirection and the parent waits until the child is finished. 



We developed many methods to test quash before sumbitting. First, as we wrote pieces of the program for each command, we tested them individually with all the edge cases we could think about. We generally used the command line interface for testing. For example, if the command was 'cd' we ensured both of us could naviage our personal directories while also testing for edges cases like if the user tries to change directories to a file that does not exist. We also tested various commands using the command.txt file to ensure quash could read command interactively. 
