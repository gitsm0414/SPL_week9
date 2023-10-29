#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<fcntl.h>

#define ARGSIZE 100

char* cmd=NULL;  //pointer for command string, global
char** args;  //pointer array for each arguments in command, global

void cleanup(void){ //free global malloc variables
	if(cmd!=NULL) free(cmd);
	if(args!=NULL) free(args);
}

int main(){
	char* ptr;	
	char path[100];

	size_t size;
	ssize_t cmdread; //related to cmd reading error
	pid_t pid;

	int cmp; //related to 'quit'
	int status;

	atexit(cleanup);

	size = 100;
	
	//memmory allocating
	args = (char**)malloc(sizeof(char*) * ARGSIZE);

	do{
		//getline error handling
		cmd = NULL;
		if((cmdread = getline(&cmd, &size, stdin)) == -1){
			perror("command reading error");
		       	exit(1);
		}
		cmd[strlen(cmd)-1] = '\0';

		//quit
		if((cmp = strcmp("quit",cmd))==0){
			//if parent has no child, then wait() returns -1 right away
			//parent waits all possible child processes then quit
			while(1){
				pid = wait(NULL);
				if(pid == -1) break;
			}
			break;
		}
		
		//checking if cmd is one of >, <, |
		char* check;
		if((check = strchr(cmd, '>'))!=NULL){
			int i = 0;
			ptr = strtok(cmd, " ");
			while(ptr != NULL){
				if(*ptr == '>') break;
				args[i++] = ptr;
				ptr = strtok(NULL, " ");
				
			}
			args[i] = NULL;
			
			

			//path
			sprintf(path, "/bin/%s", args[0]);
		
			//if child, then execute the cmd
			if(fork() == 0){
				char* filename;
				int fd;
				filename = strtok(NULL, " ");
	
				if((fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644))<0){
					perror("redirection file open error");
					exit(1);
				}
				dup2(fd, STDOUT_FILENO);
				close(fd);

				int exe;
				if((exe = execv(path, args)) == -1){
					perror("command executing error");
					exit(1);
				}
				exit(0);	
			}
			else //if parent, wait until child process ends
			{ 
				wait(&status);
			}
		}
		else if((check = strchr(cmd, '<'))!=NULL){
			int i = 0;
			ptr = strtok(cmd, " ");
			while(ptr != NULL){
				if(*ptr == '<') break;
				args[i++] = ptr;
				ptr = strtok(NULL, " ");
				
			}
			args[i] = NULL;
			
			

			//path
			sprintf(path, "/bin/%s", args[0]);
		
			//if child, then execute the cmd
			if(fork() == 0){
				char* filename;
				int fd;
				filename = strtok(NULL, " ");
	
				if((fd = open(filename, O_RDONLY))<0){
					perror("redirection file open error");
					exit(1);
				}
				dup2(fd, STDIN_FILENO);
				close(fd);

				int exe;
				if((exe = execv(path, args)) == -1){
					perror("command executing error");
					exit(1);
				}
				exit(0);	
			}
			else //if parent, wait until child process ends
			{ 
				wait(&status);
			}
		}
		else if((check = strchr(cmd, '|'))!=NULL){
			int fd[2]; // File descriptors for the pipe
    			if(pipe(fd) == -1) { // Create a pipe
        			perror("pipe error");
  			        exit(1);
    			}
			
			int i = 0;
			ptr = strtok(cmd, " ");
			while(*ptr != '|'){
				args[i++] = ptr;
				ptr = strtok(NULL, " ");				
			}
			args[i] = NULL;
	
			//path		
			sprintf(path, "/bin/%s", args[0]);	
			//if child, then execute the cmd
			if(fork() == 0){
				close(fd[0]);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);

				int exe;
				if((exe = execv(path, args)) == -1){
					perror("command executing error");
					exit(1);
				}
				exit(0);	
			}
			

			i = 0;
			ptr = strtok(cmd, " ");
			while(ptr != NULL){
				args[i++] = ptr;
				ptr = strtok(NULL, " ");				
			}
			args[i] = NULL;
	
			//path		
			sprintf(path, "/bin/%s", args[0]);	
			//if child, then execute the cmd
			if(fork() == 0){
				close(fd[1]);
				dup2(fd[0], STDIN_FILENO);
				close(fd[0]);
				int exe;
				if((exe = execv(path, args)) == -1){
					perror("command executing error");
					exit(1);
				}
				exit(0);	
			}
			close(fd[0]);
			close(fd[1]);
			wait(NULL);
			wait(NULL);
			
				
		}
		else
		{
		//cmd parsing -> args
		int i = 0;
		ptr = strtok(cmd, " ");
		while(ptr != NULL){
			args[i++] = ptr;
			ptr = strtok(NULL, " ");
			}
		args[i] = NULL;

		//path
		sprintf(path, "/bin/%s", args[0]);
		
		//if child, then execute the cmd
		if(fork() == 0){
			int exe;
			if((exe = execv(path, args)) == -1){
				perror("command executing error");
				exit(1);
			}
			exit(0);	
		}
		else //if parent, wait until child process ends
		{ 
			wait(&status);
		}
		
		}



		
			 
	}while(1);
	
	exit(0);
}
