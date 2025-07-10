/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_extra.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikellen <mikellen@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 04:30:00 by mikellen          #+#    #+#             */
/*   Updated: 2025/05/31 04:30:00 by mikellen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;

static int	process_export_arg(char *arg, t_shell *shell)
{
	char	*eq_pos;
	char	*name;
	char	*value;

	eq_pos = ft_strchr(arg, '=');
	if (!eq_pos)
		return (0);
	name = ft_substr(arg, 0, eq_pos - arg);
	if (!name)
		return (1);
	if (!validate_export_name(name, arg))
	{
		free(name);
		return (1);
	}
	value = eq_pos + 1;
	if (setenv(name, value, 1) == -1)
	{
		free(name);
		return (1);
	}
	shell->env = environ;
	free(name);
	return (0);
}

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
		if (process_export_arg(cmd->args[i], shell) == 1)
			return (1);
		i++;
	}
	return (0);
}

int	builtin_unset(t_simple_cmd *cmd, t_shell *shell)
{
	int	i;

	if (!cmd->args[1])
		return (0);
	i = 1;
	while (cmd->args[i])
	{
		unsetenv(cmd->args[i]);
		shell->env = environ;
		i++;
	}
	return (0);
}
