#include <stddef.h>
#include <stdio.h>

#include "history.h"

void hist_init(unsigned int limit)
{
    // TODO: set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
    //use a circular array with size 'limit'
}

void hist_destroy(void)
{

}

void hist_add(const char *cmd)
{

}

void hist_print(void)
{
    //use printf()
    //fflush(stdout);
}

const char *hist_search_prefix(char *prefix)
{
    // TODO: Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
    return NULL;
}

const char *hist_search_cnum(int command_number)
{
    // TODO: Retrieves a particular command number. Return NULL if no match
    // found.
    return NULL;
}

unsigned int hist_last_cnum(void)
{
    // TODO: Retrieve the most recent command number.
    return 0;
}