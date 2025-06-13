/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 16:35:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/27 16:35:00 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	builtin_echo(t_simple_cmd *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (cmd->args[i] && ft_strcmp(cmd->args[i], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (cmd->args[i])
	{
		printf("%s", cmd->args[i]);
		if (cmd->args[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}

static int	handle_exit_builtin(t_simple_cmd *cmd, t_shell *shell)
{
	int	exit_status;

	printf("exit\n");
	clear_history();
	shutdown_seq();
	exit_status = 0;
	if (cmd->args[1])
	{
		exit_status = ft_atoi(cmd->args[1]);
		if (exit_status < 0)
			exit_status = 256 + (exit_status % 256);
		else
			exit_status = exit_status % 256;
	}
	else if (shell)
		exit_status = shell->last_exit_status;
	exit(exit_status);
}

static int	handle_basic_builtins(t_simple_cmd *cmd)
{
	if (!ft_strcmp(cmd->args[0], "pwd"))
	{
		do_pwd();
		return (0);
	}
	else if (!ft_strcmp(cmd->args[0], "echo"))
		return (builtin_echo(cmd));
	else if (!ft_strcmp(cmd->args[0], "cd"))
		return (builtin_cd(cmd));
	return (-1);
}

int	execute_builtin_shell(t_simple_cmd *cmd, t_shell *shell)
{
	int	result;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	result = handle_basic_builtins(cmd);
	if (result != -1)
		return (result);
	if (!ft_strcmp(cmd->args[0], "env"))
		return (builtin_env(cmd, shell));
	else if (!ft_strcmp(cmd->args[0], "export"))
		return (builtin_export(cmd, shell));
	else if (!ft_strcmp(cmd->args[0], "unset"))
		return (builtin_unset(cmd, shell));
	else if (!ft_strcmp(cmd->args[0], "exit"))
		handle_exit_builtin(cmd, shell);
	return (1);
}
