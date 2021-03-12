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

void setup_cmd(char **args, int count, struct command_line *cmds) {
    int index = 0;
    cmds[index].tokens = &args[0];
    cmds[index].stdout_pipe = true;
    cmds[index].stdout_file = NULL;
    index++;

    for(int i = 0; i < count; i++) {
        if(strcmp(args[i], "|") == 0){
            args[i] = 0;
            cmds[index].tokens = &args[i + 1];
            cmds[index].stdout_pipe = true;
            cmds[index].stdout_file = NULL;
            index++;
        } else if(strcmp(args[i], ">") == 0){
            args[i] = 0;
            cmds[index - 1].stdout_file = args[i + 1];
        }

    }

    cmds[index - 1].stdout_pipe = false;
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
