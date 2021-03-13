# Project 2: Command Line Shell

See: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html

**About This Program:**

The Command Line Shell (v1.0) is a program that implements a shell. The shell is called `jsh` which is short for "jett's shell". 

**How It Works:**

The programs runs an interactive shell with a nice-looking prompt that displays helpful information and validates if the user input is valid or not. In addition to this, `jsh` supports the following: scripting, built-in commands, signal handling, history, background jobs, and pipes.

**How It's Built:**

Here is a list of classes that builds `jsh`:

`history.c`: Handles history

`pipe.c`: Hanldes pipes

`shell.c`: Where the main method is located and handles built-in commands, background jobs, and signals.

`util.c`: Contains shell utility functions, most specifically a `next_token` method that retrieves the next token from a string.

`ui.c`: Text-based UI functionality. The functions in this class are primarily concerned with interacting with the readline library.

*More information about the functionality of these methods and input parameters and return values in header files.

**To compile and run:**

```
make
./jsh
```
