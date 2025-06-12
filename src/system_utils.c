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
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * @brief Signal handler for SIGINT (Ctrl+C).
 *
 * This function handles the SIGINT signal by printing a newline,
 * clearing the readline buffer, and moving to a new line.
 *
 * @param signo The signal number (SIGINT).
 */
void	handle_sigint(int signo)
{
	g_signal_received = signo;
	printf("\n");
	fflush(stdout);
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

/*
 * @brief Resets STDIN to the terminal.
 */
void reset_stdin_to_terminal(void)
{
    int fd;

    // Close the current STDIN_FILENO to ensure it can be replaced
    close(STDIN_FILENO);

    // Open the controlling terminal
    fd = open("/dev/tty", O_RDONLY);
    if (fd == -1)
    {
        perror("minishell: failed to open /dev/tty");
        return;
    }

    // Ensure that STDIN_FILENO (0) now refers to the terminal
    if (fd != STDIN_FILENO)
    {
        if (dup2(fd, STDIN_FILENO) == -1)
        {
            perror("minishell: failed to dup2 /dev/tty to STDIN_FILENO");
        }
        close(fd); // Close the original fd, as STDIN_FILENO is now the one
    }
}

/*
 * @brief Initializes the shell structure.
 *
 * @param shell Pointer to the shell structure to initialize.
 * @param envp The environment variables passed to the shell.
 */
void initialize_shell(t_shell *shell, char **envp)
{
    int i;

    // Count the number of environment variables
    for (i = 0; envp[i]; i++)
        ;

    // Allocate memory for the environment variable array
    shell->env = malloc((i + 1) * sizeof(char *));
    if (!shell->env)
    {
        perror("minishell: malloc");
        exit(1);
    }

    // Copy the environment variables
    for (i = 0; envp[i]; i++)
        shell->env[i] = strdup(envp[i]);
    shell->env[i] = NULL;

    // Initialize other fields
    shell->last_exit_status = 0;
}

/*
 * @brief Cleans up the shell structure.
 *
 * @param shell Pointer to the shell structure to clean up.
 */
void cleanup_shell(t_shell *shell)
{
    int i;

    // Free the environment variable array
    for (i = 0; shell->env[i]; i++)
        free(shell->env[i]);
    free(shell->env);

    // Reset other fields if necessary
    shell->env = NULL;
    shell->last_exit_status = 0;
}
