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

/*
 * @brief Executes the echo built-in command.
 *
 * @param cmd The command structure containing echo arguments.
 * @return 0 on success.
 */
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

/*
 * @brief Checks if a command is a built-in command.
 *
 * This function checks if the given command name matches any of the
 * built-in commands that the shell should handle internally.
 *
 * @param cmd The command name to check.
 * @return 1 if it's a built-in, 0 otherwise.
 */

static int	handle_exit_builtin(t_simple_cmd *cmd, t_shell *shell)
{
	int	exit_status;

	(void)cmd;
//	printf("exit\n");
	clear_history();
	shutdown_seq();
	if (shell)
		exit_status = shell->last_exit_status;
	else
		exit_status = 0;
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

/*
 * @brief Executes a built-in command with shell context.
 */
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
