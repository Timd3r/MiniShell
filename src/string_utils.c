/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 02:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/05/31 02:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_strjoin_free(char *s1, char const *s2)
{
	char	*joined;

	if (!s1 || !s2)
		return (NULL);
	joined = ft_strjoin(s1, s2);
	free(s1);
	return (joined);
}

void	process_line(char *line, t_shell *shell)
{
	t_token	**tokens;

	tokens = make_tokens(line);
	if (tokens)
	{
		add_history(line);
		execute_tokens(tokens, shell);
		free_tokens_array(tokens);
	}
}

void	handle_readline_input(t_shell *shell, char *line)
{
	if (*line)
		process_line(line, shell);
	free(line);
}
