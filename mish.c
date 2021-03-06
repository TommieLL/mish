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
  * Purpose: To fork if it's a child process 
  * and execute commands from args/input
  */
void action(int numCommands, command *cmd){
   int pipes[2][2];

    for (int i = 0; i < numCommands; i++) {
        bool is_first = i == 0;
        bool is_last = i == numCommands - 1;

        /* File redirection is only allowed at first and last command. */
        if ((!is_first && cmd[i].infile) || (!is_last && cmd[i].outfile)) {
            fprintf(stderr, "Syntax error\n");
            fflush(stderr);
            return;
        }

        if (!is_last && pipe(pipes[i%2]) < 0) {
            fprintf(stderr,"Error creating pipe\n");
            fflush(stderr);
        }

        if ((pid[i] = fork()) < 0) {
            fprintf(stderr,"Error creating pipe\n");
            fflush(stderr);
        } else if (pid[i] == 0) {
            if (numCommands > 1) {
                if (!is_last) {
                    
                    if(dupPipe(pipes[i%2], STDOUT_FILENO == STDIN_FILENO ?
                     READ_END : WRITE_END, STDOUT_FILENO) < 0){
                    fprintf(stderr, "%d: %s\n",STDOUT_FILENO, strerror(errno));
                    fflush(stderr);
                    _exit(EXIT_FAILURE);
                    }
                }
                
                if (!is_first) {
                    
                    if(dupPipe(pipes[(i+1)%2], STDIN_FILENO == STDIN_FILENO ?
                     READ_END : WRITE_END, STDIN_FILENO) < 0){
                        fprintf(stderr, "%d: %s\n",STDIN_FILENO, strerror(errno));
                        fflush(stderr);
                        _exit(EXIT_FAILURE);
                    }
                }
            }

            if (is_first && cmd[i].infile) {
                
                if(redirect(cmd[i].infile, STDIN_FILENO == STDIN_FILENO ?
                 RDOPTS : WROPTS, STDIN_FILENO) < 0){
                    fprintf(stderr, "%s: %s\n",cmd[i].infile, strerror(errno));
                    fflush(stderr);
                    _exit(EXIT_FAILURE);
                }
            }
            
            if (is_last && cmd[i].outfile) {
                
                if(redirect(cmd[i].outfile, STDOUT_FILENO == STDIN_FILENO ?
                 RDOPTS : WROPTS, STDOUT_FILENO) < 0){
                    fprintf(stderr, "%s: %s\n",cmd[i].outfile, strerror(errno));
                    fflush(stderr);
                    _exit(EXIT_FAILURE);
                }
            }

            if (execvp(cmd[i].argv[0], cmd[i].argv) < 0) {
                fprintf(stderr, "%s: %s\n",cmd[i].argv[0], strerror(errno));
                fflush(stderr);
                _exit(EXIT_FAILURE);
            }
        } else {
            /* We create numCommands-1 pipes in total, skip close once. */
            if (is_first) {
                continue;
            }

            if (close(pipes[(i+1)%2][WRITE_END]) < 0 ||
                close(pipes[(i+1)%2][READ_END]) < 0) {
                fprintf(stderr, "close: %s\n", strerror(errno));
                fflush(stderr);
               
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
    static const char *name[] = {"cd","echo"};
    
    static const internal ptr[] = {&internal_cd, &internal_echo};
    
    for(int i = 0; i < 2; i++){
        if(strcmp(argv[0], name[i]) == 0){
            return ptr[i](argv);
            break;
        }
        }
    return NO_INTERNAL_COMMAND;

}

/*
 * Purpose: Change directory.
 */
internal_command internal_cd(char **argv){
    if (chdir(argv[1]) != 0) {
        fprintf(stderr, "cd: %s: %s\n",argv[1], strerror(errno));
        fflush(stderr);
	}
	return INTERNAL_COMMAND_SUCCESS;
}

/*
 * Purpose: A internal "echo" function.
 */
internal_command internal_echo(char **argv){
    int i = 1;
    while(argv[i] != NULL){
    char *output= argv[i+1] == NULL ? "%s" : "%s ";
    fprintf(stdout,output,argv[i]);
    argv++;
    }
    fprintf(stdout,"\n");
    fflush(stdout);
    return INTERNAL_COMMAND_SUCCESS;
}


