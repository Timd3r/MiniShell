/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:55:05 by tde-raev          #+#    #+#             */
/*   Updated: 2024/10/16 17:53:43 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>
#include <stdio.h>

void	*ft_memchr(const void *ptr, size_t value, int num)
{
	const unsigned char	*p;
	unsigned char		c;

	c = (unsigned char)value;
	p = (const unsigned char *)ptr;
	while (num-- > 0)
	{
		if (*p == c)
			return ((void *)p);
		p++;
	}
	return (NULL);
}
