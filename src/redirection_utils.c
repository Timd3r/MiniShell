/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 04:00:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/31 04:00:00 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * @brief Helper to open input file.
 */
int	open_input_file(t_simple_cmd *cmd)
{
	int	fd;

	if (cmd->is_heredoc)
		fd = handle_heredoc(cmd->input_file);
	else
		fd = open(cmd->input_file, O_RDONLY);
	if (fd == -1)
		perror(cmd->input_file);
	return (fd);
}

/*
 * @brief Helper to open output file.
 */
int	open_output_file(t_simple_cmd *cmd)
{
	int	flags;

	flags = O_WRONLY | O_CREAT;
	if (cmd->append_mode)
		flags |= O_APPEND;
	else
		flags |= O_TRUNC;
	return (open(cmd->output_file, flags, 0644));
}

/*
 * @brief Handles file redirections for a command.
 */
int	handle_redirections(t_simple_cmd *cmd)
{
	int	input_fd;
	int	output_fd;

	if (cmd->input_file)
	{
		input_fd = open_input_file(cmd);
		if (input_fd == -1)
			return (1);
		dup2(input_fd, STDIN_FILENO);
		close(input_fd);
	}
	if (cmd->output_file)
	{
		output_fd = open_output_file(cmd);
		if (output_fd == -1)
			return (1);
		dup2(output_fd, STDOUT_FILENO);
		close(output_fd);
	}
	return (0);
}
