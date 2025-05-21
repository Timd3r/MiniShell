/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 11:04:27 by tde-raev          #+#    #+#             */
/*   Updated: 2024/10/14 12:42:15 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

void	*ft_memset(void *str, int value, size_t num)
{
	unsigned char	*ptr ;

	ptr = str;
	while (num > 0)
	{
		*ptr = (unsigned char)value;
		ptr++;
		num--;
	}
	return (str);
}
