/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 14:45:42 by tde-raev          #+#    #+#             */
/*   Updated: 2024/10/25 13:41:14 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

int	str_len(char const *s)
{
	int	len;

	len = 0;
	while (s[len])
		len++;
	return (len);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*str;
	char	*start_str;
	int		size_s1;
	int		size_s2;

	size_s1 = str_len(s1);
	size_s2 = str_len(s2);
	str = (char *)malloc((size_s1 + size_s2 + 1) * (sizeof(char)));
	if (str == NULL)
		return (NULL);
	start_str = str;
	while (*s1)
	{
		*str = *s1;
		s1++;
		str++;
	}
	while (*s2)
	{
		*str = *s2;
		s2++;
		str++;
	}
	*str = '\0';
	return (start_str);
}
