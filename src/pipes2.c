/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 18:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/12/19 18:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;

t_simple_cmd	**split_pipeline(t_token **tokens, t_shell *shell)
{
	t_simple_cmd	**cmds;
	t_pipe_data		data;

	cmds = allocate_and_init_pipeline(tokens);
	if (!cmds)
		return (NULL);
	data.cmd_idx = 0;
	data.start = 0;
	data.i = -1;
	while (tokens[++data.i])
	{
		if (tokens[data.i]->type == PIPE)
		{
			if (!handle_pipe_processing(cmds, tokens, &data, shell))
				return (free_pipeline(cmds), NULL);
		}
		else if (!tokens[data.i + 1])
		{
			if (!handle_end_processing(cmds, tokens, &data, shell))
				return (free_pipeline(cmds), NULL);
			break ;
		}
	}
	cmds[data.cmd_idx + 1] = NULL;
	return (cmds);
}

static void	execute_child_process(t_simple_cmd *cmd, t_shell *shell)
{
	char	*path;

	if (handle_redirections(cmd) != 0)
		exit(1);
	if (is_builtin(cmd->args[0]))
		exit(execute_builtin_shell(cmd, shell));
	path = find_executable_path(cmd->args[0]);
	if (!path)
		handle_command_not_found(cmd->args[0]);
	setup_exec_signals();
	execve(path, cmd->args, shell->env);
	free(path);
	exit(127);
}

void	execute_pipe_cmd(t_simple_cmd *cmd, t_pipe_info *info, t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid != 0)
		return ;
	if (info->idx > 0)
		dup2(info->pipes[info->idx - 1][0], STDIN_FILENO);
	if (info->idx < info->total - 1)
		dup2(info->pipes[info->idx][1], STDOUT_FILENO);
	close_all_pipes(info->pipes, info->total - 1);
	execute_child_process(cmd, shell);
}

int	execute_pipeline_shell(t_simple_cmd **cmds, t_shell *shell)
{
	int			**pipes;
	int			cmd_count;
	int			i;
	t_pipe_info	info;

	cmd_count = 0;
	while (cmds[cmd_count])
		cmd_count++;
	if (cmd_count == 1)
		return (execute_simple_command_shell(cmds[0], shell));
	pipes = create_pipes(cmd_count - 1);
	if (!pipes)
		return (1);
	info.pipes = pipes;
	info.total = cmd_count;
	i = 0;
	while (i < cmd_count)
	{
		info.idx = i++;
		execute_pipe_cmd(cmds[info.idx], &info, shell);
	}
	cleanup_pipes(pipes, cmd_count - 1);
	return (wait_for_children(cmd_count));
}
