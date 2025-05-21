/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 18:00:38 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/21 14:02:17 by tde-raev         ###   ########.fr       */
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

t_token	**make_tokens()
{

}

int	main(void)
{
	char	*line;
	char	*prompt;
	char	**split_line;

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
			make_tokens(line);
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

