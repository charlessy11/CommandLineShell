/**
 * @file
 *
 * Deals with pipes.
 */

#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#include "pipe.h"

bool statusCode = true;
bool interactive = true;
int numCommands = 0;
bool executing = false;


/**
* Command line struct
*/
struct command_line {
    char **tokens;
    bool stdout_pipe; //determine when you've reached the last command in the pipeline
    char *stdout_file; //decide whether the final result gets written to a file or the terminal
};

// allocate initial memory for a list of 128 command_line structs. 
// TODO: allocate dynamically; malloc one ptr, then realloc as needed
struct command_line* create_commands() {
    struct command_line *commands = malloc(sizeof(struct command_line*) * 128);
    // allocate initial memory for a list of
    for(int i = 0; i < 128; i++) {
        commands[i].tokens = malloc(sizeof(char) * 129); // inital command line capacity of 128 ch
        commands[i].stdout_pipe = false;
        commands[i].stdout_file = NULL;
    }
    return commands; 
}

int free_commands(struct command_line *commands) {
    free(commands);
    return 0;
}

int tokenize(char* command, struct command_line* commands) {
    char *next_tok = command;
    char *curr_tok;
    int index = 0;
    while((curr_tok = next_token(&next_tok, " \t")) != NULL) {
        // check for special characters: | and >
        if(strcmp(curr_tok, "|") == 0) {
            // skip this token, update pipe info, increment command number
            commands[numCommands].stdout_pipe = true;
            commands[numCommands].tokens[index] = NULL; // null terminate
            numCommands++;
            index = 0;
        }
        else if(strcmp(curr_tok, ">") == 0) {
            // set the next argument to the output file
            curr_tok = next_token(&next_tok, " \t");
            commands[numCommands].stdout_file = curr_tok;
        }
        else {
            // add token to command
            commands[numCommands].tokens[index++] = curr_tok;
        }
    }
    commands[numCommands].tokens[index] = NULL; // null terminate
    if(numCommands == 0 && index != 0) numCommands = 1;
    return 0;
}

void execute_pipeline(struct command_line *cmds)
{
    /* base case */
    if (cmds->stdout_pipe == false) {
        if (cmds->stdout_file == NULL) {
            execvp(*cmds->tokens, cmds->tokens);
            return;
        }
        else {
            //create a file and set correct permissions
            int fd = open(cmds->stdout_file, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            //redirect stdout stream
            dup2(fd, STDOUT_FILENO);
            execvp(*cmds->tokens, cmds->tokens);
            return;
        }
    }    
     /* Creates a pipe. */
    int fd[2]; /* we need 2 fds, one for the read and another for the write end */
    if (pipe(fd) == -1) {
        perror("pipe");
        return;
    }
    /* fork a new process */
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }
    /* check if pid is the child */
    else if (pid == 0) {
        dup2(fd[1], STDOUT_FILENO); //dup2 stdout to pipe[1]
        close(fd[0]); //close pipe[0]
        //execvp the command
        execvp(*cmds->tokens, cmds->tokens);
    }
    /* check if pid is the parent */
    else {
        dup2(fd[0], STDIN_FILENO); //dup2 stdin to pipe[0]
        close(fd[1]); //close pipe[1]
        execute_pipeline(++cmds); //move on to the next command in the pipeline
    }
}
