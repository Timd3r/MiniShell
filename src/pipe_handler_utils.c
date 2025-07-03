/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handler_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 02:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/05/31 02:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;

static void	handle_child_process(t_pipe_cmd_params *params);

int	**create_pipes(int count)
{
	int	**pipes;
	int	i;

	pipes = malloc(sizeof(int *) * count);
	if (!pipes)
		return (NULL);
	i = 0;
	while (i < count)
	{
		pipes[i] = malloc(sizeof(int) * 2);
		if (!pipes[i] || pipe(pipes[i]) == -1)
		{
			while (--i >= 0)
				free(pipes[i]);
			free(pipes);
			return (NULL);
		}
		i++;
	}
	return (pipes);
}

void	execute_piped_command_shell(t_pipe_cmd_params *params)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
		handle_child_process(params);
}

static void	handle_child_process(t_pipe_cmd_params *params)
{
	if (params->idx > 0)
		dup2(params->pipes[params->idx - 1][0], STDIN_FILENO);
	if (params->idx < params->total - 1)
		dup2(params->pipes[params->idx][1], STDOUT_FILENO);
	close_all_pipes(params->pipes, params->total - 1);
	if (handle_redirections(params->cmd) != 0)
		exit(1);
	if (is_builtin(params->cmd->args[0]))
		execute_builtin_in_pipe(params);
	else
		execute_external_in_pipe(params);
}

int	execute_pipeline_loop(t_simple_cmd **cmds, t_shell *shell, int cmd_count)
{
	int					**pipes;
	int					i;
	int					status;
	t_pipe_cmd_params	params;

	pipes = create_pipes(cmd_count - 1);
	if (!pipes)
		return (1);
	i = 0;
	params.total = cmd_count;
	params.shell = shell;
	while (i < cmd_count)
	{
		params.cmd = cmds[i];
		params.pipes = pipes;
		params.idx = i;
		execute_piped_command_shell(&params);
		i++;
	}
	close_all_pipes(pipes, cmd_count - 1);
	status = wait_for_children(cmd_count);
	free_pipes(pipes, cmd_count - 1);
	return (status);
}
