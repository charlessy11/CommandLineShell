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

int cd_cmd(int argc, char *args[]);
int exit_cmd(int argc, char *args[]);
int hist_cmd(int argc, char *args[]);
int handle_builtins(int argc, char *args[]);

/**
 * struct to store built-in cmd info
 */ 
struct built_in {
    char user[80];
    int (*func)(int argc, char *args[]);
};

/**
 * built-ins and their associated functions
 */ 
struct built_in builtin_cmd[] = {
    {"exit", exit_cmd},
    {"cd", cd_cmd},
    {"history", hist_cmd},
};

int cd_cmd(int argc, char *args[]) {
    if (args[1] == NULL) {
        args[1] = getenv("HOME");
    }
    if (chdir(args[1]) != 0) {
        perror("No directory");
        set_result(chdir(args[1]));
    }
    return 0;
}

int exit_cmd(int argc, char *args[]) {
    hist_destroy();
    exit(0);
    return 0;
}

int hist_cmd(int argc, char *args[]) {
    hist_print();
    return 0;
}

int handle_builtins(int argc, char *args[]) {
    int i;
    for (i = 0; i < sizeof(builtin_cmd) / sizeof(struct built_in); i++) {
        if (strcmp(args[0], builtin_cmd[i].user) == 0) {
            return builtin_cmd[i].func(argc, args);
        }
    }
    return -1;
}

void sigint_handler(int signo) {
    signal(signo, SIG_IGN);
    fflush(stdout);
    printf("\n");
}

int main(void)
{
    init_ui();
    signal(SIGINT, sigint_handler); 
    hist_init(100);

    char *command;
    while (true) {
        command = read_command();
       
        if (command == NULL)
        {
            break;
        }

        char in[1000];
        strcpy(in, command);

        LOG("Input command: %s\n", command);

        char *args[4096] = { 0 }; //POSIX
        int tokens = 0;
        char *next_tok = command;
        char *curr_tok;

        char bang[2] = { 0 };
        bang[0] = *(command);
        bang[1] = '\0';

        char bangbang[3] = { 0 };
        bangbang[0] = *(command);
        bangbang[1] = *(command + 1);
        bangbang[2] = '\0';

        bool is_hist = false;

        if (strcmp(bangbang, "!!") == 0) {
            char *entry = hist_search_cnum(hist_last_cnum());
            if (entry == NULL) {
                continue;
            }
            strcpy(next_tok, entry);
            hist_add(hist_search_cnum(hist_last_cnum()));
            is_hist = true;
        }
        else if (strcmp(bang, "!") == 0) {
            char prefix[10] = { 0 };
            substr(prefix, next_tok, 1, strlen(next_tok));
            int num = atoi(prefix);
            if (num == 0) {
                char *entry = hist_search_prefix(prefix);
                if (entry == NULL) {
                    continue;
                }
                strcpy(next_tok, entry);
                hist_add(entry);
                is_hist = true;
            }
            else {
                char *cmd = hist_search_cnum(num);
                if (cmd == NULL) {
                    continue;
                }
                strcpy(next_tok, cmd);
                hist_add(hist_search_cnum(num));
                is_hist = true;
            }    
        }
        if (is_hist == false) {
            hist_add(in);
        }

        sum_count();

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

        int result = handle_builtins(tokens, args);
        if (result == 0)
        {
            continue;
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
    hist_destroy();
    return 0;
}
