/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 11:47:17 by tde-raev          #+#    #+#             */
/*   Updated: 2024/10/16 16:19:28 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *str, int c)
{
	char			*ch;
	int				i;
	unsigned char	uch;

	uch = (unsigned char)c;
	ch = NULL;
	i = 0;
	while (str[i])
	{
		if ((unsigned char)str[i] == uch)
			ch = (char *)&str[i];
		i++;
	}
	if ((unsigned char)str[i] == uch)
		ch = (char *)&str[i];
	return (ch);
}
