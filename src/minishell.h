/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 17:55:01 by tde-raev          #+#    #+#             */
/*   Updated: 2025/06/05 17:57:48 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef MINISHELL_H
# define MINISHELL_H

# include <sys/types.h> // For various system data types
# include <sys/wait.h>  // For waitpid
# include <sys/stat.h>  // For stat (file status)
# include <dirent.h>    // For directory operations
# include <termios.h>   // For terminal control
# include <unistd.h>    // For POSIX API (e.g., fork, exec, read, write)
# include <stdio.h>     // For standard I/O (e.g., printf, perror)
# include <stdlib.h>    // For general utilities (e.g., malloc, free, exit)
# include <readline/readline.h> // For readline function
# include <readline/history.h>  // For history management
# include <signal.h>    // For signal handling
# include <fcntl.h>     // For file control (e.g., open)
# include <errno.h>     // For error number definitions
# include <string.h>    // For string manipulation (e.g., strcmp, strlen)

// Custom library include (assuming libft.h is in ../libft/)
# include "../libft/libft.h"

// Enum for token types
typedef enum e_token_type
{
    WORD,    // Regular words or commands
    PIPE,    // |
    IN,      // < (input redirection)
    OUT,     // > (output redirection)
    APPEND,  // >> (append output redirection)
    HEREDOC  // << (here-document)
}	t_token_type;

// Struct for a token
typedef struct s_token
{
    t_token_type	type; // The type of the token
    char			*c;   // The string value of the token
}	t_token;

typedef struct s_redir
{
    t_token_type	type;     // Type of redirection (IN, OUT, APPEND, HEREDOC) [2, 4]
    char			*file;    // The filename or delimiter string [2, 4]
} t_redir;

typedef struct s_command
{
    char				*name;  // The command name (e.g., "ls", "grep", "echo", "cd")
    char				**args; // The command arguments (NULL-terminated array: ["-l", NULL] for "ls -l")
    t_redir				**redirections; // List of redirections for this command (NULL-terminated array of t_redir *)
    int					input_pipe; // Flag: 1 if input comes from a pipe, 0 otherwise
    int					output_pipe; // Flag: 1 if output goes to a pipe, 0 otherwise
    struct s_command	*next; // Pointer to the next command in the pipeline (NULL if last)
} t_command;

// --- Prototypes for functions in string_utils.c ---
int		ft_strcmp(const char *s1, const char *s2);
char	*read_echo(char *line);
char	*ft_strjoin_free(char *s1, char const *s2);
void	ft_free_split(char **split); // Added prototype for ft_free_split

// --- Prototypes for functions in system_utils.c ---
void	shutdown_seq(void);
void	handle_C(int signo);
void	do_pwd(void);

// --- Prototypes for functions in lexer_utils.c ---
t_token	*new_token(t_token_type type, char *value);
void	skip_whitespace(const char **str);
int		get_word_len(const char *str);
void	free_tokens_array(t_token **tokens);

// --- Prototypes for functions in token_handlers.c ---
// Returns 1 if a token was handled, 0 on malloc failure, -1 if no token matched.
int		handle_token_type(const char **current_pos, t_token ***tokens,
            int *token_idx);

// --- Prototypes for functions in minishell.c ---
void	check_command(char **lines);
t_token	**make_tokens(char *line); // Main tokenization function

// --- Prototypes for functions in command_handler.c ---
t_command *new_command(void);
t_redir *new_redirection(t_token_type type, char *file);
int add_redirection(t_command *cmd, t_redir *redir);
int add_argument(t_command *cmd, char *arg);
void free_command(t_command *cmd);
void free_commands(t_command *cmd);
t_command *build_commands(t_token **tokens);
void print_commands(t_command *commands);

//cd_utils
void execute_cd(t_token **tokens);

//testing tokens
void print_tokens(t_token **tokens);

#endif

