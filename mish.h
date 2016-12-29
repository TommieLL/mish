#ifndef MISH_H
#define MISH_H

//getline()
#define _GNU_SOURCE

/*
 * included libraries.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * Included files.
 */
#include "parser.h"
#include "execute.h"
#include "sighant.h"

#define WROPTS O_WRONLY | O_CREAT | O_EXCL
#define RDOPTS O_RDONLY

#define exit_if_error(ret_value) \
    do { \
        if (ret_value < 0) { \
            perror("ret_value"); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

/*
 * Partially applied functions on redirect and dupPipe.
 */
#define redirect_partial(file, fd) \
    exit_if_error(redirect(file, fd == STDIN_FILENO ? RDOPTS : WROPTS, fd))
#define dup_partial(pip, fd) \
    exit_if_error(dupPipe(pip, fd == STDIN_FILENO ? READ_END : WRITE_END, fd))
    

#define redirect_stdin(infile) redirect_partial(infile, STDIN_FILENO)
#define redirect_stdout(outfile) redirect_partial(outfile, STDOUT_FILENO)
#define dup_stdin(pip) dup_partial(pip, STDIN_FILENO)
#define dup_stdout(pip) dup_partial(pip, STDOUT_FILENO)


/*
 * A list of all running processes.
 */
extern pid_t pid[MAXCOMMANDS];

/*
 * Return values for builtin commands
 */
typedef enum internal {
        INTERNAL_COMMAND_EXIT =-1, INTERNAL_COMMAND_SUCCESS, NO_INTERNAL_COMMAND
} internal_command;
/*
 * A function pointer to builtin command
 */
typedef internal_command (*internal)(char **);


void prompt(void);
void run(void);
internal_command internal_exit(char **argv);
void action(int numCommands, command *cmd);
internal_command call_internal(char **argv);
internal_command cd(char **argv);


#endif
