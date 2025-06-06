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

/*
 * @brief Processes command arguments from tokens.
 *
 * @param cmd The command structure to populate.
 * @param tokens The token array.
 * @param arg_idx Pointer to current argument index.
 * @param i Pointer to current token index.
 * @return 0 on success, -1 on failure.
 */
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

/*
 * @brief Processes quote and variable expansion for command arguments.
 */
static void	process_arg_expansion(t_simple_cmd *cmd, t_shell *shell)
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

/*
 * @brief Parses tokens into a simple command structure with shell context.
 *
 * This function takes an array of tokens and converts them into a
 * t_simple_cmd structure. It handles basic commands with arguments
 * and simple redirections, with proper quote processing and variable expansion.
 *
 * @param tokens The NULL-terminated array of tokens to parse.
 * @param shell The shell context for variable expansion.
 * @return A pointer to the parsed command, or NULL on failure.
 */
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

/*
 * @brief Legacy wrapper for parse_simple_command.
 */
t_simple_cmd	*parse_simple_command(t_token **tokens)
{
	return (parse_simple_command_shell(tokens, NULL));
}
