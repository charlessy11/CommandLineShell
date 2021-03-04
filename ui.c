#include <stdio.h>
#include <readline/readline.h>
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "history.h"
#include "logger.h"
#include "ui.h"

static int readline_init(void);

static bool scripting = false;
// static char prompt_str[80] = "--[enter a command]--> ";
// static char *line = NULL;
// static size_t line_sz = 0;

void init_ui(void)
{
    LOGP("Initializing UI...\n");

    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");

    if (isatty(STDIN_FILENO)) {
        LOGP("stdin is a TTY; entering interactive mode\n");
    } else {
        LOGP("data piped in on stdin; entering script mode\n");
        scripting = true;
    }

    rl_startup_hook = readline_init;
}

char *prompt_line(void) {
    /* create memory for info to be displayed in shell prompt */
    char *home_dir = calloc(100, sizeof(char));
    char *username = calloc(100, sizeof(char));
    char *hostname = calloc(100, sizeof(char));
    char *cwd = calloc(100, sizeof(char));
    /* set starting command number to 1 */
    int command_num = 1;
    /* retrieve info */
    gethostname(hostname, 100);
    getlogin_r(username, 100);
    getcwd(cwd, 100);

    sprintf(home_dir, "/home/%s", username);
    
    int len = strlen(home_dir);
    if (strncmp(cwd, home_dir, len) == 0) {
        sprintf(cwd, "~%s", cwd+len);
    }

    char *prompt_str = calloc(80, sizeof(char));
    sprintf(prompt_str, "[%s]-[%d]-[%s@%s:%s]%c ", "VALID", command_num++, username, hostname, cwd, '$');
    //if invalid command


    // fflush(stdout);
    return prompt_str;
}

char *read_command(void)
{
    if (scripting == true) {
        //read another way
        char *line = NULL;
        size_t line_sz = 0;
        size_t read_sz = getline(&line, &line_sz, stdin);
        if (read_sz == -1) {
            perror("getline");
            return NULL;
        }
        line[read_sz - 1] = '\0';
        return line;
    } else {
        return readline(prompt_line());
    }
}

int readline_init(void)
{
    rl_bind_keyseq("\\e[A", key_up);
    rl_bind_keyseq("\\e[B", key_down);
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_variable_bind("colored-completion-prefix", "on");
    rl_attempted_completion_function = command_completion;
    return 0;
}

int key_up(int count, int key)
{
    /* Modify the command entry text: */
    rl_replace_line("User pressed 'up' key", 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // TODO: reverse history search

    return 0;
}

int key_down(int count, int key)
{
    /* Modify the command entry text: */
    rl_replace_line("User pressed 'down' key", 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // TODO: forward history search

    return 0;
}

char **command_completion(const char *text, int start, int end)
{
    /* Tell readline that if we don't find a suitable completion, it should fall
     * back on its built-in filename completion. */
    rl_attempted_completion_over = 0;

    return rl_completion_matches(text, command_generator);
}

/**
 * This function is called repeatedly by the readline library to build a list of
 * possible completions. It returns one match per function call. Once there are
 * no more completions available, it returns NULL.
 */
char *command_generator(const char *text, int state)
{
    // TODO: find potential matching completions for 'text.' If you need to
    // initialize any data structures, state will be set to '0' the first time
    // this function is called. You will likely need to maintain static/global
    // variables to track where you are in the search so that you don't start
    // over from the beginning.

    return NULL;
}
