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

#include "minishell.h" // Includes your minishell.h

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
		printf("\033[38;2;%d;%d;%dm%c\033[0m",
			intensity, intensity, intensity, farewell[i]);
		fflush(stdout);
		usleep(100000); // 100 milliseconds
		i++;
	}
	printf("\n");
}

/*
 * @brief Signal handler for SIGINT (Ctrl+C).
 *
 * This function handles the SIGINT signal by printing a newline,
 * clearing the readline buffer, and moving to a new line.
 *
 * @param signo The signal number (SIGINT).
 */
void	handle_C(int signo)
{
	(void)signo; // Cast to void to suppress unused parameter warning
	printf("\n");
	fflush(stdout);
	rl_replace_line("", 0); // Clear the current input line
	rl_on_new_line();       // Move to a new line
}

/*
 * @brief Executes the 'pwd' command.
 *
 * This function prints the current working directory to standard output.
 * If getcwd fails, it prints a system error message.
 */
void	do_pwd(void)
{
	char	cwd[1024]; // Buffer for current working directory

	if (getcwd(cwd, sizeof(cwd)) != NULL)
		printf("%s\n", cwd);
	else
		perror("getcwd error"); // Print system error if getcwd fails
}

