/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 18:06:14 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/21 13:06:43 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
#define MINISHELL_H

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include "../libft/libft.h"

typedef enum e_token_type
{
	WORD,	// words
	PIPE,	// |
	IN,     // <
	OUT,    // >
	APPEND, // >>
	HEREDOC // <<
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*c;
}	t_token;

int		ft_strcmp(const char *s1, const char *s2);
char	*read_echo(char *line);
void	shutdown_seq(void);
void	handle_C(int signo);
void	do_pwd(void);
void	check_command(char  **lines);
char	*ft_strjoin_free(char *s1, char const *s2);

#endif
