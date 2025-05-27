/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 18:06:14 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/26 15:48:09 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

// Standard library includes
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

// External variable for environment
extern char **environ;

// Global variable for signal handling (as required by subject)
extern volatile sig_atomic_t g_signal_received;

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

// Struct for a simple command (no pipes yet, just single commands)
typedef struct s_simple_cmd
{
	char	**args;		// NULL-terminated array of arguments
	char	*input_file;	// Input redirection file (< or <<)
	char	*output_file;	// Output redirection file (> or >>)
	int		append_mode;	// 1 if >>, 0 if >
	int		is_heredoc;	// 1 if <<, 0 if <
}	t_simple_cmd;

// --- Prototypes for functions in string_utils.c ---
int		ft_strcmp(const char *s1, const char *s2);
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
t_token	**make_tokens(char *line); // Main tokenization function

// --- Prototypes for functions in parser.c ---
t_simple_cmd	*parse_simple_command(t_token **tokens);
void		free_simple_cmd(t_simple_cmd *cmd);

// --- Prototypes for functions in executor.c ---
int		execute_simple_command(t_simple_cmd *cmd);
int		is_builtin(char *cmd);
int		execute_builtin(t_simple_cmd *cmd);

//testing tokens
void print_tokens(t_token **tokens);

#endif

