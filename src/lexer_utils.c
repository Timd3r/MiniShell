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

/*
 * @brief Creates a new t_token structure.
 *
 * This function allocates memory for a new token, sets its type and value,
 * and returns a pointer to the newly created token.
 *
 * @param type The type of the token (e.g., WORD, PIPE, IN).
 * @param value The string value associated with the token. This string
 * is expected to be dynamically allocated (e.g., by ft_strdup)
 * before being passed to this function, as this function
 * does not duplicate it.
 * @return A pointer to the new t_token, or NULL if memory allocation fails.
 */
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

/*
 * @brief Skips whitespace characters in a string.
 *
 * This function advances the given string pointer past any space (' ')
 * or tab ('\t') characters.
 *
 * @param str A pointer to a pointer to the string. The pointer will be
 * modified to point to the first non-whitespace character.
 */
void	skip_whitespace(const char **str)
{
	while (**str == ' ' || **str == '\t')
		(*str)++;
}

/*
 * @brief Determines the length of a "word" token.
 *
 * A word is defined as a sequence of characters that are not whitespace
 * and not shell operators (|, <, >).
 *
 * @param str The string from which to determine the word length.
 * @return The length of the word.
 */
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

/*
 * @brief Frees all tokens in a NULL-terminated array of t_token pointers.
 *
 * This helper function is used for error handling to clean up memory
 * if token creation fails midway. It frees the string value (token->c)
 * and then the token structure itself for each token, and finally
 * the array of pointers.
 *
 * @param tokens The array of t_token pointers to free.
 */
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
