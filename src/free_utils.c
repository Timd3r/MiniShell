/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 01:00:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/31 01:00:00 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h" // Includes minishell.h for necessary definitions

void	ft_free_split(char **split)
{
	int	i;

	if (!split)
		return ;
	i = 0;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

void	exit_shell(int exit_code)
{
	exit(exit_code);
}

void	cleanup_current_command(t_token **tokens, t_simple_cmd *cmd)
{
	if (tokens)
		free_tokens_array(tokens);
	if (cmd)
		free_simple_cmd(cmd);
}

void	safe_exit_shell(int exit_code, t_token **tokens, t_simple_cmd *cmd)
{
	printf("exit\n");
	cleanup_current_command(tokens, cmd);
	clear_history();
	shutdown_seq();
	exit(exit_code);
}
