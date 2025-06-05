/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:18:23 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/26 16:36:25 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * @brief Handles input redirection operators.
 *
 * @param current_pos Pointer to current position in input string.
 * @param tokens Pointer to tokens array.
 * @param token_idx Pointer to current token index.
 * @return 1 on success, 0 on malloc failure.
 */
int	handle_input_ops(const char **current_pos, t_token ***tokens,
	int *token_idx)
{
	if ((*current_pos)[1] == '<')
	{
		(*tokens)[(*token_idx)++] = new_token(HEREDOC, ft_strdup("<<"));
		(*current_pos)++;
	}
	else
		(*tokens)[(*token_idx)++] = new_token(IN, ft_strdup("<"));
	if (!(*tokens)[*token_idx - 1])
		return (0);
	return (1);
}

/*
 * @brief Handles output redirection operators.
 *
 * @param current_pos Pointer to current position in input string.
 * @param tokens Pointer to tokens array.
 * @param token_idx Pointer to current token index.
 * @return 1 on success, 0 on malloc failure.
 */
int	handle_output_ops(const char **current_pos, t_token ***tokens,
	int *token_idx)
{
	if ((*current_pos)[1] == '>')
	{
		(*tokens)[(*token_idx)++] = new_token(APPEND, ft_strdup(">>"));
		(*current_pos)++;
	}
	else
		(*tokens)[(*token_idx)++] = new_token(OUT, ft_strdup(">"));
	if (!(*tokens)[*token_idx - 1])
		return (0);
	return (1);
}

/*
 * @brief Handles quoted strings in word tokens.
 *
 * @param current_pos Pointer to current position in input string.
 * @param word_val Buffer to store the word.
 * @param word_len Length of the word.
 */
void	handle_quoted_word(const char **current_pos, char *word_val,
	int word_len)
{
	ft_strlcpy(word_val, *current_pos, word_len + 1);
}
