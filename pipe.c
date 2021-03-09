#include <stdbool.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

struct command_line {
    char **tokens;
    bool stdout_pipe; //determine when you've reached the last command in the pipeline
    char *stdout_file; //decide whether the final result gets written to a file or the terminal
};

//func that takes in tokens and length of tokens
//set up struct (by making | = null) and divides input 
//call execute pipeline
//returns status of execute_pipeline

int set_up_cmd_line(char tokens, int len) 
{
    // int temp = 0;
    // while (temp != len) {
    //     if (tokens == '|') {
            
    //     }
    // }
    int i;
    for (i = 0; i < len; i++) {
        if ()
    }
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