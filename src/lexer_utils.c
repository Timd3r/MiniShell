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

#include "minishell.h" // Includes your minishell.h for t_token and types

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
	token->c = value; // Assign the duplicated string
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
	}
	else if (str[i] == '\'')
	{
		i++;
		while (str[i] != '\'')
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
		free(tokens[i]->c); // Free the string value
		free(tokens[i]);    // Free the token structure
		i++;
	}
	free(tokens); // Free the array of pointers itself
}

/*
 * @brief Prints the type and value of each token in a NULL-terminated array.
 *
 * This is a utility function for debugging the output of the lexer.
 * It iterates through the array of t_token pointers and prints information
 * about each token found until the NULL terminator.
 *
 * @param tokens The NULL-terminated array of t_token pointers.
 */
void print_tokens(t_token **tokens)
{
    int i;

    if (!tokens)
    {
        printf("Tokens array is NULL.\n");
        return ;
    }

    i = 0;
    printf("--- Tokens ---\n");
    while (tokens[i] != NULL) // Iterate until the NULL terminator [4, 7]
    {
        printf("Token %d: Type=", i);
        // Print the token type as a readable string based on the enum [1, 2]
        switch (tokens[i]->type)
        {
            case WORD:
                printf("WORD");
                break;
            case PIPE:
                printf("PIPE");
                break;
            case IN:
                printf("IN");
                break;
            case OUT:
                printf("OUT");
                break;
            case APPEND:
                printf("APPEND");
                break;
            case HEREDOC:
                printf("HEREDOC");
                break;
            // Add cases for any other token types you define
            default:
                printf("UNKNOWN");
                break;
        }
        // Print the token's string value [1, 2]
        printf(", Value=\"%s\"\n", tokens[i]->c);
        i++;
    }
    printf("--------------\n");
}
