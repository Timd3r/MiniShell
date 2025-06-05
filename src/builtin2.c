/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikellen <mikellen@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 03:00:00 by mikellen          #+#    #+#             */
/*   Updated: 2025/05/31 03:00:00 by mikellen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * @brief Executes the cd built-in command.
 *
 * @param cmd The command structure containing cd arguments.
 * @return 0 on success, 1 on failure.
 */
int	builtin_cd(t_simple_cmd *cmd)
{
	char	*path;
	char	*home;

	if (!cmd->args[1])
	{
		home = getenv("HOME");
		if (!home)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", 2);
			return (1);
		}
		path = home;
	}
	else
		path = cmd->args[1];
	if (chdir(path) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		perror(path);
		return (1);
	}
	return (0);
}

/*
 * @brief Executes the env built-in command.
 *
 * @param cmd The command structure (unused for env).
 * @param shell The shell context containing environment.
 * @return 0 on success.
 */
int	builtin_env(t_simple_cmd *cmd, t_shell *shell)
{
	int	i;

	(void)cmd;
	if (!shell || !shell->env)
		return (1);
	i = 0;
	while (shell->env[i])
	{
		printf("%s\n", shell->env[i]);
		i++;
	}
	return (0);
}

/*
 * @brief Helper function to process a single export argument.
 *
 * @param arg The argument string containing name=value.
 * @return 0 on success, 1 on failure.
 */
static int	process_export_arg(char *arg)
{
	char	*eq_pos;
	char	*name;

	eq_pos = ft_strchr(arg, '=');
	if (!eq_pos)
		return (0);
	name = ft_substr(arg, 0, eq_pos - arg);
	if (!name)
		return (1);
	if (setenv(name, eq_pos + 1, 1) == -1)
	{
		free(name);
		return (1);
	}
	free(name);
	return (0);
}

/*
 * @brief Executes the export built-in command.
 *
 * @param cmd The command structure containing export arguments.
 * @param shell The shell context.
 * @return 0 on success.
 */
int	builtin_export(t_simple_cmd *cmd, t_shell *shell)
{
	int	i;

	if (!cmd->args[1])
	{
		print_exported_vars(shell);
		return (0);
	}
	i = 1;
	while (cmd->args[i])
	{
		if (process_export_arg(cmd->args[i]) == 1)
			return (1);
		i++;
	}
	return (0);
}

/*
 * @brief Executes the unset built-in command.
 *
 * @param cmd The command structure containing unset arguments.
 * @param shell The shell context.
 * @return 0 on success.
 */
int	builtin_unset(t_simple_cmd *cmd, t_shell *shell)
{
	int	i;

	(void)shell;
	if (!cmd->args[1])
		return (0);
	i = 1;
	while (cmd->args[i])
	{
		unsetenv(cmd->args[i]);
		i++;
	}
	return (0);
}
