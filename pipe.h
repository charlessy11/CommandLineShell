/**
 * @file
 *
 * Contains shell utility functions.
 */

#ifndef _PIPE_H_
#define _PIPE_H_

void execute_pipeline(struct command_line *cmds);
void prepareCmds(char **args, int count, struct command_line *cmds);

#endif