/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 10:26:34 by tde-raev          #+#    #+#             */
/*   Updated: 2024/10/25 16:43:05 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

void	free_words(char **splitted, int max)
{
	int		i;

	i = 0;
	while (splitted[i] && i < max)
	{
		free(splitted[i]);
		i++;
	}
	free(splitted);
}

int	word_len(char const *str, char token)
{
	int		i;
	int		count;

	i = 0;
	count = 0;
	while (str[i] == token)
		i++;
	while (str[i] != '\0')
	{
		if (str[i] != token && (i == 0 || str[i - 1] == token))
			count++;
		i++;
	}
	return (count);
}

char	*alloc_word(char const *s, char c, int *i)
{
	int		start;
	char	*word;
	int		j;

	start = *i;
	while (s[*i] && s[*i] != c)
		(*i)++;
	word = (char *)malloc((*i - start + 1) * sizeof(char));
	if (word == NULL)
		return (NULL);
	j = 0;
	while (start < *i)
		word[j++] = s[start++];
	word[j] = '\0';
	return (word);
}

char	**split_words(char const *s, char c, char **array)
{
	int		i;
	int		count;
	char	*word;

	count = 0;
	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] != c)
		{
			word = alloc_word(s, c, &i);
			if (word == NULL)
			{
				free_words(array, count);
				return (NULL);
			}
			array[count++] = word;
		}
		else
			i++;
	}
	array[count] = NULL;
	return (array);
}

char	**ft_split(char const *s, char c)
{
	char	**array;

	if (!s)
		return (NULL);
	array = (char **)malloc((word_len(s, c) + 1) * (sizeof(char *)));
	if (array == NULL)
		return (NULL);
	return (split_words(s, c, array));
}
/*
int	main(void)
{
	char **arr = ft_split("xxxxxxxxhello!", ' ');
	free_words(arr, 1);
	return (0);
}
*/
