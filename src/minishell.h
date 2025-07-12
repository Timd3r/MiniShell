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

#ifndef MINISHELL_H
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

typedef struct s_pipe_info
{
	int	**pipes;
	int	idx;
	int	total;
}	t_pipe_info;

typedef struct s_pipe_data
{
	int	cmd_idx;
	int	start;
	int	i;
}	t_pipe_data;

int				ft_strcmp(const char *s1, const char *s2);
char			*ft_strjoin_free(char *s1, char const *s2);
void			ft_free_split(char **split);
void			process_line(char *line, t_shell *shell);
void			handle_readline_input(t_shell *shell, char *line);
void			execute_tokens(t_token **tokens, t_shell *shell);

void			shutdown_seq(void);
void			handle_sigint(int signo);
void			do_pwd(void);
void			handle_eof_shell(t_shell *shell);
void			setup_signals(void);

t_token			*new_token(t_token_type type, char *value);
void			skip_whitespace(const char **str);
int				get_word_len(const char *str);
void			free_tokens_array(t_token **tokens);
int				validate_token_syntax(t_token **tokens);

int				handle_token_type(const char **current_pos, t_token ***tokens,
					int *token_idx);
int				handle_input_ops(const char **current_pos, t_token ***tokens,
					int *token_idx);
int				handle_output_ops(const char **current_pos, t_token ***tokens,
					int *token_idx);
void			handle_quoted_word(const char **current_pos, char *word_val,
					int word_len);
int				skip_double_quote(const char *str, int i);
int				skip_single_quote(const char *str, int i);

t_token			**make_tokens(char *line);

t_simple_cmd	*parse_simple_command(t_token **tokens);
t_simple_cmd	*parse_simple_command_shell(t_token **tokens, t_shell *shell);
void			free_simple_cmd(t_simple_cmd *cmd);
int				count_args(t_token **tokens);
t_simple_cmd	*init_simple_cmd(int arg_count);
int				process_redirections(t_simple_cmd *cmd, t_token **tokens,
					int *i);
void			process_args_quotes(t_simple_cmd *cmd, t_shell *shell);
void			process_file_quotes(char **file, t_shell *shell);

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

/* CD utility functions */
char			*get_home_path(void);
char			*get_oldpwd_path(void);
char			*expand_tilde_path(char *arg);
char			*get_target_path(t_simple_cmd *cmd, int *should_free);
int				change_directory(char *path, int should_free);

/* Pipe handling functions */
int				count_pipes(t_token **tokens);
int				**create_pipes(int count);
void			close_all_pipes(int **pipes, int count);
void			free_pipeline(t_simple_cmd **cmds);
void			cleanup_pipes(int **pipes, int count);
t_simple_cmd	**split_by_pipes_shell(t_token **tokens, t_shell *shell);

t_simple_cmd	**split_pipeline(t_token **tokens, t_shell *shell);
void			execute_pipe_cmd(t_simple_cmd *cmd, t_pipe_info *info,
					t_shell *shell);
int				execute_pipeline_shell(t_simple_cmd **cmds, t_shell *shell);
t_simple_cmd	*parse_segment_with_shell(t_token **tokens, int start,
					int end, t_shell *shell);
int				handle_pipe_processing(t_simple_cmd **cmds, t_token **tokens,
					t_pipe_data *data, t_shell *shell);
int				handle_end_processing(t_simple_cmd **cmds, t_token **tokens,
					t_pipe_data *data, t_shell *shell);
t_simple_cmd	**allocate_and_init_pipeline(t_token **tokens);
int				wait_for_children(int count);

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
void			handle_sigint_exec(int sig);
void			setup_exec_signals(void);
void			setup_exec_wait_signals(void);
void			handle_eof(void);
void			handle_eof_shell(t_shell *shell);

/* Executor utility functions */
void			heredoc_child_process(int write_fd, char *delimiter);
int				open_input_file(t_simple_cmd *cmd);
int				open_output_file(t_simple_cmd *cmd);
int				handle_heredoc(char *delimiter);
int				handle_input_redirection(t_simple_cmd *cmd);
int				handle_output_redirection(t_simple_cmd *cmd);
int				handle_redirections(t_simple_cmd *cmd);

int				is_valid_identifier(const char *str);
int				validate_export_name(char *name, char *arg);

char			**dup_env(char **envp);
void			update_pwd_env(void);
void			handle_command_not_found(char *cmd_name);

void			cleanup_current_command(t_token **tokens, t_simple_cmd *cmd);
void			safe_exit_shell(int exit_code, t_token **tokens,
					t_simple_cmd *cmd);
int				validate_exit_arg(char *arg, t_shell *shell);
int				calculate_exit_status(char *arg, t_shell *shell);

#endif
