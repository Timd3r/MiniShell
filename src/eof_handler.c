/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eof_handler.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikellen <mikellen@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 02:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/05/31 02:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_eof_shell(t_shell *shell)
{
	int	exit_status;

	write(2, "exit\n", 5);
	clear_history();
	shutdown_seq();
	if (shell)
		exit_status = shell->last_exit_status;
	else
		exit_status = 0;
	exit(exit_status);
}

void	handle_eof(void)
{
	handle_eof_shell(NULL);
}
