/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_exec.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/30 15:45:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/30 15:45:00 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

extern char	**environ;

static char	*search_in_paths(char *cmd, char **paths)
{
	char	*full_path;
	char	*temp;
	int		i;

	i = 0;
	while (paths[i])
	{
		temp = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(temp, cmd);
		free(temp);
		if (access(full_path, X_OK) == 0)
		{
			ft_free_split(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	ft_free_split(paths);
	return (NULL);
}

char	*find_executable_path(char *cmd)
{
	char	*path_env;
	char	**paths;

	if (!cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = getenv("PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	return (search_in_paths(cmd, paths));
}

static void	child_process(t_simple_cmd *cmd, char *executable_path,
				t_shell *shell)
{
	extern char	**environ;

	setup_exec_signals();
	if (handle_redirections(cmd) != 0)
		exit(1);
	if (shell && shell->env)
	{
		if (execve(executable_path, cmd->args, shell->env) == -1)
		{
			perror("minishell: execve failed");
			exit(127);
		}
	}
	else
	{
		if (execve(executable_path, cmd->args, environ) == -1)
		{
			perror("minishell: execve failed");
			exit(127);
		}
	}
}

static int	parent_process(pid_t pid, char *executable_path)
{
	int	status;

	if (pid > 0)
	{
		setup_exec_wait_signals();
		waitpid(pid, &status, 0);
		setup_signals();
		free(executable_path);
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
	else
	{
		perror("minishell: fork failed");
		free(executable_path);
		return (1);
	}
}

int	execute_external_shell(t_simple_cmd *cmd, char *executable_path,
			t_shell *shell)
{
	pid_t	pid;

	if (!cmd || !executable_path)
	{
		if (executable_path)
			free(executable_path);
		return (1);
	}
	pid = fork();
	if (pid == 0)
		child_process(cmd, executable_path, shell);
	return (parent_process(pid, executable_path));
}
