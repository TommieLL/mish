/* sighant.c
 * Tommie Lindberg
 */

#include "sighant.h"

pid_t pid[MAXCOMMANDS];

/*
 * Purpose: To build a struct that contains the signal
 * handler function and the flags.
 */
void set_sighandler(void) {
    struct sigaction sa;
    sa.sa_handler = &handler;
    sa.sa_flags = SA_RESTART;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

/*
 * Purpose: To determine what should happen when
 * a signal has been recorded.
 */
void handler(int sig) {
    for (int i = 0; i < MAXCOMMANDS; i++) {
        if (pid[i] == -1) {
            continue;
        }

        if (kill(pid[i], sig) < 0) {
            fprintf(stderr, "kill: %d %s\n",pid[i], strerror(errno));
            fflush(stderr);
        }

        pid[i] = -1;
    }
}
