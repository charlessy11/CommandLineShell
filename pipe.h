/**
 * @file
 *
 * Deals with pipes.
 */

#ifndef _PIPE_H_
#define _PIPE_H_

/**
* Command line struct
*/
struct command_line {
    char **tokens;
    bool stdout_pipe; //determine when you've reached the last command in the pipeline
    char *stdout_file; //decide whether the final result gets written to a file or the terminal
};

/**
* Executes pipeline
*
* @param cmds the commands
*/
void execute_pipeline(struct command_line *cmds);

/**
* Sets up commands
*
* @param args the args
* @param count the count
* @param cmds the commands
*/
void setup_cmd(char **args, int count, struct command_line *cmds);


#endif