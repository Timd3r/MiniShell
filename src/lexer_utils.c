/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:17:55 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/26 16:34:38 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*new_token(t_token_type type, char *value)
{
	t_token	*token;

	token = malloc(sizeof(t_token));
	if (!token)
	{
		perror("minishell: malloc failed for new_token");
		return (NULL);
	}
	token->type = type;
	token->c = value;
	return (token);
}

void	skip_whitespace(const char **str)
{
	while (**str == ' ' || **str == '\t')
		(*str)++;
}

int	get_word_len(const char *str)
{
	int	i;

	i = 0;
	if (str[i] == '"')
	{
		i++;
		while (str[i] != '"')
			i++;
		i++;
	}
	else if (str[i] == '\'')
	{
		i++;
		while (str[i] != '\'')
			i++;
		i++;
	}
	else
	{
		while (str[i] && str[i] != ' ' && str[i] != '\t' && str[i] != '|'
			&& str[i] != '<' && str[i] != '>')
			i++;
	}
	return (i);
}

void	free_tokens_array(t_token **tokens)
{
	int	i;

	if (!tokens)
		return ;
	i = 0;
	while (tokens[i])
	{
		free(tokens[i]->c);
		free(tokens[i]);
		i++;
	}
	free(tokens);
}
