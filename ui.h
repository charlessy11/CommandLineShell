/**
 * @file
 *
 * Text-based UI functionality. These functions are primarily concerned with
 * interacting with the readline library.
 */

#ifndef _UI_H_
#define _UI_H_

/**
* Completes command
*
* @param text the text command
* @param start the start of the command
* @param end the end of the command
*/
char **command_completion(const char *text, int start, int end);

/**
* Generates command
*
* @param text the text command
* @param state the state of the command
*/
char *command_generator(const char *text, int state);

/**
 * Initialize user interface
 */
void init_ui(void);

/**
 * Handles event when up key is pressed
 *
 * @param count the count
 * @param key the key
 * @return status the status
 */

int key_up(int count, int key);
/**
 * Handles event when down key is pressed
 *
 * @param count the count
 * @param key the key
 * @return status the status
 */
int key_down(int count, int key);

/**
 * Sets the prompt line
 *
 * @return prompt string the prompt string
 */
char *prompt_line(void);

/**
 * Reads the command input by user
 *
 * @return command input
 */
char *read_command(void);

/**
 * Retrieves command count
 *
 * @return current command count
 */
int get_count();

/**
 * Adds to command count
 */
void sum_count();

/**
 * Sets result based on the status of last command
 *
 * @param cmd_status the status of last command
 */
void set_result(int cmd_status);

#endif
