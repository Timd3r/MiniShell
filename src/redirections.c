/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 16:30:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/27 16:30:00 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int	process_redirections(t_simple_cmd *cmd, t_token **tokens, int *i)
{
	if (tokens[*i]->type == IN || tokens[*i]->type == HEREDOC)
		return (handle_input_redir(cmd, tokens, i));
	else if (tokens[*i]->type == OUT || tokens[*i]->type == APPEND)
		return (handle_output_redir(cmd, tokens, i));
	return (0);
}

void	heredoc_child_process(int write_fd, char *delimiter)
{
	char	*line;

	signal(SIGINT, SIG_DFL);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			if (line)
				free(line);
			break ;
		}
		write(write_fd, line, ft_strlen(line));
		write(write_fd, "\n", 1);
		free(line);
	}
	close(write_fd);
	exit(0);
}

int	handle_heredoc(char *delimiter)
{
	int		pipefd[2];
	pid_t	pid;
	int		status;

	if (pipe(pipefd) == -1)
		return (-1);
	pid = fork();
	if (pid == 0)
		heredoc_child_process(pipefd[1], delimiter);
	close(pipefd[1]);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status))
	{
		close(pipefd[0]);
		return (-1);
	}
	return (pipefd[0]);
}
