/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 18:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/12/19 18:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_simple_cmd	*parse_segment_with_shell(t_token **tokens, int start,
		int end, t_shell *shell)
{
	t_token			**segment;
	t_simple_cmd	*cmd;
	int				i;
	int				j;

	if (end <= start || !tokens[start])
		return (NULL);
	segment = malloc(sizeof(t_token *) * (end - start + 1));
	if (!segment)
		return (NULL);
	i = start;
	j = 0;
	while (i < end && tokens[i])
		segment[j++] = tokens[i++];
	segment[j] = NULL;
	cmd = parse_simple_command_shell(segment, shell);
	free(segment);
	return (cmd);
}

int	handle_pipe_processing(t_simple_cmd **cmds, t_token **tokens,
		t_pipe_data *data, t_shell *shell)
{
	cmds[data->cmd_idx] = parse_segment_with_shell(tokens, data->start,
			data->i, shell);
	if (!cmds[data->cmd_idx])
		return (0);
	data->cmd_idx++;
	data->start = data->i + 1;
	return (1);
}

int	handle_end_processing(t_simple_cmd **cmds, t_token **tokens,
		t_pipe_data *data, t_shell *shell)
{
	cmds[data->cmd_idx] = parse_segment_with_shell(tokens, data->start,
			data->i + 1, shell);
	if (!cmds[data->cmd_idx])
		return (0);
	return (1);
}

t_simple_cmd	**allocate_and_init_pipeline(t_token **tokens)
{
	t_simple_cmd	**cmds;

	cmds = malloc(sizeof(t_simple_cmd *) * (count_pipes(tokens) + 2));
	return (cmds);
}
