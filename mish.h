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


/*
 * All running processes.
 */
extern pid_t pid[MAXCOMMANDS];

/*
 * Return values for the internal commands.
 */
typedef enum internal {
        INTERNAL_COMMAND_EXIT =-1, INTERNAL_COMMAND_SUCCESS, NO_INTERNAL_COMMAND
} internal_command;
/*
 * A function pointer.
 */
typedef internal_command (*internal)(char **);


void prompt(void);
void run(void);
void action(int numCommands, command *cmd);
internal_command call_internal(char **argv);
internal_command internal_cd(char **argv);
internal_command internal_echo(char **argv);


#endif
