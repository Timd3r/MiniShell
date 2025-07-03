/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 02:00:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/31 02:00:00 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_numeric_string(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	i = 0;
	if (str[i] == '-' || str[i] == '+')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	validate_exit_arg(char *arg, t_shell *shell)
{
	if (!is_numeric_string(arg))
	{
		ft_putstr_fd("minishell: exit: ", 2);
		ft_putstr_fd(arg, 2);
		ft_putstr_fd(": numeric argument required\n", 2);
		shell->last_exit_status = 2;
		return (-42);
	}
	return (0);
}

int	calculate_exit_status(char *arg, t_shell *shell)
{
	int	exit_status;

	(void)shell;
	exit_status = ft_atoi(arg);
	if (exit_status < 0)
		exit_status = 256 + (exit_status % 256);
	else
		exit_status = exit_status % 256;
	return (exit_status);
}
