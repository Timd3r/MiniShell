/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 18:00:38 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/27 15:25:30 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	check_command(char	**lines)
{
	int		i;
	char	**split_lines;

	i = 0;
	while (lines[i] != NULL)
	{
		split_lines = ft_split(lines[i], ' ');
		if (!ft_strcmp(split_lines[0], "pwd") && split_lines[1] == NULL)
			do_pwd();
		i++;
	}
}

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
	char	*line;
	char	*prompt;
	char	**split_line;
	t_token	**tokens;

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
			split_line = ft_split(line, '|');
			tokens = make_tokens(line);
			print_tokens(tokens);
			check_command(split_line);
			if (!ft_strcmp(line, "exit"))
			{
				shutdown_seq();
				break ;
			}
			printf("%s", read_echo(line));
		}
		free(line);
	}
	clear_history();
	return (0);
}

