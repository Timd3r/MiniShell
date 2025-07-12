/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 09:23:05 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/21 13:08:15 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
