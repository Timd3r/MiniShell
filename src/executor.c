/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 03:00:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/31 03:00:00 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	handle_builtin_child(t_simple_cmd *cmd, t_shell *shell)
{
	int	status;

	status = 0;
	if (handle_redirections(cmd) != 0)
		exit(1);
	status = execute_builtin_shell(cmd, shell);
	if (status == -42)
	{
		clear_history();
		shutdown_seq();
		exit(shell->last_exit_status);
	}
	exit(status);
}

static int	handle_builtin_parent(pid_t pid)
{
	int	status;

	status = 0;
	if (pid < 0)
	{
		perror("minishell: fork failed");
		return (1);
	}
	setup_exec_wait_signals();
	waitpid(pid, &status, 0);
	setup_signals();
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
		return (128 + WTERMSIG(status));
	}
	return (1);
}

static int	execute_builtin_with_redirect(t_simple_cmd *cmd, t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
		handle_builtin_child(cmd, shell);
	return (handle_builtin_parent(pid));
}

int	execute_simple_command_shell(t_simple_cmd *cmd, t_shell *shell)
{
	char	*executable_path;
	int		status;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	if (is_builtin(cmd->args[0]))
	{
		if (cmd->input_file || cmd->output_file)
			return (execute_builtin_with_redirect(cmd, shell));
		return (execute_builtin_shell(cmd, shell));
	}
	executable_path = find_executable_path(cmd->args[0]);
	if (!executable_path)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd->args[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		return (127);
	}
	status = execute_external_shell(cmd, executable_path, shell);
	return (status);
}

int	execute_simple_command(t_simple_cmd *cmd)
{
	return (execute_simple_command_shell(cmd, NULL));
}
