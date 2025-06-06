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

/*
 * @brief Counts the number of WORD tokens in the token array.
 *
 * This helper function counts how many arguments (WORD tokens) are present
 * in the token array, which is needed to allocate the args array.
 *
 * @param tokens The NULL-terminated array of tokens.
 * @return The number of WORD tokens found.
 */
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

/*
 * @brief Initializes a simple command structure.
 *
 * This function allocates and initializes a t_simple_cmd structure
 * with default values.
 *
 * @param arg_count The number of arguments to allocate space for.
 * @return A pointer to the initialized command, or NULL on failure.
 */
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

/*
 * @brief Frees a simple command structure and all its allocated memory.
 *
 * This function properly frees all memory associated with a t_simple_cmd
 * structure, including the args array and redirection filenames.
 *
 * @param cmd The command structure to free.
 */
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
