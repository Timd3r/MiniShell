/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikellen <mikellen@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 03:00:00 by mikellen          #+#    #+#             */
/*   Updated: 2025/05/31 03:00:00 by mikellen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*get_home_path(void)
{
	char	*home;

	home = getenv("HOME");
	if (!home)
		ft_putstr_fd("minishell: cd: HOME not set\n", 2);
	return (home);
}

char	*get_oldpwd_path(void)
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

char	*expand_tilde_path(char *arg)
{
	char	*home;
	char	*expanded;

	if (arg[0] != '~')
		return (arg);
	if (arg[1] == '\0')
	{
		home = get_home_path();
		return (home);
	}
	if (arg[1] == '/')
	{
		home = getenv("HOME");
		if (!home)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n", 2);
			return (NULL);
		}
		expanded = ft_strjoin(home, arg + 1);
		return (expanded);
	}
	return (arg);
}

char	*get_target_path(t_simple_cmd *cmd, int *should_free)
{
	char	*path;

	*should_free = 0;
	if (!cmd->args[1])
	{
		path = get_home_path();
		if (!path)
			return (NULL);
	}
	else if (ft_strcmp(cmd->args[1], "-") == 0)
	{
		path = get_oldpwd_path();
		if (!path)
			return (NULL);
	}
	else
	{
		path = expand_tilde_path(cmd->args[1]);
		if (!path)
			return (NULL);
		if (path != cmd->args[1])
			*should_free = 1;
	}
	return (path);
}

int	change_directory(char *path, int should_free)
{
	if (chdir(path) == -1)
	{
		ft_putstr_fd("minishell: cd: ", 2);
		perror(path);
		if (should_free)
			free(path);
		return (1);
	}
	if (should_free)
		free(path);
	update_pwd_env();
	return (0);
}
