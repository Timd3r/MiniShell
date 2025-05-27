/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 18:00:38 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/26 15:47:38 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * @brief Creates an array of tokens from an input line.
 *
 * This function parses the input line, identifying words, pipes, and
 * redirection operators, and converts them into an array of t_token
 * structures. The array is NULL-terminated.
 *
 * @param line The input string to be tokenized.
 * @return A NULL-terminated array of t_token pointers, or NULL if
 * memory allocation fails at any point or an unexpected character is found.
*/
t_token	**make_tokens(char *line)
{
	t_token		**tokens;
	int			token_idx;
	const char	*current_pos;
	int			ret;

	// Overallocate to simplify initial allocation; a more precise count
	// would require a first pass. Max possible tokens is strlen(line).
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
		if (ret == 0 || ret == -1) // 0 for malloc fail, -1 for unexpected char
			return (free_tokens_array(tokens), NULL);
	}
	tokens[token_idx] = NULL; // Null-terminate the array of token pointers
	return (tokens);
}

int	main(void)
{
	char			*line;
	char			*prompt;
	t_token			**tokens;
	t_simple_cmd	*cmd;

	prompt = "\033[1;36mMiniShell\033[0m\033[1;31m> \033[0m";
	signal(SIGINT, handle_C);
	while (1)
	{
		line = readline(prompt);
		if (!line)
		{
			shutdown_seq();
			break ;
		}
		if (*line)
		{
			add_history(line);
			tokens = make_tokens(line);
			if (tokens)
			{
				cmd = parse_simple_command(tokens);
				if (cmd)
				{
					execute_simple_command(cmd);
					free_simple_cmd(cmd);
				}
				free_tokens_array(tokens);
			}
		}
		free(line);
	}
	clear_history();
	return (0);
}

