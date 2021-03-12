/**
 * @file
 *
 * The shell
 */

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
#include "pipe.h"

/**
* Change directory
*
* @param argc
* @param args
*
* @returns 0 if successful and -1 if otherwise
*/
int cd_cmd(int argc, char *args[]);

/**
* Exit command
*
* @returns 0 if successful and -1 if otherwise
*/
int exit_cmd();

/**
* History command
*
* @returns 0 if successful and -1 if otherwise
*/
int hist_cmd();

/**
* Handles built-ins
*
* @param argc
* @param args
*
* @returns 0 if successful and -1 if otherwise
*/
int handle_builtins(int argc, char *args[]);

/**
* Print job list
*
* @return 0 if successful
*/
int print_jobs();

/**
* Add job
*
* @param cmd
*/
void add_job(const char *cmd);


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
    {"cd", cd_cmd},
    {"exit", exit_cmd},
    {"history", hist_cmd},
    {"jobs", print_jobs}
};

/**
* Change directory
*
* @param argc
* @param args
*
* @returns 0 if successful and -1 if otherwise
*/
int cd_cmd(int argc, char *args[]) {
    if (args[1] == NULL) {
        args[1] = getenv("HOME");
        chdir(args[1]);
    }
    if (chdir(args[1]) != 0) {
        set_result(chdir(args[1]));
        perror("No directory");
    }
    return 0;
}

/**
* Exit command
*
* @returns 0 if successful and -1 if otherwise
*/
int exit_cmd() {
    hist_destroy();
    exit(0);
    return 0;
}

/**
* History command
*
* @returns 0 if successful and -1 if otherwise
*/
int hist_cmd() {
    hist_print();
    return 0;
}

/**
* Handles built-ins
*
* @param argc
* @param args
*
* @returns 0 if successful and -1 if otherwise
*/
int handle_builtins(int argc, char *args[]) {
    int i = 0;
    while (i != sizeof(builtin_cmd) / sizeof(struct built_in)) {
        if (strcmp(args[0], builtin_cmd[i].user) == 0) {
            return builtin_cmd[i].func(argc, args);
        }
        ++i;
    }

    return -1;
}

/**
 * Job entry struct
 */
struct job_entry
{
    int job_num;
    char job[100];
};

/**
 * List of job entries
 */
struct job_entry job_list[10];

static int job_count = 0;
static int counter = 0;
static int max_jobs = 10;

/**
 * Add jobs
 *
 * @param cmd the command to be addes
 */
void add_job(const char *cmd)
{
    if (job_count >= max_jobs) {
        counter++;
        job_count = 0;
    }
    strcpy(job_list[job_count].job, cmd);
    job_list[job_count].job_num = job_count;
    job_count++;
}

/**
 * Print jobs
 */
int print_jobs()
{   
    // int i = job_count;
    // if (counter * max_jobs > 0) {
    //     while (i != max_jobs) {
    //         printf("%s\n", job_list[i].job);
    //     }
    //     ++i;
    // }
    // int j = 0;
    // while (j != job_count) {
    //     printf("%s\n", job_list[i].job);
    //     ++j;
    // }
    pid_t pid = fork();
    int i;
    for (i = 0; i < max_jobs; i++) {
        if (pid == 0) {//if pid == 0 then dont print
            continue;
        } 
        else {
            printf("%s\n", job_list[i].job);
            //else print jobs
        }
    }


    fflush(stdout);
    return 0;
}

/**
* Handles sigint
*
* @param signo the signal number
*
*/
void sigint_handler(int signo) {
    signal(signo, SIG_IGN);
    fflush(stdout);
    printf("\n");
}

/**
* Creates substring
*
* @param dest the destination
* @param src the source
* @param start the start
* @param end the end
*
* @returns dest the destination
*/
char* substr(char *dest, const char *src, int start, int end)
{
    while (end != 0) {
        *dest = *(src + start);
        ++dest;
        ++src;
        --end;
    }
    *dest = '\0';
    return dest;
}


/**
* The main
*
*/
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

        bool is_hist = false;

        if (command[0] == '!') {
            if (command[1] == '!') { //checks if double bang
                char *entry = hist_search_cnum(hist_last_cnum());
                if (entry == NULL) {
                    continue;
                }
                strcpy(next_tok, entry);
                hist_add(hist_search_cnum(hist_last_cnum()));
                is_hist = true;
                } else {
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
                else { //checks if single bang
                    char *cmd = hist_search_cnum(num);
                    if (cmd == NULL) {
                        continue;
                    }
                    strcpy(next_tok, cmd);
                    hist_add(hist_search_cnum(num));
                    is_hist = true;
                }    
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

        char **cmd_ptr = args;
        char **next_cmd = NULL;
        bool is_job = false;

        for (int i = 0; i < tokens; i++) {
            if (args[i] != NULL && strcmp("|", args[i]) == 0) {
                // printf("Pipe found!!");
                // execute_pipeline(in);
                struct command_line cmds[_POSIX_ARG_MAX];
                setup_cmd(args, tokens, cmds);

                pid_t child = fork();
                if (child == 0) {
                    execute_pipeline(cmds);
                } else if (child == -1) {
                    perror("fork");
                } else {
                    int status;
                    waitpid(child, &status, 0);
                }
                args[i] = 0;
                next_cmd = &args[i + 1];
            }
            if (args[i] != NULL && strcmp("&", args[i]) == 0) {
                add_job(in); 
                args[i] = 0;
                is_job = true; 
            }
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
            int ret = execvp(cmd_ptr[0], cmd_ptr);
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
            if (is_job == false) {
                int status = 0;
                waitpid(child, &status, 0);
                LOG("Status: %d", status);
                set_result(status);
            } else {
                //add the PID to our job list
            }
        }
    }
    hist_destroy();
    return 0;
}
