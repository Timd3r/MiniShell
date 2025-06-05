# MiniShell

A simple shell implementation in C that mimics basic bash functionality.

## Features

### Built-in Commands
- `echo` with `-n` option
- `cd` with relative or absolute paths
- `pwd` (no options)
- `export` (no options)
- `unset` (no options)
- `env` (no options or arguments)
- `exit` (no options)

### Core Functionality
- Interactive prompt display
- Command history (using readline)
- Executable search and launch (PATH variable or relative/absolute paths)
- Signal handling (ctrl-C, ctrl-D, ctrl-\)
- Quote handling (single and double quotes)
- Environment variable expansion (`$VAR`, `$?`)

### Redirections
- `<` input redirection
- `>` output redirection
- `>>` append mode
- `<<` heredoc

### Pipes
- `|` pipe operator for command chaining

## Compilation

```bash
make        # Compile the project
make clean  # Remove object files
make fclean # Remove object files and executable
make re     # Recompile everything
```

## Usage

```bash
./minishell
```

## Project Structure

```
src/
├── minishell.c           # Main program loop
├── minishell.h           # Header file with all declarations
├── builtin.c             # Built-in command execution
├── builtin2.c            # Additional built-in commands
├── builtin_export_utils.c # Export command utilities
├── executor.c            # Command execution logic
├── external_exec.c       # External command execution
├── parser.c              # Command parsing
├── parser_utils.c        # Parser utility functions
├── lexer_utils.c         # Lexical analysis
├── token_handlers.c      # Token processing
├── token_utils.c         # Token utility functions
├── pipe_handler.c        # Pipeline execution
├── pipe_handler_utils.c  # Pipeline utilities
├── pipe_utils.c          # Additional pipe utilities
├── redirections.c        # I/O redirection handling
├── quote_handler.c       # Quote processing
├── quote_handler_utils.c # Quote utility functions
├── env_expansion.c       # Environment variable expansion
├── signal_handler.c      # Signal handling
├── string_utils.c        # String manipulation utilities
├── system_utils.c        # System-related utilities
├── free_utils.c          # Memory management
├── utils.c               # General utilities
└── debug_utils.c         # Debugging functions
```

## Memory Management

The project implements careful memory management with:
- Proper malloc/free pairing
- Error handling for all memory allocations
- Cleanup functions for complex data structures
- No memory leaks in normal operation

## Error Handling

- Comprehensive error checking for system calls
- Proper exit status handling
- Graceful handling of edge cases
- Consistent error message formatting

## Standards Compliance

- Written in C according to 42 School norm
- Compiled with `-Wall -Wextra -Werror`
- Uses only allowed functions from the subject
- Single global variable for signal handling only 