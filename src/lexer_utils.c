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
	while (str[i] && str[i] != ' ' && str[i] != '\t' && str[i] != '|'
		&& str[i] != '<' && str[i] != '>')
	{
		if (str[i] == '"')
			i = skip_double_quote(str, i);
		else if (str[i] == '\'')
			i = skip_single_quote(str, i);
		else
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

t_token	**make_tokens(char *line)
{
	t_token		**tokens;
	int			token_idx;
	const char	*current_pos;
	int			ret;

	tokens = malloc(sizeof(t_token *) * (ft_strlen(line) + 1));
	if (!tokens)
		return (perror("minishell: malloc failed for tokens array"), NULL);
	token_idx = 0;
	current_pos = line;
	while (*current_pos)
	{
		skip_whitespace(&current_pos);
		if (*current_pos == '\0')
			break ;
		ret = handle_token_type(&current_pos, &tokens, &token_idx);
		if (ret == 0 || ret == -1)
			return (free_tokens_array(tokens), NULL);
	}
	tokens[token_idx] = NULL;
	return (tokens);
}
