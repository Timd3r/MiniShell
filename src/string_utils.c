/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/21 15:16:15 by tde-raev          #+#    #+#             */
/*   Updated: 2025/05/22 14:59:47 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h" // Includes your minishell.h

/*
 * @brief Compares two strings.
 *
 * This function compares the string s1 to the string s2.
 * It returns an integer less than, equal to, or greater than zero if s1
 * is found, respectively, to be less than, to match, or be greater than s2.
 *
 * @param s1 The first string to compare.
 * @param s2 The second string to compare.
 * @return An integer indicating the result of the comparison.
 */
int	ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

/*
 * @brief Reads and processes an "echo" command line.
 *
 * This function takes a line, splits it by spaces, and if the first part
 * is "echo", it concatenates the remaining parts into a single string
 * followed by a newline. It handles memory allocation and freeing of
 * intermediate split strings.
 *
 * @param line The input line to process.
 * @return A dynamically allocated string containing the echoed text,
 * or an empty string if the input is not an "echo" command.
 * Returns NULL on allocation failure.
 */
char	*read_echo(char *line)
{
	int		i;
	char	**split;
	char	*result;
	int		new_line;

	new_line = 0;
	split = ft_split(line, ' ');
	if (!split || !split[0] || ft_strcmp(split[0], "echo") != 0)
	{
		if (split)
			ft_free_split(split); // Now ft_free_split is available
		return (ft_strdup(""));
	}
	result = ft_strdup("");
	if (!result)
	{
		ft_free_split(split);
		return (NULL);
	}
	i = 1;
	if (ft_strcmp(split[i], "-n") == 0)
	{
		new_line = 1;
		i++;
	}
	while (split[i])
	{
		result = ft_strjoin_free(result, split[i]);
		if (!result)
		{
			ft_free_split(split);
			return (NULL);
		}
		if (split[++i])
		{
			result = ft_strjoin_free(result, " ");
			if (!result)
			{
				ft_free_split(split);
				return (NULL);
			}
		}
	}
	if (new_line == 0)
		result = ft_strjoin_free(result, "\n");
	ft_free_split(split); // Free the split array
	return (result);
}

/*
 * @brief Joins two strings and frees the first string.
 *
 * This function concatenates string s1 and string s2, then frees the memory
 * allocated for s1. It's useful for building strings iteratively.
 *
 * @param s1 The first string to join (will be freed).
 * @param s2 The second string to join (const).
 * @return A newly allocated string containing the concatenation of s1 and s2,
 * or NULL if memory allocation fails.
 */
char	*ft_strjoin_free(char *s1, char const *s2)
{
	char	*result;

	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

