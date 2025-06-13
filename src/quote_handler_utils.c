/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quote_handler_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 02:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/05/31 02:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*extract_quote_content(const char **str, int len)
{
	char	*word;
	int		i;
	int		j;

	word = malloc(len);
	if (!word)
		return (NULL);
	(*str)++;
	i = 0;
	j = 0;
	while (i < len - 2)
		word[j++] = (*str)[i++];
	word[j] = '\0';
	*str += i + 1;
	return (word);
}

char	*extract_quoted_word(const char **str)
{
	char	quote;
	int		len;

	quote = **str;
	len = get_quoted_len(*str, quote);
	if (len == -1)
	{
		ft_putstr_fd("minishell: unclosed quote\n", 2);
		return (NULL);
	}
	return (extract_quote_content(str, len));
}

char	*handle_single_quote(char *str, int *i)
{
	int		start;
	int		len;

	(*i)++;
	start = *i;
	while (str[*i] && str[*i] != '\'')
		(*i)++;
	len = *i - start;
	if (str[*i] == '\'')
		(*i)++;
	return (ft_substr(str, start, len));
}

char	*handle_double_quote(char *str, int *i, t_shell *shell)
{
	char	*content;
	char	*expanded;
	int		start;
	int		len;

	(*i)++;
	start = *i;
	while (str[*i] && str[*i] != '"')
		(*i)++;
	len = *i - start;
	if (str[*i] == '"')
		(*i)++;
	content = ft_substr(str, start, len);
	expanded = expand_variables(content, shell);
	free(content);
	return (expanded);
}
