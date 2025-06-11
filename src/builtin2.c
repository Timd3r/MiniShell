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
 * @brief Helper function to get HOME directory path.
 *
 * @return HOME path or NULL on failure.
 */
static char	*get_home_path(void)
{
	char	*home;

	home = getenv("HOME");
	if (!home)
		ft_putstr_fd("minishell: cd: HOME not set\n", 2);
	return (home);
}

/*
 * @brief Helper function to get OLDPWD directory path.
 *
 * @return OLDPWD path or NULL on failure.
 */
static char	*get_oldpwd_path(void)
{
	char	*oldpwd;

	oldpwd = getenv("OLDPWD");
	if (!oldpwd)
	{
		ft_putstr_fd("minishell: cd: OLDPWD not set\n", 2);
		return (NULL);
	}
	printf("%s\n", oldpwd);
	return (oldpwd);
}

/*
 * @brief Executes the cd built-in command.
 *
 * @param cmd The command structure containing cd arguments.
 * @return 0 on success, 1 on failure.
 */
int	builtin_cd(t_simple_cmd *cmd)
{
	char	*path;

	if (!cmd->args[1])
	{
		path = get_home_path();
		if (!path)
			return (1);
	}
	else if (ft_strcmp(cmd->args[1], "-") == 0)
	{
		path = get_oldpwd_path();
		if (!path)
			return (1);
	}
	else
		path = cmd->args[1];
	if (chdir(path) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		perror(path);
		return (1);
	}
	update_pwd_env();
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
