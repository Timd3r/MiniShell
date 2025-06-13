/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 18:00:38 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/26 15:47:38 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal_received = 0;

t_token	**make_tokens(char *line)
{
	t_token		**tokens;
	int			token_idx;
	const char	*current_pos;
	int			ret;

	tokens = malloc(sizeof(t_token *) * (ft_strlen(line) + 1));
	if (!tokens)
		return (perror("minishell: malloc failed for tokens array"), NULL);
	token_idx = 0;
	current_pos = line;
	while (*current_pos)
	{
		skip_whitespace(&current_pos);
		if (*current_pos == '\0')
			break ;
		ret = handle_token_type(&current_pos, &tokens, &token_idx);
		if (ret == 0 || ret == -1)
			return (free_tokens_array(tokens), NULL);
	}
	tokens[token_idx] = NULL;
	return (tokens);
}

static void	execute_tokens(t_token **tokens, t_shell *shell)
{
	t_simple_cmd	**pipeline;
	t_simple_cmd	*cmd;
	int				status;

	if (count_pipes(tokens) > 0)
	{
		pipeline = split_by_pipes(tokens);
		if (pipeline)
		{
			status = execute_pipeline_shell(pipeline, shell);
			shell->last_exit_status = status;
			free_pipeline(pipeline);
		}
	}
	else
	{
		cmd = parse_simple_command_shell(tokens, shell);
		if (cmd)
		{
			status = execute_simple_command_shell(cmd, shell);
			shell->last_exit_status = status;
			free_simple_cmd(cmd);
		}
	}
}

static void	process_line(char *line, t_shell *shell)
{
	t_token	**tokens;

	tokens = make_tokens(line);
	if (tokens)
	{
		add_history(line);
		execute_tokens(tokens, shell);
		free_tokens_array(tokens);
	}
}

static void	shell_loop(t_shell *shell, char *prompt)
{
	char	*line;

	while (1)
	{
		g_signal_received = 0;
		line = readline(prompt);
		if (!line)
		{
			handle_eof_shell(shell);
			break ;
		}
		if (*line)
		{
			process_line(line, shell);
		}
		free(line);
	}
}

int	main(void)
{
	char		*prompt;
	t_shell		shell;
	extern char	**environ;

	shell.env = environ;
	shell.last_exit_status = 0;
	prompt = "\033[1;36mMiniShell\033[0m\033[1;31m> \033[0m";
	setup_signals();
	shell_loop(&shell, prompt);
	clear_history();
	return (0);
}
