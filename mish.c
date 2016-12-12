#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include "mish.h"

/**
 * @brief Prints the prompt to stderr.
 */
void prompt(void)
{
	fprintf(stderr,"mish%% ");
	fflush(stderr);
}

int main(void)
{
	pid_t pid;
	int *pip = NULL;
	int  i;
	command *data= malloc(sizeof(*data));
	command comline[2];
	comline[0]=*data;
	char command_line[MAXWORDS];
	
		//Check for signal
		if(signal(SIGINT, signal_handler) != 0) {
			fprintf(stderr,"Couldn't register the signal\n");
			fflush(stderr);
		}
		prompt();

	// Loops until fgets gets EOF
	while(fgets(command_line,MAXWORDS,stdin)){

		nCom = parse(command_line, comline);

		
		//Alloc memory and creates the pipe if ther's more  than 1 command
		if(nCom > 1) {
			pip = (int *)malloc((nCom-1)*2*sizeof(int));
			if(pip == NULL) {
				fprintf(stderr,"Couldn't allocate memory for the pipe");
				fflush(stderr);
				prompt();
				continue;
			}
			for(i = 0; i <(nCom-1) ;i++){
				if(pipe(&pip[i*2]) != 0) {
					fprintf(stderr,"Couldn't create the pipe");
					fflush(stderr);
				}
			}
		}
		for(i = 0; i<nCom;i++){

			//cd function
			if(strcmp(comline[i].argv[0],"cd")==0) {

				if(chdir(comline[i].argv[1])<0) {
					fprintf(stderr,"Change of diretory failed\n");
					fflush(stderr);
	

					}
				}

			
			//If it's a child process
			if((pid = fork()) == 0) {

				//input redirect
				if(redirect(comline[i].infile, O_RDONLY,0) <0) {
					fprintf(stderr,"Couldn't redirect to input file");
					fflush(stderr);
				}
				//output redirect
				if(redirect(comline[i].outfile, O_CREAT|O_WRONLY|O_EXCL,1)<0) {
					fprintf(stderr,"Couldn't redirect to output file");
					fflush(stderr);
				}
				if( i <(nCom -1)) {
					*pip = pip[(i*2) +1];
					if(dupPipe(pip,WRITE_END, STDOUT_FILENO) <0) {
						fprintf(stderr,"Error number %d\n", errno);
						fflush(stderr);
						exit(EXIT_FAILURE);
					}
				}
				
				if(i > 0) {
					*pip = pip[(i -1)*2];
					if(dupPipe(pip,READ_END,STDIN_FILENO)<0) {
						fprintf(stderr,"Error number %d\n", errno);
						fflush(stderr);
						exit(EXIT_FAILURE);
					}
				}
				
				//
				if(pip!=NULL) {
					for(int i = 0; i<(nCom-1); i++) {
						if(close(pip[i*2])<0){fprintf(stderr,"Close was interrupted by a signal %d", errno);}
						if(close(pip[(i*2)+1])<0){fprintf(stderr,"Close was interrupted by a signal %d", errno);}
						fflush(stderr);
					}
				}
				//External commands
				if(execvp(comline[i].argv[0], comline[i].argv)<0){fprintf(stderr,"Error: %d", errno); fflush(stderr);}
			}else if( pid > 0) {
				//Insert all the pipes in an array
				pid_array[i] = pid;
			} else {
				fprintf(stderr,"Error: fork failed error %d", errno);
				fflush(stderr);
			}
		}
		//Try to close the pipes
		for(i = 0 ; i < (nCom-1); i++) {
			if(close(pip[i*2])<0){fprintf(stderr,"Close was interrupted by a signal %d", errno);}
			if(close(pip[i*2+1])<0){fprintf(stderr,"Close was interrupted by a signal %d", errno);}
			fflush(stderr);
		}
		//the sleep function
		for(i=0; i < nCom; i++) {
			if(wait(pid_array)<0){fprintf(stderr,"Wait was interrupted by a signal %d", errno); fflush(stderr);}
		}
		fflush(stdout);
		fflush(stderr);
		//Close and free the pipes
		 if(pip != NULL){
			 for(i = 0; i <(nCom-1); i++){
				 if(close(pip[i*2])<0){{fprintf(stderr,"Close was interrupted by a signal %d", errno);}
				 if(close(pip[(i*2)+1])<0){fprintf(stderr,"Close was interrupted by a signal %d", errno);}
				 fflush(stderr);
			 }
			 free(pip);
			 pip = NULL;
		 }
		 nCom=0;
		 prompt();
	}
}
	free(data);

	return 0;
}
