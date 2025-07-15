/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 18:00:38 by tde-raev          #+#    #+#             */
/*   Updated: 2025/07/15 16:13:23 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal_received = 0;

static void	execute_pipes(t_token **tokens, t_shell *shell)
{
	t_simple_cmd	**pipeline;
	int				status;

	pipeline = split_by_pipes_shell(tokens, shell);
	if (pipeline)
	{
		status = execute_pipeline_shell(pipeline, shell);
		shell->last_exit_status = status;
		free_pipeline(pipeline);
	}
	else
		shell->last_exit_status = 1;
}

static void	execute_single_cmd(t_token **tokens, t_shell *shell)
{
	t_simple_cmd	*cmd;
	int				status;

	cmd = parse_simple_command_shell(tokens, shell);
	if (cmd)
	{
		status = execute_simple_command_shell(cmd, shell);
		if (status == -42)
		{
			safe_exit_shell(shell->last_exit_status, tokens, cmd);
		}
		shell->last_exit_status = status;
		free_simple_cmd(cmd);
	}
	else
		shell->last_exit_status = 1;
}

void	execute_tokens(t_token **tokens, t_shell *shell)
{
	if (count_pipes(tokens) > 0)
		execute_pipes(tokens, shell);
	else
		execute_single_cmd(tokens, shell);
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
		handle_readline_input(shell, line);
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
	increment_shlvl();
	shell_loop(&shell, prompt);
	clear_history();
	return (0);
}
