/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_validator.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 12:00:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/12/19 12:00:00 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	validate_pipe_token(t_token **tokens, int i)
{
	if (i == 0 || !tokens[i + 1])
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	if (tokens[i + 1]->type == PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}

static int	validate_redir_token(t_token **tokens, int i)
{
	if (!tokens[i + 1] || tokens[i + 1]->type != WORD)
	{
		if (tokens[i + 1] && tokens[i + 1]->type == PIPE)
			ft_putstr_fd("minishell: syntax error near unexpected token `|'\n",
				2);
		else
			ft_putstr_fd("minishell: syntax error near unexpected"
				" token `newline'\n", 2);
		return (0);
	}
	return (1);
}

int	validate_token_syntax(t_token **tokens)
{
	int	i;

	i = 0;
	while (tokens[i])
	{
		if (tokens[i]->type == PIPE)
		{
			if (!validate_pipe_token(tokens, i))
				return (0);
		}
		else if (tokens[i]->type == IN || tokens[i]->type == OUT
			|| tokens[i]->type == APPEND || tokens[i]->type == HEREDOC)
		{
			if (!validate_redir_token(tokens, i))
				return (0);
		}
		i++;
	}
	return (1);
}
