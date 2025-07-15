/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 16:01:42 by tde-raev          #+#    #+#             */
/*   Updated: 2025/07/15 16:24:45 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	increment_shlvl(void)
{
	char	*shlvl_value;
	int		shlvl;
	char	*new_shlvl;

	shlvl = 1;
	*shlvl_value = getenv("SHLVL");
	if (shlvl_value)
		shlvl = atoi(shlvl_value) + 1;
	else
		shlvl = 1;
	if (shlvl > 1000)
		shlvl = 1;
	new_shlvl = ft_itoa(shlvl);
	if (new_shlvl)
	{
		setenv("SHLVL", new_shlvl, 1);
		free(new_shlvl);
	}
}
