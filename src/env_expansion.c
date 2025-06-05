/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_expansion.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell <minishell@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/31 02:00:00 by minishell         #+#    #+#             */
/*   Updated: 2025/05/31 02:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 * @brief Expands environment variables in a string.
 */
char	*expand_variables(char *str, t_shell *shell)
{
	char	*result;
	char	*temp;
	int		i;

	if (!str || !ft_strchr(str, '$'))
		return (ft_strdup(str));
	result = ft_calloc(1, 1);
	i = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1])
		{
			temp = handle_variable_expansion(str, &i, shell);
			result = ft_strjoin_free(result, temp);
			free(temp);
		}
		else
		{
			temp = ft_substr(str, i, 1);
			result = ft_strjoin_free(result, temp);
			free(temp);
			i++;
		}
	}
	return (result);
}

/*
 * @brief Handles the expansion of a single variable.
 */
char	*handle_variable_expansion(char *str, int *i, t_shell *shell)
{
	char	*var_name;
	char	*value;
	int		start;
	int		len;

	(*i)++;
	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_itoa(shell->last_exit_status));
	}
	start = *i;
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;
	len = *i - start;
	if (len == 0)
		return (ft_strdup("$"));
	var_name = ft_substr(str, start, len);
	value = getenv(var_name);
	free(var_name);
	if (value)
		return (ft_strdup(value));
	return (ft_strdup(""));
}

/*
 * @brief Expands variables in all tokens using shell struct.
 */
void	expand_tokens_shell(t_token **tokens, t_shell *shell)
{
	int		i;
	char	*expanded;

	i = 0;
	while (tokens[i])
	{
		if (tokens[i]->type == WORD)
		{
			expanded = expand_variables(tokens[i]->c, shell);
			free(tokens[i]->c);
			tokens[i]->c = expanded;
		}
		i++;
	}
}

/*
 * @brief Legacy wrapper for expand_tokens (deprecated).
 */
void	expand_tokens(t_token **tokens)
{
	(void)tokens;
}

/*
 * @brief Legacy function for updating exit status (deprecated).
 */
void	update_exit_status(int status)
{
	(void)status;
}
