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
