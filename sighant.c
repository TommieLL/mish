#define _POSIX_SOURCE
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include "sighant.h"


/**
 * @brief Signal handler, sends a signal to kill all started processes.
 * @param sig
 */
 
 pid_t pid[MAXCOMMANDS];
 
void set_signal_handler(){
    struct sigaction sa;
    sa.sa_handler = &signal_handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);
    
    if(sigaction(SIGINT, &sa, NULL) < 0){
        perror(sigaction);
        exit(EXIT_FAILURE);
    }
}
    
void signal_handler(int sig)
{


	for(int i = 0; i < MAXCOMMANDS; i++) {
		if(pid[i] == -1) {
		    continue;
		}
		if(kill(pid[i], sig) < 0) {
		    errno(sig);
		}
		
		pid[i] = -1;
	}
}
