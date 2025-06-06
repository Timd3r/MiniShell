/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:17:55 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/26 16:34:38 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	print_token_type(t_token_type type)
{
	if (type == WORD)
		printf("WORD");
	else if (type == PIPE)
		printf("PIPE");
	else if (type == IN)
		printf("IN");
	else if (type == OUT)
		printf("OUT");
	else if (type == APPEND)
		printf("APPEND");
	else if (type == HEREDOC)
		printf("HEREDOC");
	else
		printf("UNKNOWN");
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
void	print_tokens(t_token **tokens)
{
	int	i;

	if (!tokens)
	{
		printf("Tokens array is NULL.\n");
		return ;
	}
	i = 0;
	printf("--- Tokens ---\n");
	while (tokens[i] != NULL)
	{
		printf("Token %d: Type=", i);
		print_token_type(tokens[i]->type);
		printf(", Value=\"%s\"\n", tokens[i]->c);
		i++;
	}
	printf("--------------\n");
}
