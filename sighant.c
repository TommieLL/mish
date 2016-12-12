#define _POSIX_SOURCE
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include "sighant.h"


/**
 * @brief Signal handler, sends a signal to kill all started processes.
 * @param sig
 */
void signal_handler(int sig)
{


	for(int i = 0; i < nCom; i++) {
		if( kill(pid_array[i], sig) < 0) {
			fprintf(stderr,"There was an error sending the signal to the process");

		}
	}
}
