/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/27 16:35:00 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/27 16:35:00 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * @brief Executes the echo built-in command.
 *
 * @param cmd The command structure containing echo arguments.
 * @return 0 on success.
 */
static int	builtin_echo(t_simple_cmd *cmd)
{
	int	i;

	i = 1;
	while (cmd->args[i])
	{
		printf("%s", cmd->args[i]);
		if (cmd->args[i + 1])
			printf(" ");
		i++;
	}
	printf("\n");
	return (0);
}

/*
 * @brief Checks if a command is a built-in command.
 *
 * This function checks if the given command name matches any of the
 * built-in commands that the shell should handle internally.
 *
 * @param cmd The command name to check.
 * @return 1 if it's a built-in, 0 otherwise.
 */
int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (!ft_strcmp(cmd, "echo") || !ft_strcmp(cmd, "cd")
		|| !ft_strcmp(cmd, "pwd") || !ft_strcmp(cmd, "export")
		|| !ft_strcmp(cmd, "unset") || !ft_strcmp(cmd, "env")
		|| !ft_strcmp(cmd, "exit"))
		return (1);
	return (0);
}

/*
 * @brief Executes a built-in command.
 *
 * This function handles the execution of built-in commands like echo,
 * cd, pwd, etc.
 *
 * @param cmd The command structure containing the built-in to execute.
 * @return The exit status of the command (0 for success).
 */
int	execute_builtin(t_simple_cmd *cmd)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	if (!ft_strcmp(cmd->args[0], "pwd"))
	{
		do_pwd();
		return (0);
	}
	else if (!ft_strcmp(cmd->args[0], "echo"))
		return (builtin_echo(cmd));
	else if (!ft_strcmp(cmd->args[0], "exit"))
	{
		printf("exit\n");
		shutdown_seq();
		exit(0);
	}
	return (1);
}

/*
 * @brief Searches for executable in PATH directories.
 *
 * @param cmd The command name.
 * @param paths Array of PATH directories.
 * @return Full path to executable or NULL.
 */
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

/*
 * @brief Finds the full path of an executable using PATH environment variable.
 *
 * This function searches for an executable in the directories listed in
 * the PATH environment variable.
 *
 * @param cmd The command name to search for.
 * @return The full path to the executable, or NULL if not found.
 */
static char	*find_executable_path(char *cmd)
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

/*
 * @brief Executes an external command using fork and execve.
 *
 * @param cmd The command structure.
 * @param executable_path Path to the executable.
 * @return Exit status of the command.
 */
static int	execute_external(t_simple_cmd *cmd, char *executable_path)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == 0)
	{
		if (execve(executable_path, cmd->args, environ) == -1)
		{
			perror("minishell: execve failed");
			exit(127);
		}
	}
	else if (pid > 0)
	{
		waitpid(pid, &status, 0);
		free(executable_path);
		return (WEXITSTATUS(status));
	}
	else
	{
		perror("minishell: fork failed");
		free(executable_path);
		return (1);
	}
	return (0);
}

/*
 * @brief Executes a simple command.
 *
 * This function executes a simple command, handling both built-ins and
 * external programs. It also handles basic input/output redirections.
 *
 * @param cmd The command structure to execute.
 * @return The exit status of the command.
 */
int	execute_simple_command(t_simple_cmd *cmd)
{
	char	*executable_path;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	if (is_builtin(cmd->args[0]))
		return (execute_builtin(cmd));
	executable_path = find_executable_path(cmd->args[0]);
	if (!executable_path)
	{
		printf("minishell: %s: command not found\n", cmd->args[0]);
		return (127);
	}
	return (execute_external(cmd, executable_path));
}
