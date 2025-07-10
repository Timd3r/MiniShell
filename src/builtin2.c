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

int	builtin_cd(t_simple_cmd *cmd)
{
	char	*path;
	int		should_free;

	path = get_target_path(cmd, &should_free);
	if (!path)
		return (1);
	return (change_directory(path, should_free));
}

int	builtin_cd_shell(t_simple_cmd *cmd, t_shell *shell)
{
	char	*path;
	int		should_free;
	int		result;

	path = get_target_path(cmd, &should_free);
	if (!path)
		return (1);
	result = change_directory(path, should_free);
	if (result == 0)
		update_shell_env(shell);
	return (result);
}

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
