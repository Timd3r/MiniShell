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

static int	is_numeric_string(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	i = 0;
	if (str[i] == '-' || str[i] == '+')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
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
		if (!is_numeric_string(cmd->args[1]))
		{
			ft_putstr_fd("minishell: exit: ", 2);
			ft_putstr_fd(cmd->args[1], 2);
			ft_putstr_fd(": numeric argument required\n", 2);
			shell->last_exit_status = 2;
			return (-42);
		}
		exit_status = ft_atoi(cmd->args[1]);
		if (exit_status < 0)
			exit_status = 256 + (exit_status % 256);
		else
			exit_status = exit_status % 256;
	}
	else if (shell)
		exit_status = shell->last_exit_status;
	shell->last_exit_status = exit_status;
	return (-42);
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
		return (handle_exit_builtin(cmd, shell));
	return (1);
}
