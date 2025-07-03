/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:28:55 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/21 15:29:04 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h" // Includes minishell.h for necessary definitions

void	ft_free_split(char **split)
{
	int	i;

	if (!split)
		return ;
	i = 0;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

void	exit_shell(int exit_code)
{
	exit(exit_code);
}

int	wait_for_children(int count)
{
	int	status;
	int	last_status;
	int	i;

	last_status = 0;
	i = 0;
	while (i < count)
	{
		wait(&status);
		if (i == count - 1)
			last_status = WEXITSTATUS(status);
		i++;
	}
	return (last_status);
}
