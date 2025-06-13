/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_handler.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mikellen <mikellen@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 02:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/05/31 02:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_quote(char c)
{
	return (c == '\'' || c == '"');
}

int	get_quoted_len(const char *str, char quote)
{
	int	i;

	i = 1;
	while (str[i] && str[i] != quote)
		i++;
	if (str[i] == quote)
		return (i + 1);
	return (-1);
}

static char	*process_unquoted_char(char *str, int *i, t_shell *shell)
{
	char	*temp;

	temp = ft_substr(str, *i, 1);
	if (temp[0] == '$' && str[*i + 1])
	{
		free(temp);
		temp = handle_variable_expansion(str, i, shell);
	}
	else
		(*i)++;
	return (temp);
}

static char	*process_quote_loop(char *str, t_shell *shell)
{
	char	*result;
	char	*temp;
	int		i;

	result = ft_calloc(1, 1);
	i = 0;
	while (str[i])
	{
		if (str[i] == '\'')
			temp = handle_single_quote(str, &i);
		else if (str[i] == '"')
			temp = handle_double_quote(str, &i, shell);
		else
			temp = process_unquoted_char(str, &i, shell);
		result = ft_strjoin_free(result, temp);
		free(temp);
	}
	return (result);
}

char	*process_token_quotes(char *str, t_shell *shell)
{
	char	*expanded;

	expanded = expand_variables(str, shell);
	if (!expanded)
		return (NULL);

	free(str);
	return (expanded);
}
