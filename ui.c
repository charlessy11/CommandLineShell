#include <stdio.h>
#include <readline/readline.h>
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <limits.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "util.h"


static int readline_init(void);
// static char prompt_str[80] = "--[enter a command]--> ";

static bool scripting = false;

static char *line = NULL;
static size_t line_sz = 0;

static int command_num = 1; //set starting command number to 1

int counter = 0;

static int result = 0;

int get_count()
{
    return counter;
}

void sum_count()
{
    counter++;
}

void set_result(int cmd_status)
{
    result = cmd_status;
}

void init_ui(void)
{
    LOGP("Initializing UI...\n");

    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");

    if (isatty(STDIN_FILENO)) {
        LOGP("stdin is a TTY; entering interactive mode\n");
        scripting = false;
    } else {
        LOGP("data piped in on stdin; entering script mode\n");
        scripting = true;
    }

    line = malloc(sizeof(char) * 1024);
    line_sz = 1024;

    rl_startup_hook = readline_init;
}

char *prompt_line(void)
{
    //create memory for info to be displayed in shell prompt
    char *username = calloc(100, sizeof(char));
    char *hostname = calloc(100, sizeof(char));
    char cwd[PATH_MAX];
    char *ptr = cwd;

    //retrieve info
    gethostname(hostname, 100);
    getlogin_r(username, 100);
    getcwd(cwd, 100);
    struct passwd *pws;
    pws = getpwuid(getuid());

    
    int len = strlen(pws->pw_dir);
    if (strncmp(cwd, pws->pw_dir, len) == 0) {
        LOG("Length = %d", len);
        LOG("CWD = %s", cwd);
        *(cwd+len) = '~';
        *(cwd+len+1) = '/';
        ptr = (cwd+len);
        LOG("PTR = %s", ptr);
    }

    char *prompt_str = calloc(80, sizeof(char));


    if (result == 0) {
        sprintf(prompt_str, "[%s]-[%d]-[%s@%s:%s]%c ", "VALID", command_num++, username, hostname, ptr, '$');
    }
    else {
        sprintf(prompt_str, "[%s]-[%d]-[%s@%s:%s]%c ", "INVALID", command_num++, username, hostname, ptr, '$');
    }
    //HELP!!! if invalid command

    return prompt_str;

    free(username);
    free(hostname);
    free(prompt_str);
}

char *read_command(void)
{
     if (scripting == true) {
        //read another way
        size_t read_sz = getline(&line, &line_sz, stdin);
        if (read_sz == -1) {
            perror("getline");
            free(line);
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
    char *curr_cmd = strdup(rl_line_buffer);
    /* Modify the command entry text: */
    char temp[4096];
    sprintf(temp, "%s. Search: %s", "User pressed 'up' key", curr_cmd);
    rl_replace_line(temp, 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // TODO: reverse history search

    return 0;
}

int key_down(int count, int key)
{
    char *curr_cmd = strdup(rl_line_buffer);
    /* Modify the command entry text: */
    char temp[4096];
    sprintf(temp, "%s. Search: %s", "User pressed 'down' key", curr_cmd);
    rl_replace_line(temp, 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // TODO: forward history search

    return 0;
}

/**
 * Tell readline that if we don't find a suitable completion, it should fall
 * back on its built-in filename completion. 
 */
char **command_completion(const char *text, int start, int end)
{
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
