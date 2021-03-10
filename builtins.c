// #include "builtins.h"

// #include <stdbool.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>  

// int handle_builtins(char *args) {
// //checks if user didn't input a command
//         if (args[0] == NULL) {
//             continue;
//         }
//         //checks if user entered "exit"
//         if (strcmp(args[0], "exit") == 0) {
//             return 0;
//         }
//         if (strcmp(args[0], "cd") == 0 && args[1] == NULL) {
//             args[1] = getenv("HOME");
//         }
//         if (chdir(args[1]) != 0) {
//             perror("No directory");
//         }
//     return 0;
// }