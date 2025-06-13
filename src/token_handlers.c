/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_handlers.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:18:23 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/26 16:36:25 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_operator(const char **current_pos, t_token ***tokens,
	int *token_idx)
{
	if (**current_pos == '|')
	{
		(*tokens)[(*token_idx)++] = new_token(PIPE, ft_strdup("|"));
		if (!(*tokens)[*token_idx - 1])
			return (0);
	}
	else if (**current_pos == '<')
	{
		if (!handle_input_ops(current_pos, tokens, token_idx))
			return (0);
	}
	else if (**current_pos == '>')
	{
		if (!handle_output_ops(current_pos, tokens, token_idx))
			return (0);
	}
	else
		return (-1);
	(*current_pos)++;
	return (1);
}

static int	handle_word(const char **current_pos, t_token ***tokens,
	int *token_idx)
{
	int		word_len;
	char	*word_val;

	word_len = get_word_len(*current_pos);
	if (word_len == 0)
		return (-1);
	word_val = malloc(sizeof(char) * (word_len + 1));
	if (!word_val)
	{
		perror("minishell: malloc failed for word_val");
		return (0);
	}
	if (**current_pos == '\'' || **current_pos == '"')
		handle_quoted_word(current_pos, word_val, word_len);
	else
		ft_strlcpy(word_val, *current_pos, word_len + 1);
	(*tokens)[(*token_idx)++] = new_token(WORD, word_val);
	if (!(*tokens)[*token_idx - 1])
	{
		free(word_val);
		return (0);
	}
	(*current_pos) += word_len;
	free(word_val);
	return (1);
}

int	handle_token_type(const char **current_pos, t_token ***tokens,
	int *token_idx)
{
	int	ret;

	ret = handle_operator(current_pos, tokens, token_idx);
	if (ret != -1)
		return (ret);
	ret = handle_word(current_pos, tokens, token_idx);
	if (ret != -1)
		return (ret);
	return (-1);
}
