/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 15:31:13 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/27 16:34:06 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_command	parse_tokens(t_token **tokens)
{
	int			i;
	t_command	command;
	char		**args;

	i = 0;
	if (token[i]->type == WORD)
	{
		command->name = token[i]->c;
		i++;
		while (token[i]->type == WORD)
		{
			
		}
	}
}
