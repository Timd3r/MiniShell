/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_exec_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 02:00:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/31 02:00:00 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;

void	execute_builtin_in_pipe(t_pipe_cmd_params *params)
{
	int	status;

	status = execute_builtin_shell(params->cmd, params->shell);
	if (status == -42)
	{
		clear_history();
		shutdown_seq();
		exit(params->shell->last_exit_status);
	}
	exit(status);
}

void	execute_external_in_pipe(t_pipe_cmd_params *params)
{
	char	*executable_path;

	executable_path = find_executable_path(params->cmd->args[0]);
	if (!executable_path)
		handle_command_not_found(params->cmd->args[0]);
	setup_exec_signals();
	if (params->shell && params->shell->env)
		execve(executable_path, params->cmd->args, params->shell->env);
	else
		execve(executable_path, params->cmd->args, environ);
	perror("minishell: execve failed");
	free(executable_path);
	exit(127);
}

t_simple_cmd	**setup_pipe_params(t_token **tokens, t_shell *shell,
				t_pipe_segment_params *params, int *idxs)
{
	t_simple_cmd	**cmds;
	int				pipe_count;

	pipe_count = count_pipes(tokens);
	cmds = allocate_pipeline(pipe_count);
	if (!cmds)
		return (NULL);
	idxs[0] = 0;
	idxs[1] = 0;
	params->tokens = tokens;
	params->cmds = cmds;
	params->idxs = idxs;
	params->shell = shell;
	return (cmds);
}

int	process_pipe_segment_shell(t_pipe_segment_params *params, int i)
{
	if (!params->tokens[i + 1])
		i++;
	params->cmds[params->idxs[0]] = parse_command_segment_shell(params->tokens,
			params->idxs[1], i, params->shell);
	if (!params->cmds[params->idxs[0]])
	{
		free_pipeline(params->cmds);
		return (0);
	}
	params->idxs[0]++;
	params->idxs[1] = i + 1;
	return (1);
}

int	process_pipe_tokens(t_token **tokens, t_pipe_segment_params *params)
{
	int	i;

	i = 0;
	while (tokens[i])
	{
		if (tokens[i]->type == PIPE || !tokens[i + 1])
		{
			if (!process_pipe_segment_shell(params, i))
				return (0);
		}
		i++;
	}
	return (1);
}
