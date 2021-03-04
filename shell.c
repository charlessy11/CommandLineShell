#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "util.h"


void sigint_handler(int signo) {
    printf("\n");
}

int main(void)
{
    init_ui();

    char *command;
    /* Set up our signal handler. SIGINT can be sent via Ctrl+C */
    signal(SIGINT, sigint_handler); 

    while (true) {
        command = read_command();
        if (command == NULL) {
            break;
        }

        LOG("Input command: %s\n", command);

        char *args[4096] = { 0 }; //POSIX
        int tokens = 0;
        char *next_tok = command;
        char *curr_tok;
        /* Tokenize. Note that ' \t\n\r' will all be removed. */
        while ((curr_tok = next_token(&next_tok, " \t\n\r")) != NULL) {
            //checks if user entered "#"
            if (*curr_tok == '#') {
                break;
            }
            args[tokens++] = curr_tok;
        }   
        args[tokens] = (char *) NULL;
        //checks if user didn't input a command
        if (args[0] == NULL) {
            continue;
        }
        //checks if user entered "exit"
        if (strcmp(args[0], "exit") == 0) {
            return 0;
        }
        if (strcmp(args[0], "cd") == 0 && args[1] == NULL) {
            args[1] = getenv("HOME");
        }
        if (chdir(args[1]) != 0) {
            perror("No directory");
        }


        pid_t child = fork();
        if (child == -1) {
            perror("fork");
            continue;
        } 
        else if (child == 0) {
            /* We are the child process */
            int ret = execvp(args[0], args);
            if (ret == -1) {
                perror("execvp");
                close(fileno(stdin));
                close(fileno(stdout));
                close(fileno(stderr));
                return EXIT_FAILURE;
            }
        } 
        else {
            /* We are the parent process */
            int status = 0;
            waitpid(child, &status, 0);
        }
    }

    return 0;
}
