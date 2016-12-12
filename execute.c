#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "execute.h"

/* Duplicate a pipe to a standard I/O file descriptor, and close both pipe ends
 * Arguments:	pip	the pipe
 *		end	tells which end of the pipe shold be dup'ed; it can be
 *			one of READ_END or WRITE_END
 *		destfd	the standard I/O file descriptor to be replaced
 * Returns:	-1 on error, else destfd
 */
int dupPipe(int pip[2], int end, int destfd){
	
	if( end == READ_END){
		dup2(pip[0],destfd);
		return destfd;
	}
	else if( end == WRITE_END){
		dup2(pip[1], destfd);
		return destfd;
	}else{
		return (-1);
	}
	

	return destfd;
}
/* Redirect a standard I/O file descriptor to a file
 * Arguments:	filename	the file to/from which the standard I/O file
 * 				descriptor should be redirected
 * 		flags	indicates whether the file should be opened for reading
 * 			or writing
 * 		destfd	the standard I/O file descriptor which shall be
 *			redirected
 * Returns:	-1 on error, else destfd
 */
int redirect(char *filename, int flags, int destfd){
	
	if(filename != NULL){
		close(destfd);
		//S_IXOTH =Set execution rights for other users to true.
		destfd = open(filename,flags,0777);
		if(destfd < 0){
			perror(filename);
		}
	}
	return destfd;
}