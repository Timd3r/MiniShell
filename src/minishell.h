/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 18:06:14 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/31 01:36:39 by mikellen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef MINISHELL_H
# define MINISHELL_H

# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <dirent.h>
# include <termios.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
# include <fcntl.h>
# include <errno.h>
# include <string.h>

# include "../libft/libft.h"

extern volatile sig_atomic_t	g_signal_received;

typedef enum e_token_type
{
	WORD,
	PIPE,
	IN,
	OUT,
	APPEND,
	HEREDOC
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*c;
}	t_token;

typedef struct s_simple_cmd
{
	char	**args;
	char	*input_file;
	char	*output_file;
	int		append_mode;
	int		is_heredoc;
}	t_simple_cmd;

typedef struct s_shell
{
	char	**env;
	int		last_exit_status;
}	t_shell;

typedef struct s_pipe_cmd_params
{
	t_simple_cmd	*cmd;
	int				**pipes;
	int				idx;
	int				total;
	t_shell			*shell;
}	t_pipe_cmd_params;

int				ft_strcmp(const char *s1, const char *s2);
char			*ft_strjoin_free(char *s1, char const *s2);
void			ft_free_split(char **split);

void			shutdown_seq(void);
void			handle_sigint(int signo);
void			do_pwd(void);
void			handle_eof_shell(t_shell *shell);
void			setup_signals(void);

t_token			*new_token(t_token_type type, char *value);
void			skip_whitespace(const char **str);
int				get_word_len(const char *str);
void			free_tokens_array(t_token **tokens);

int				handle_token_type(const char **current_pos, t_token ***tokens,
					int *token_idx);
int				handle_input_ops(const char **current_pos, t_token ***tokens,
					int *token_idx);
int				handle_output_ops(const char **current_pos, t_token ***tokens,
					int *token_idx);
void			handle_quoted_word(const char **current_pos, char *word_val,
					int word_len);

t_token			**make_tokens(char *line);

t_simple_cmd	*parse_simple_command(t_token **tokens);
t_simple_cmd	*parse_simple_command_shell(t_token **tokens, t_shell *shell);
void			free_simple_cmd(t_simple_cmd *cmd);
int				count_args(t_token **tokens);
t_simple_cmd	*init_simple_cmd(int arg_count);
int				process_redirections(t_simple_cmd *cmd, t_token **tokens,
					int *i);

int				execute_simple_command(t_simple_cmd *cmd);
int				execute_simple_command_shell(t_simple_cmd *cmd, t_shell *shell);
int				is_builtin(char *cmd);
int				execute_builtin_shell(t_simple_cmd *cmd, t_shell *shell);
int				builtin_echo(t_simple_cmd *cmd);
int				builtin_cd(t_simple_cmd *cmd);
int				builtin_env(t_simple_cmd *cmd, t_shell *shell);
int				builtin_export(t_simple_cmd *cmd, t_shell *shell);
void			print_exported_vars(t_shell *shell);
int				builtin_unset(t_simple_cmd *cmd, t_shell *shell);
char			*find_executable_path(char *cmd);
int				execute_external_shell(t_simple_cmd *cmd, char *executable_path,
					t_shell *shell);

void			print_tokens(t_token **tokens);

/* Pipe handling functions */
int				count_pipes(t_token **tokens);
t_simple_cmd	**split_by_pipes(t_token **tokens);
int				execute_pipeline(t_simple_cmd **cmds);
int				execute_pipeline_shell(t_simple_cmd **cmds, t_shell *shell);
int				**create_pipes(int count);
void			execute_piped_command(t_simple_cmd *cmd, int **pipes, int idx,
					int total);
void			execute_piped_command_shell(t_pipe_cmd_params *params);
int				execute_pipeline_loop(t_simple_cmd **cmds, t_shell *shell,
					int cmd_count);
void			close_all_pipes(int **pipes, int count);
int				wait_for_children(int count);
void			free_pipes(int **pipes, int count);
void			free_pipeline(t_simple_cmd **cmds);
t_simple_cmd	*parse_command_segment(t_token **tokens, int start, int end);

/* Environment expansion functions */
char			*expand_variables(char *str, t_shell *shell);
char			*handle_variable_expansion(char *str, int *i, t_shell *shell);
void			expand_tokens(t_token **tokens);
void			expand_tokens_shell(t_token **tokens, t_shell *shell);
void			update_exit_status(int status);

/* Quote handling functions */
int				is_quote(char c);
int				get_quoted_len(const char *str, char quote);
char			*extract_quoted_word(const char **str);
char			*process_token_quotes(char *str, t_shell *shell);
char			*handle_single_quote(char *str, int *i);
char			*handle_double_quote(char *str, int *i, t_shell *shell);

/* Signal handling functions */
void			setup_signals(void);
void			handle_sigint_interactive(int sig);
void			setup_exec_signals(void);
void			handle_eof(void);
void			handle_eof_shell(t_shell *shell);

/* Executor utility functions */
void			heredoc_child_process(int write_fd, char *delimiter);
int				open_input_file(t_simple_cmd *cmd);
int				open_output_file(t_simple_cmd *cmd);
int				handle_heredoc(char *delimiter);

#endif
