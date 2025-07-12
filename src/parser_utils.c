/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 16:30:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/27 16:30:00 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	count_args(t_token **tokens)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (tokens[i])
	{
		if (tokens[i]->type == WORD)
			count++;
		i++;
	}
	return (count);
}

t_simple_cmd	*init_simple_cmd(int arg_count)
{
	t_simple_cmd	*cmd;

	cmd = malloc(sizeof(t_simple_cmd));
	if (!cmd)
		return (NULL);
	cmd->args = malloc(sizeof(char *) * (arg_count + 1));
	if (!cmd->args)
	{
		free(cmd);
		return (NULL);
	}
	cmd->input_file = NULL;
	cmd->output_file = NULL;
	cmd->append_mode = 0;
	cmd->is_heredoc = 0;
	return (cmd);
}

void	free_simple_cmd(t_simple_cmd *cmd)
{
	int	i;

	if (!cmd)
		return ;
	if (cmd->args)
	{
		i = 0;
		while (cmd->args[i])
		{
			free(cmd->args[i]);
			i++;
		}
		free(cmd->args);
	}
	if (cmd->input_file)
		free(cmd->input_file);
	if (cmd->output_file)
		free(cmd->output_file);
	free(cmd);
}

void	process_args_quotes(t_simple_cmd *cmd, t_shell *shell)
{
	char	*processed_value;
	int		i;

	i = 0;
	while (cmd->args[i])
	{
		processed_value = process_token_quotes(cmd->args[i], shell);
		if (processed_value)
		{
			free(cmd->args[i]);
			cmd->args[i] = processed_value;
		}
		i++;
	}
}

void	process_file_quotes(char **file, t_shell *shell)
{
	char	*processed_value;

	if (*file)
	{
		processed_value = process_token_quotes(*file, shell);
		if (processed_value)
		{
			free(*file);
			*file = processed_value;
		}
	}
}
