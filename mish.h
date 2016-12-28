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


/*
 * A list of all running processes.
 */
extern pid_t pid[MAXCOMMANDS];

/*
 * Return values for builtin commands
 */
typedef enum internal {
    INTERNAL_COMMAND_EXIT = -1, INTERNAL_COMMAND_SUCCESS, NO_INTERNAL_COMMAND
} internal_command;
/*
 * A function pointer to builtin command
 */
typedef internal_command (*internal)(char **);

#endif
