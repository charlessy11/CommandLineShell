#ifndef _HISTORY_H_
#define _HISTORY_H_

/**
 * Initialize history data struct
 *
 * @param limit the max number of entries maintained
 */
void hist_init(unsigned int limit);

/**
 * Free history list
 */
void hist_destroy(void);

/**
 * Add to history list
 *
 * @param cmd the command to add to the history list
 */
void hist_add(const char *cmd);

/**
 * Prints the histroy list
 */
void hist_print(void);

/**
 * Retrieves the most recent command starting with 'prefix', or NULL if no match found
 *
 * @param prefix the prefix to match with history entry
 * @return the most recent entry with matching prefix
 */
char *hist_search_prefix(char *prefix);

/**
 * Retrieves a particular command number. Return NULL if no match found 
 *
 * @param command_number the command number to search
 * @return the command associated with command number
 */
char *hist_search_cnum(int command_number);

/**
 * Retrieve the most recent command number
 *
 * @return the most recent command number
 */
int hist_last_cnum(void);

#endif
