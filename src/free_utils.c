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

/*
 * @brief Frees a NULL-terminated array of strings (like those
 * returned by ft_split).
 *
 * Iterates through the array, freeing each string, and then frees the
 * array itself.
 *
 * @param split The NULL-terminated array of strings to free.
 */
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
