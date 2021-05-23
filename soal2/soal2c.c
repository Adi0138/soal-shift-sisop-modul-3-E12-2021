#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<stdlib.h>

int main() {
    	int pipe1[2], pipe2[2];
    	pid_t pid;

    	if(pipe(pipe1) == -1) {
        	fprintf(stderr, "Pipe failed");
        	return 1;
    	}
    	if(pipe(pipe2) == -1) {
        	fprintf(stderr, "Pipe failed");
        	return 1;
    	}

    	pid = fork();
    	if(pid < 0) {
        	fprintf(stderr, "fork failed");
        	return 1;
    	} 
	    else if(pid == 0) {
        	dup2(pipe1[1], 1);

        	close(pipe1[0]);
        	close(pipe1[1]);

        	char *argv[] = {"ps", "aux", NULL};
        	execv("/usr/bin/ps", argv);

        	perror("Bad exec ps");
        	_exit(1);
    	} 
	    else {
        	pid = fork();
        	if(pid < 0) {
            		fprintf(stderr, "fork failed");
            		return 1;
        	} 
		      else if(pid == 0) {
			          dup2(pipe1[0], 0);

            		dup2(pipe2[1], 1);

            		close(pipe1[0]);
            		close(pipe1[1]);
            		close(pipe2[0]);
            		close(pipe2[1]);

            		char *argv[] = {"sort", "-nrk", "3,3", NULL};
            		execv("/usr/bin/sort", argv);

            		perror("Bad exec sort");
            		_exit(1);
        	}		 
		      else { 
            		close(pipe1[0]);
            		close(pipe1[1]);

            		dup2(pipe2[0], 0);

            		close(pipe2[0]);
            		close(pipe2[1]);

            		char *argv[] = {"head", "-5", NULL};
            		execv("/usr/bin/head", argv);

            		perror("Bad exec head");
            		_exit(1);
        	}
    	}
}
