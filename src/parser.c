/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 16:30:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/27 16:30:00 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_command_args(t_simple_cmd *cmd, t_token **tokens,
			int *arg_idx, int *i)
{
	if (tokens[*i]->type == WORD)
	{
		cmd->args[*arg_idx] = ft_strdup(tokens[*i]->c);
		if (!cmd->args[*arg_idx])
		{
			free_simple_cmd(cmd);
			return (-1);
		}
		(*arg_idx)++;
	}
	else if (process_redirections(cmd, tokens, i))
	{
		return (0);
	}
	return (0);
}

static void	process_arg_expansion(t_simple_cmd *cmd, t_shell *shell)
{
	process_args_quotes(cmd, shell);
	process_file_quotes(&cmd->input_file, shell);
	process_file_quotes(&cmd->output_file, shell);
}

t_simple_cmd	*parse_simple_command_shell(t_token **tokens, t_shell *shell)
{
	t_simple_cmd	*cmd;
	int				arg_count;
	int				arg_idx;
	int				i;

	if (!tokens || !tokens[0])
		return (NULL);
	arg_count = count_args(tokens);
	cmd = init_simple_cmd(arg_count);
	if (!cmd)
		return (NULL);
	arg_idx = 0;
	i = 0;
	while (tokens[i])
	{
		if (process_command_args(cmd, tokens, &arg_idx, &i) == -1)
			return (NULL);
		i++;
	}
	cmd->args[arg_idx] = NULL;
	if (shell)
		process_arg_expansion(cmd, shell);
	return (cmd);
}

t_simple_cmd	*parse_simple_command(t_token **tokens)
{
	return (parse_simple_command_shell(tokens, NULL));
}
