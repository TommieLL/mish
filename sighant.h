/* sighant.h
 * Tommie Lindberg 
 */

#ifndef SIGHANT_H
#define SIGHANT_H

#include "mish.h"

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>


void set_sighandler(void);
void handler(int sig);
#endif
