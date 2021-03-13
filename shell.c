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
#include <readline/readline.h>
#include <stddef.h>

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
* Print jobs
*
* @returns 0 if successful and -1 if otherwise
*/
int job_cmd();


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
    {"jobs", job_cmd}
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
    int size = sizeof(builtin_cmd) / sizeof(struct built_in);
    while (i != size) {
        if (strcmp(args[0], builtin_cmd[i].user) == 0) {
            return builtin_cmd[i].func(argc, args);
        }
        ++i;
    }

    return -1;
}

/**
* job entry struct
*/ 
struct job_entry
{
    int job_num; 
    char *job;
}; 

/**
* list of jobs 
*/ 
struct job_entry *job_list; 

static int job_count = 0; 

/**
 * initializes job list   
 */
void job_init(void)
{
    job_list = calloc(10, sizeof(struct job_entry)); //10 is the max jobs
}

/**
 * adds the job to the jobs list and also adds the job name and the job pid
 */
void add_job(char *cmd, int pid) 
{ 
    int len = strlen(cmd);
    if(job_list[job_count % 10].job == NULL){
        job_list[job_count].job = calloc(len, sizeof(char) + 1); 
    } else {
        job_list[job_count % 10].job = realloc(job_list[job_count % 10].job, sizeof(char) * len + 1); 
    }
    strcpy(job_list[job_count].job, cmd); 
    job_list[job_count].job_num = pid; 
    job_count++; 
}

/**
 * prints out all the items in the history list 
 */
int job_cmd()
{
    int i;
    for(i = 0; i < job_count; i++){
        if(job_list[i].job != NULL) {
            printf("%s\n",  job_list[i].job);
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
void sigint_handler(int signo) 
{
    if (isatty(STDIN_FILENO)) {
        printf("\n");
        puts(prompt_line());
        rl_on_new_line();
        rl_replace_line("", 1);
        rl_redisplay();

        fflush(stdout); 
    }
}

/**
 * signal for if the background job has completed to remove it from the list 
 * @param signo 
 */
void child_handler(int signo) 
{
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    int i;
    for(i = 0; i < job_count; i++) {
        if(job_list[i].job_num == pid) {
            free(job_list[i].job); 
            job_list[i].job_num = 0; 
            job_list[i].job = NULL;
        }
    }
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
    signal(SIGINT, sigint_handler); 
    signal(SIGCHLD, child_handler);
    init_ui(); 
    hist_init(100);
    job_init();

    char *command;
    while (true) {
        command = read_command();
       
        if (command == NULL)
        {
            break;
        }

        char in[100];
        strcpy(in, command);

        LOG("Input command: %s\n", command);

        char *args[4096] = { 0 }; //POSIX
        int tokens = 0;
        char *next_tok = command;
        char *curr_tok;
        char *temp = strndup(command, strlen(command));

        bool is_hist = false;

        if (command[0] == '!') {
            if (command[1] == '!') { //checks if double bang
                char *entry = hist_search_cnum(hist_last_cnum());
                if (entry == NULL) {
                    goto cleanup;
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
                        goto cleanup;
                    }
                    strcpy(next_tok, entry);
                    hist_add(entry);
                    is_hist = true;
                }
                else { //checks if single bang
                    char *cmd = hist_search_cnum(num);
                    if (cmd == NULL) {
                        goto cleanup;
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

        bool is_job = false;
        if (*(command + strlen(command) - 1) == '&') {
            *(command + strlen(command) - 1) = '\0';
            is_job = true;   
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
            goto cleanup;
        }

        char **cmd_ptr = args;

        int result = handle_builtins(tokens, args);
        if (result == 0) {
            goto cleanup;
        }

        struct command_line cmds[_POSIX_ARG_MAX];
        setup_cmd(args, tokens, cmds);

        pid_t child = fork();
        if (child == -1) {
            perror("fork");
            goto cleanup;
        } else if (child == 0) {
            execute_pipeline(cmds);
            int ret = execvp(cmd_ptr[0], cmd_ptr);
            if (ret == -1) {
                perror("execvp");
                close(fileno(stdin));
                close(fileno(stdout));
                close(fileno(stderr));
                return EXIT_FAILURE;
            }
        } else {
            if (is_job == false) {
                int status = 0;
                waitpid(child, &status, 0);
                LOG("Status: %d", status);
                set_result(status);
            } else {
                //add the PID to our job list
                int pid = child;
                add_job(temp, pid); 
            }
        }
cleanup:        
        free(temp);
    }
    hist_destroy();
    free(job_list);
    return 0;
}
