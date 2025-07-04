/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikellen <mikellen@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 12:00:00 by mikellen          #+#    #+#             */
/*   Updated: 2025/06/04 12:00:00 by mikellen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	validate_export_name(char *name, char *arg)
{
	if (!is_valid_identifier(name))
	{
		ft_putstr_fd("minishell: export: `", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd("': not a valid identifier\n", 2);
		return (0);
	}
	return (1);
}

static void	print_variable_formatted(char *env_var)
{
	char	*eq_pos;
	char	*name;

	eq_pos = ft_strchr(env_var, '=');
	if (eq_pos)
	{
		name = ft_substr(env_var, 0, eq_pos - env_var);
		if (name)
		{
			printf("declare -x %s=\"%s\"\n", name, eq_pos + 1);
			free(name);
		}
	}
	else
		printf("declare -x %s\n", env_var);
}

static void	sort_env(char **env_copy, int count)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (i < count - 1)
	{
		j = 0;
		while (j < count - i - 1)
		{
			if (ft_strcmp(env_copy[j], env_copy[j + 1]) > 0)
			{
				temp = env_copy[j];
				env_copy[j] = env_copy[j + 1];
				env_copy[j + 1] = temp;
			}
			j++;
		}
		i++;
	}
}

void	print_exported_vars(t_shell *shell)
{
	int		i;
	int		count;
	char	**env_copy;

	if (!shell || !shell->env)
		return ;
	count = 0;
	while (shell->env[count])
		count++;
	env_copy = malloc(sizeof(char *) * (count + 1));
	if (!env_copy)
		return ;
	i = -1;
	while (++i < count)
		env_copy[i] = shell->env[i];
	env_copy[count] = NULL;
	sort_env(env_copy, count);
	i = -1;
	while (++i < count)
		print_variable_formatted(env_copy[i]);
	free(env_copy);
}
