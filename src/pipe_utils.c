/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikellen <mikellen@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 02:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/05/31 02:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;

void	close_all_pipes(int **pipes, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

void	free_pipes(int **pipes, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		free(pipes[i]);
		i++;
	}
	free(pipes);
}

void	free_pipeline(t_simple_cmd **cmds)
{
	int	i;

	if (!cmds)
		return ;
	i = 0;
	while (cmds[i])
	{
		free_simple_cmd(cmds[i]);
		i++;
	}
	free(cmds);
}

void	execute_piped_command(t_simple_cmd *cmd, int **pipes, int idx,
			int total)
{
	t_pipe_cmd_params	params;

	params.cmd = cmd;
	params.pipes = pipes;
	params.idx = idx;
	params.total = total;
	params.shell = NULL;
	execute_piped_command_shell(&params);
}

t_simple_cmd	*parse_command_segment(t_token **tokens, int start, int end)
{
	t_token			**segment;
	t_simple_cmd	*cmd;
	int				i;
	int				j;

	if (end <= start)
		return (NULL);
	segment = malloc(sizeof(t_token *) * (end - start + 1));
	if (!segment)
		return (NULL);
	i = start;
	j = 0;
	while (i < end && tokens[i])
	{
		segment[j] = tokens[i];
		i++;
		j++;
	}
	segment[j] = NULL;
	cmd = parse_simple_command(segment);
	free(segment);
	return (cmd);
}
