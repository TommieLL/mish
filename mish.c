#include "mish.h"

pid_t pid[MAXCOMMANDS];

int main()
{
    memset(pid, -1, sizeof pid);
    set_sighandler();
    run();

    return EXIT_SUCCESS;
}

/**
 * @brief Prints the prompt to stderr.
 */
void prompt(void)
{
	fprintf(stderr,"mish%% ");
	fflush(stderr);
}

/*
 * Purpose: The main loop.
 */
void run(void){
    size_t bufferSize = MAXLINELEN;
    char *buffer = malloc(bufferSize);
    command cmd[MAXCOMMANDS];
    int numCommands;
    internal_command inc;

    do {
        prompt();

        if (getline(&buffer, &bufferSize, stdin) == EOF) {
            //printf("");
            break;
        }

        if ((numCommands = parse(buffer, cmd)) == 0) {
            continue;
        }

        if ((inc = call_internal(cmd[0].argv)) != NO_INTERNAL_COMMAND) {
            continue;
        }

        action(numCommands, cmd);
    }
    while (inc != INTERNAL_COMMAND_EXIT);

    free(buffer);
}
 /*
  * Purpose: Fuckin fork.
  */
void action(int numCommands, command *cmd){
   int pipes[2][2];

    for (int i = 0; i < numCommands; i++) {
        bool is_first = i == 0;
        bool is_last = i == numCommands - 1;

        /* File redirection is only allowed at first and last command. */
        if ((!is_first && cmd[i].infile) || (!is_last && cmd[i].outfile)) {
            fprintf(stderr, "mish%%s: Syntax error\n");
            fflush(stderr);
            return;
        }

        if (!is_last && pipe(pipes[i%2]) < 0) {
            fprintf(stderr,"mish%% Error creating pipe\n");
            fflush(stderr);
        }

        if ((pid[i] = fork()) < 0) {
            fprintf(stderr,"mish%% Error creating pipe");
            fflush(stderr);
        } else if (pid[i] == 0) {
            if (numCommands > 1) {
                if (!is_last) {
                    dup_stdout(pipes[i%2]);
                }
                
                if (!is_first) {
                    dup_stdin(pipes[(i+1)%2]);
                }
            }

            if (is_first && cmd[i].infile) {
                redirect_stdin(cmd[i].infile);
            }
            
            if (is_last && cmd[i].outfile) {
                redirect_stdout(cmd[i].outfile);
            }

            if (execvp(cmd[i].argv[0], cmd[i].argv) < 0) {
                perror("execvp");
                //print_errno();
                exit(EXIT_FAILURE);
            }
        } else {
            /* We create numCommands-1 pipes in total, skip close once. */
            if (is_first) {
                continue;
            }

            if (close(pipes[(i+1)%2][WRITE_END]) < 0 ||
                close(pipes[(i+1)%2][READ_END]) < 0) {
                perror("close");
                //print_errno();
            }
        }
    }

    for (int i = 0; i < numCommands; i++) {
        int inc;

        do {
            waitpid(pid[i], &inc, 0);
        }  while (!WIFEXITED(inc) && !WIFSIGNALED(inc));

        pid[i] = -1;
    }
}
/*
 * Purpose: To check if the typed command is internal.
 * Returns: Value of the internal command or -1 if it's not an internal command.
 */
internal_command call_internal(char **argv){
    static const char *name[] = {"cd", "exit"};
    
    static const internal ptr[] = {&cd, &internal_exit};
    
    for(int i = 0; i < 2; i++){
        if(strcmp(argv[0], name[i]) == 0){
            return ptr[i](argv);
            break;
        }
        }
    return NO_INTERNAL_COMMAND;

}

/*
 * Purpose: If the command is internal.
 */
internal_command cd(char **argv){
    char *dir = argv[1] == NULL ? getenv("HOME") : argv[1];
    if (chdir(dir) != 0) {
        fprintf(stderr,"Change of diretory failed\n");
        fflush(stderr);
	}
	return INTERNAL_COMMAND_SUCCESS;
}

/*
 * exit - Exit mish
 */
internal_command internal_exit(char **argv) {
    argv++;
        return INTERNAL_COMMAND_EXIT;
    
}

