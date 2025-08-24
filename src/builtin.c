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

	if (cmd->args[1] && cmd->args[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		return (1);
	}
	exit_status = 0;
	if (cmd->args[1])
	{
		if (validate_exit_arg(cmd->args[1], shell) == -42)
			return (-42);
		exit_status = calculate_exit_status(cmd->args[1], shell);
	}
	else if (shell)
		exit_status = shell->last_exit_status;
	shell->last_exit_status = exit_status;
	return (-42);
}

int	execute_builtin_shell(t_simple_cmd *cmd, t_shell *shell)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	if (!ft_strcmp(cmd->args[0], "pwd"))
	{
		do_pwd();
		return (0);
	}
	else if (!ft_strcmp(cmd->args[0], "echo"))
		return (builtin_echo(cmd));
	else if (!ft_strcmp(cmd->args[0], "cd"))
		return (builtin_cd_shell(cmd, shell));
	else if (!ft_strcmp(cmd->args[0], "env"))
		return (builtin_env(cmd, shell));
	else if (!ft_strcmp(cmd->args[0], "export"))
		return (builtin_export(cmd, shell));
	else if (!ft_strcmp(cmd->args[0], "unset"))
		return (builtin_unset(cmd, shell));
	else if (!ft_strcmp(cmd->args[0], "exit"))
		return (handle_exit_builtin(cmd, shell));
	return (1);
}
