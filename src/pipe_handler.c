/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe_handler.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 02:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/05/31 02:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_simple_cmd	**allocate_pipeline(int pipe_count)
{
	t_simple_cmd	**cmds;

	cmds = malloc(sizeof(t_simple_cmd *) * (pipe_count + 2));
	return (cmds);
}

static int	process_pipe_segment(t_token **tokens, t_simple_cmd **cmds,
				int *idxs, int i)
{
	if (!tokens[i + 1])
		i++;
	cmds[idxs[0]] = parse_command_segment(tokens, idxs[1], i);
	if (!cmds[idxs[0]])
	{
		free_pipeline(cmds);
		return (0);
	}
	idxs[0]++;
	idxs[1] = i + 1;
	return (1);
}

t_simple_cmd	**split_by_pipes(t_token **tokens)
{
	t_simple_cmd	**cmds;
	int				pipe_count;
	int				idxs[2];
	int				i;

	pipe_count = count_pipes(tokens);
	cmds = allocate_pipeline(pipe_count);
	if (!cmds)
		return (NULL);
	idxs[0] = 0;
	idxs[1] = 0;
	i = 0;
	while (tokens[i])
	{
		if (tokens[i]->type == PIPE || !tokens[i + 1])
		{
			if (!process_pipe_segment(tokens, cmds, idxs, i))
				return (NULL);
		}
		i++;
	}
	cmds[idxs[0]] = NULL;
	return (cmds);
}

int	execute_pipeline_shell(t_simple_cmd **cmds, t_shell *shell)
{
	int	cmd_count;

	cmd_count = 0;
	while (cmds[cmd_count])
		cmd_count++;
	if (cmd_count == 1)
		return (execute_simple_command_shell(cmds[0], shell));
	return (execute_pipeline_loop(cmds, shell, cmd_count));
}
