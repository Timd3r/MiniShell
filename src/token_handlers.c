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

#include "minishell.h" // For t_token, t_token_type, new_token, get_word_len, etc.

/*
 * @brief Handles the creation of an operator token (PIPE, IN, OUT, APPEND, HEREDOC).
 *
 * This function checks the current character for known operators. If a match is
 * found, it creates the corresponding token, adds it to the tokens array, and
 * advances the current_pos pointer.
 *
 * @param current_pos A pointer to a pointer to the current position in the input string.
 * It will be advanced if an operator token is found.
 * @param tokens A pointer to a pointer to the array of t_token pointers. The new
 * token will be added to this array.
 * @param token_idx A pointer to the current index in the tokens array. It will be
 * incremented if a token is successfully added.
 * @return 1 if an operator token was successfully handled.
 * 0 if malloc failed during token creation.
 * -1 if the current character is not a recognized operator.
 */
static int	handle_operator(const char **current_pos, t_token ***tokens,
	int *token_idx)
{
	if (**current_pos == '|')
		(*tokens)[(*token_idx)++] = new_token(PIPE, ft_strdup("|"));
	else if (**current_pos == '<')
	{
		if ((*current_pos)[1] == '<')
		{
			(*tokens)[(*token_idx)++] = new_token(HEREDOC, ft_strdup("<<"));
			(*current_pos)++;
		}
		else
			(*tokens)[(*token_idx)++] = new_token(IN, ft_strdup("<"));
	}
	else if (**current_pos == '>')
	{
		if ((*current_pos)[1] == '>')
		{
			(*tokens)[(*token_idx)++] = new_token(APPEND, ft_strdup(">>"));
			(*current_pos)++;
		}
		else
			(*tokens)[(*token_idx)++] = new_token(OUT, ft_strdup(">"));
	}
	else
		return (-1); // Not an operator
	if (!(*tokens)[*token_idx - 1]) // Check if new_token failed
		return (0);
	(*current_pos)++; // Advance past the operator
	return (1);
}

/*
 * @brief Handles the creation of a WORD token.
 *
 * This function extracts a "word" from the current position, creates a WORD token,
 * adds it to the tokens array, and advances the current_pos pointer.
 *
 * @param current_pos A pointer to a pointer to the current position in the input string.
 * It will be advanced past the word.
 * @param tokens A pointer to a pointer to the array of t_token pointers. The new
 * token will be added to this array.
 * @param token_idx A pointer to the current index in the tokens array. It will be
 * incremented if a token is successfully added.
 * @return 1 if a word token was successfully handled.
 * 0 if malloc failed during token creation.
 * -1 if no word could be extracted (should not happen if not an operator).
 */
static int	handle_word(const char **current_pos, t_token ***tokens,
	int *token_idx)
{
	int		word_len;
	char	*word_val;

	word_len = get_word_len(*current_pos);
	if (word_len == 0)
		return (-1); // No word found
	word_val = malloc(sizeof(char) * (word_len + 1));
	if (!word_val)
	{
		perror("minishell: malloc failed for word_val");
		return (0); // Malloc failed
	}
	if (**current_pos == '\'' || **current_pos == '"')
	{
		(*current_pos)++;
		ft_strlcpy(word_val, *current_pos, word_len);
	}
	else
		ft_strlcpy(word_val, *current_pos, word_len + 1);
	(*tokens)[(*token_idx)++] = new_token(WORD, word_val);
	if (!(*tokens)[*token_idx - 1]) // Check if new_token failed
	{
		free(word_val); // Free the word_val if new_token failed
		return (0);
	}
	(*current_pos) += word_len; // Advance past the word
	return (1);
}

/*
 * @brief Determines and handles the next token type (operator or word).
 *
 * This is the main dispatch function for token handling. It first tries to
 * handle an operator. If not an operator, it tries to handle a word.
 *
 * @param current_pos A pointer to a pointer to the current position in the input string.
 * It will be advanced based on the token found.
 * @param tokens A pointer to a pointer to the array of t_token pointers.
 * @param token_idx A pointer to the current index in the tokens array.
 * @return 1 if a token was successfully handled.
 * 0 if malloc failed during token creation in a handler.
 * -1 if the character at current_pos does not match any known token type.
 */
int	handle_token_type(const char **current_pos, t_token ***tokens,
	int *token_idx)
{
	int	ret;

	ret = handle_operator(current_pos, tokens, token_idx);
	if (ret != -1) // If it was an operator or malloc failed
		return (ret);
	ret = handle_word(current_pos, tokens, token_idx);
	if (ret != -1) // If it was a word or malloc failed
		return (ret);
	return (-1); // No token type matched
}