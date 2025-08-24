/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   system_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:16:51 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/21 15:16:54 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	shutdown_seq(void)
{
	const char	*farewell;
	int			len;
	int			i;
	int			intensity;

	farewell = "Goodbye from Minishell...";
	len = strlen(farewell);
	i = 0;
	while (i < len)
	{
		intensity = 255 - (i * (255 / len));
		if (intensity < 0)
			intensity = 0;
		fprintf(stderr, "\033[38;2;%d;%d;%dm%c\033[0m",
			intensity, intensity, intensity, farewell[i]);
		fflush(stderr);
		usleep(100000);
		i++;
	}
	fprintf(stderr, "\n");
}

void	do_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
		printf("%s\n", cwd);
	else
		perror("getcwd error");
}

int	count_pipes(t_token **tokens)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (tokens[i])
	{
		if (tokens[i]->type == PIPE)
			count++;
		i++;
	}
	return (count);
}

char	**dup_env(char **envp)
{
	char	**new_env;
	int		count;
	int		i;

	count = 0;
	while (envp && envp[count])
		count++;
	new_env = malloc(sizeof(char *) * (count + 1));
	if (!new_env)
		return (NULL);
	i = 0;
	while (i < count)
	{
		new_env[i] = ft_strdup(envp[i]);
		if (!new_env[i])
		{
			while (--i >= 0)
				free(new_env[i]);
			free(new_env);
			return (NULL);
		}
		i++;
	}
	new_env[count] = NULL;
	return (new_env);
}

void	update_pwd_env(void)
{
	char	*old_pwd;
	char	cwd[4096];

	old_pwd = getenv("PWD");
	if (old_pwd)
		setenv("OLDPWD", old_pwd, 1);
	if (getcwd(cwd, sizeof(cwd)))
		setenv("PWD", cwd, 1);
}

void	update_shell_env(t_shell *shell)
{
	extern char	**environ;

	if (!shell)
		return ;
	shell->env = environ;
}
