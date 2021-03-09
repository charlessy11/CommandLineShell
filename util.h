/**
 * @file
 *
 * Contains shell utility functions.
 */

#ifndef _UTIL_H_
#define _UTIL_H_

char *next_token(char **str_ptr, const char *delim);
char* substr(char *des, const char *src, int start, int end);

#endif
