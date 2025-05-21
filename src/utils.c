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

int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

char	*read_echo(char *line)
{
	int		i;
	char	**split;
	char	*result;

	split = ft_split(line, ' ');
    if (!split || !split[0] || strcmp(split[0], "echo"))
    {
        free(split);
        return (ft_strdup(""));
    }

    result = ft_strdup("");
    i = 1;
    while (split[i])
    {
        result = ft_strjoin_free(result, split[i]);
        if (split[++i])
            result = ft_strjoin_free(result, " ");
    }
    result = ft_strjoin_free(result, "\n");
    free(split);
    return (result);
}

char *ft_strjoin_free(char *s1, char const *s2)
{
    char *result = ft_strjoin(s1, s2);
    free(s1);
    return (result);
}

void	shutdown_seq(void)
{
    const char *farewell = "Goodbye from Minishell...";
    int len = strlen(farewell);
    int i = 0;

    while (i < len) {
        int intensity = 255 - (i * (255 / len));
        if (intensity < 0)
			intensity = 0;
        printf("\033[38;2;%d;%d;%dm%c\033[0m", 
			intensity, intensity, intensity, farewell[i]);
        fflush(stdout);
        usleep(100000);
		i++;
    }

    printf("\n");
	}

void	handle_C(int signo)
{
	(void)signo;
	printf("\n");
	fflush(stdout);
	rl_replace_line("", 0);
	rl_on_new_line();
}

void	do_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
		printf("%s\n", cwd);
	else
		perror("getcwd error");
}
