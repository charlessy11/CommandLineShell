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

// int pipeline_status = 0;

void setup_cmd(char **args, int count, struct command_line *cmds) {
    int index = 0;
    cmds[index].tokens = &args[0];
    cmds[index].stdout_pipe = false;
    cmds[index].stdout_file = NULL;
    index++;

    for(int i = 0; i < count; i++) {
        if(strcmp(args[i], "|") == 0){
            args[i] = 0;
            cmds[index].tokens = &args[i + 1];
            cmds[index-1].stdout_pipe = true;
            index++;
        } 
        else if(strcmp(args[i], ">") == 0){
            args[i] = 0;
            cmds[index - 1].stdout_file = args[i + 1];
        }
        else if(strcmp(args[i], "<") == 0){
            args[i] = 0;
            cmds[index - 1].stdin_file = args[i + 1];
        }
    }
}

int execute_pipeline(struct command_line *cmds)
{
    //create a file and set correct permissions
    // int file_dir = open(cmds->stdout_file, O_CREAT | O_WRONLY | O_TRUNC, 0666);
    //check for stdin_file
    // if (cmds->stdin_file != NULL) {
    //     //dup2
    //     dup2(file_dir, STDIN_FILENO);
    // }
    /* base case */
    if (cmds->stdout_pipe == false) {
        if (cmds->stdout_file == NULL) {
            execvp(*cmds->tokens, cmds->tokens);
            return -1;
        }
        else {
            // //create a file and set correct permissions
            int fd = open(cmds->stdout_file, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            // if (cmds->stdin_file != NULL) {
            //     //dup2
            //     dup2(fd, STDIN_FILENO);
            // }
            //redirect stdout stream
            dup2(fd, STDOUT_FILENO);
            execvp(*cmds->tokens, cmds->tokens);
            return -1;
        }
    }    
     /* Creates a pipe. */
    int fd[2]; /* we need 2 fds, one for the read and another for the write end */
    if (pipe(fd) == -1) {
        perror("pipe");
        return -1;
    }
    /* fork a new process */
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    /* check if pid is the child */
    else if (pid == 0) {
        //dup2 stdout to pipe[1]
        if (dup2(fd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
        } 
        close(fd[0]); //close pipe[0]
        //execvp the command
        execvp(*cmds->tokens, cmds->tokens);
    }
    /* check if pid is the parent */
    else {
        //dup2 stdin to pipe[0]
        if (dup2(fd[0], STDIN_FILENO) == -1) {
            perror("dup22");
        } 
        close(fd[1]); //close pipe[1]
        execute_pipeline(++cmds); //move on to the next command in the pipeline
    }
    return 0;
}
