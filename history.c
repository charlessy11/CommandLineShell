/**
 * @file
 *
 * Handles history.
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"
#include "ui.h"

/** 
* history entry 
*/
struct hist_entry
{
    int cmd_num;
    char command[100];
};

/**
* list of history entries 
*/
struct hist_entry *hist_list;

static int cmd_count = 0;
static int max_sz = 0;
static int counter = 0;

void hist_init(unsigned int limit)
{
    hist_list = malloc(limit * sizeof(struct hist_entry));
    max_sz = limit;
}

void hist_destroy(void)
{
    free(hist_list);
}

void hist_add(const char *cmd)
{
    if (cmd_count >= max_sz) {
        cmd_count = 0;
        counter++;
    }
    strcpy(hist_list[cmd_count].command, cmd);
    hist_list[cmd_count].cmd_num = get_count();
    cmd_count++;
}

void hist_print(void)
{
    int i = cmd_count;
    while (i != max_sz) {
        if ((max_sz * counter) > 0) {
            printf("%d  %s\n", hist_list[i].cmd_num + 1, hist_list[i].command);
        }
        ++i;
    }
    int j = 0;
    while (j != cmd_count) {
        printf("%d  %s\n", hist_list[j].cmd_num + 1, hist_list[j].command);
        ++j;
    }
    fflush(stdout);
}

char *hist_search_prefix(char *prefix)
{
    int len = strlen(prefix);
    int i = cmd_count - 1;
    while (i != 0) {
       if (strncmp(prefix, hist_list[i].command, len) == 0) {
            return hist_list[i].command;
        } 
        --i;
    }
    int j = max_sz - 1;
    while (j != cmd_count) {
        if (counter != 0) {
            if (strncmp(prefix, hist_list[j].command, len) == 0) {
                return hist_list[j].command;
            }
        }
        --j;
    }

    return NULL;
}

char *hist_search_cnum(int command_number)
{
    int i = 0;
    while (i != max_sz) {
        if (command_number - 1 == hist_list[i].cmd_num) {
            return hist_list[i].command;
        }
        ++i;
    }
    if (command_number - ((counter * max_sz) - 1) < 0) {
        return NULL;
    }
    if (command_number > max_sz) {
        return (hist_list[command_number - ((counter * max_sz) - 1) - 1].command);
    } else {
        return (hist_list[command_number - 1].command);
    }

    return NULL;
}

int hist_last_cnum(void)
{
    return cmd_count;
}