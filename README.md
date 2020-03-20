# EECS678

# Installation Instructions

To install, download main.c and the Makefile. Then, type 'make test' which creates and runs the executable 'quash'. You should now be inside the quash program.  

# Report

Quite a Shell (Quash) implements many feature similar to the bash shell program. The process of building quash began with supporting the set, cd, quit, and exit commands. First, set sets the value of some variable in an environment. The variable HOME sets the user's home directory while the variable PATH sets the path to search for executables. If the arguement(args) to quash was 'set PATH args' then args was set to the global variable relPath. Then, the UNIX system call setenv("PATH", relPath, 0) was envoked to change the path the searches for executables. Similarly, if the argument was 'set HOME args' the  gloabl variable homePath was changed using setenv("HOME", homePath, 1). Next, the cd command, or change directories was implemented using the UNIX system call chdir(). If the cd command has no arguments(args) then the directory is changed to the home directory using chdir(getenv("HOME")). Otherwise, if the cd has args then the incomming arguement is added to the current path using the variable prevDirectory and the directory is changed using chdir(prevDirectory). Finally, the 'quit' and 'exit' commands were implemented by returning 0. 



