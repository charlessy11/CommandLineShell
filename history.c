#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"
#include "ui.h"

int cmd_count = 0;
int overflow_count = 0;
int max_sz = 0;

/* history entry */
struct hist_entry
{
    int cmd_num;
    char command[100];
};

/* list of history entries */
struct hist_entry *hist_list = {0};

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
        overflow_count++;
        cmd_count = 0;
    }
    strcpy(hist_list[cmd_count].command, cmd);
    hist_list[cmd_count].cmd_num = get_count();
    cmd_count++;
}

void hist_print(void)
{
    int amount = overflow_count * max_sz;
    int i;
    if (amount > 0) {
        for (i = cmd_count; i < max_sz; i++) {
            printf("%d  %s\n", hist_list[i].cmd_num + 1, hist_list[i].command);
        }
    }
    for (i = 0; i < cmd_count; i++) {
        printf("%d  %s\n", hist_list[i].cmd_num + 1, hist_list[i].command);
    }
    fflush(stdout);
}

char *hist_search_prefix(char *prefix)
{
    int len = strlen(prefix);
    int i;
    for (i = cmd_count - 1; i >= 0; i--) {
        if (strncmp(prefix, hist_list[i].command, len) == 0) {
            return hist_list[i].command;
        }
    }
    if (overflow_count > 0) {
        for (i = max_sz - 1; i >= cmd_count; i--) {
            if (strncmp(prefix, hist_list[i].command, len) == 0) {
                return hist_list[i].command;
            }
        }
    }

    return NULL;
}

char *hist_search_cnum(int command_number)
{
    if (command_number > (overflow_count * max_sz) + cmd_count) {
        return NULL;
    }
    int i;
    for (i = 0; i < max_sz; i++) {
        if (command_number - 1 == hist_list[i].cmd_num) {
            return hist_list[i].command;
        }
    }

    int amount = overflow_count * max_sz;
    if (command_number - amount - 1 < 0) {
        return NULL;
    }
    if (command_number > max_sz) {
        return (hist_list[command_number - amount - 1].command);
    } else {
        return (hist_list[command_number - 1].command);
    }

    return NULL;
}

int hist_last_cnum(void)
{
    return get_count();
}