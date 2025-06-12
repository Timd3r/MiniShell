/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 16:30:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/06/11 16:29:49 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>

/*
 * @brief Handles input redirection tokens.
 *
 * @param cmd The command structure to update.
 * @param tokens The token array.
 * @param i Pointer to current token index.
 * @return 1 if redirection was handled, 0 otherwise.
 */
int	handle_input_redir(t_simple_cmd *cmd, t_token **tokens, int *i)
{
	if (tokens[*i + 1] && tokens[*i + 1]->type == WORD)
	{
		cmd->input_file = ft_strdup(tokens[*i + 1]->c);
		cmd->is_heredoc = (tokens[*i]->type == HEREDOC);
		(*i)++;
		return (1);
	}
	return (0);
}

/*
 * @brief Handles output redirection tokens.
 *
 * @param cmd The command structure to update.
 * @param tokens The token array.
 * @param i Pointer to current token index.
 * @return 1 if redirection was handled, 0 otherwise.
 */
int	handle_output_redir(t_simple_cmd *cmd, t_token **tokens, int *i)
{
	if (tokens[*i + 1] && tokens[*i + 1]->type == WORD)
	{
		cmd->output_file = ft_strdup(tokens[*i + 1]->c);
		cmd->append_mode = (tokens[*i]->type == APPEND);
		(*i)++;
		return (1);
	}
	return (0);
}

/*
 * @brief Processes redirection tokens.
 *
 * @param cmd The command structure to update.
 * @param tokens The token array.
 * @param i Pointer to current token index.
 * @return 1 if redirection was processed, 0 otherwise.
 */
int	process_redirections(t_simple_cmd *cmd, t_token **tokens, int *i)
{
	if (tokens[*i]->type == IN || tokens[*i]->type == HEREDOC)
		return (handle_input_redir(cmd, tokens, i));
	else if (tokens[*i]->type == OUT || tokens[*i]->type == APPEND)
		return (handle_output_redir(cmd, tokens, i));
	return (0);
}

/*
 * @brief Handles heredoc input redirection.
 *
 * @param delimiter The string that ends the heredoc.
 * @return File descriptor for reading heredoc content, or -1 on error.
 */
int handle_heredoc(char *delimiter)
{
    int     pipefd[2];
    char    *line;

    if (pipe(pipefd) == -1)
    {
        perror("minishell: pipe");
        return (-1);
    }

    while (1)
    {
        line = readline("> ");
        if (!line || ft_strcmp(line, delimiter) == 0)
        {
            free(line);
            break;
        }
        write(pipefd[1], line, ft_strlen(line));
        write(pipefd[1], "\n", 1);
        free(line);
    }

    close(pipefd[1]); // Close the write end of the pipe
    return (pipefd[0]); // Return the read end of the pipe
}
