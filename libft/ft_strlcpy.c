/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 12:02:03 by tde-raev          #+#    #+#             */
/*   Updated: 2024/10/16 11:11:20 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

int	ft_strlcpy(char *dest, const char *src, size_t i)
{
	size_t	j;
	size_t	src_len;

	j = 0;
	src_len = 0;
	if (i > 0)
	{
		while (src[j] && j < i - 1)
		{
			dest[j] = src[j];
			j++;
		}
		dest[j] = '\0';
	}
	while (src[src_len])
	{
		src_len++;
	}
	return (src_len);
}
