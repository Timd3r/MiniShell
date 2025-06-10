/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:16:51 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/21 15:16:54 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * @brief Displays a shutdown sequence with a fading effect.
 *
 * This function prints "Goodbye from Minishell..." character by character
 * with a fading color effect and a small delay, simulating a shutdown.
 */
void	shutdown_seq(void)
{
	const char	*farewell;
	int			len;
	int			i;
	int			intensity;

	farewell = "Goodbye from Minishell...";
	len = strlen(farewell);
	i = 0;
	while (i < len)
	{
		intensity = 255 - (i * (255 / len));
		if (intensity < 0)
			intensity = 0;
		fprintf(stderr, "\033[38;2;%d;%d;%dm%c\033[0m",
			intensity, intensity, intensity, farewell[i]);
		fflush(stderr);
		usleep(100000);
		i++;
	}
	fprintf(stderr, "\n");
}

/*
 * @brief Executes the 'pwd' command.
 *
 * This function prints the current working directory to standard output.
 * If getcwd fails, it prints a system error message.
 */
void	do_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
		printf("%s\n", cwd);
	else
		perror("getcwd error");
}

/*
 * @brief Counts the number of pipes in the token array.
 */
int	count_pipes(t_token **tokens)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (tokens[i])
	{
		if (tokens[i]->type == PIPE)
			count++;
		i++;
	}
	return (count);
}
