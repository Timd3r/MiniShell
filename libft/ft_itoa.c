/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 11:59:18 by tde-raev          #+#    #+#             */
/*   Updated: 2024/10/28 13:17:47 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

char	*special_cases(int n)
{
	char	*str;

	if (n == -2147483648)
	{
		str = ft_strdup("-2147483648");
		if (str == NULL)
			return (NULL);
		return (str);
	}
	if (n == 0)
	{
		str = ft_strdup("0");
		if (str == NULL)
			return (NULL);
		return (str);
	}
	return (NULL);
}

static char	*to_str(int n, int len)
{
	int		is_neg;
	char	*str;

	str = special_cases(n);
	is_neg = 0;
	if (str != NULL || n == 0 || n == -2147483648)
		return (str);
	if (n < 0)
	{
		is_neg = 1;
		len++;
		n = -n;
	}
	str = (char *)malloc((len + 1) * sizeof(char));
	if (str == NULL)
		return (NULL);
	if (is_neg)
		str[0] = '-';
	str[len] = '\0';
	while (len-- > is_neg)
	{
		str[len] = (n % 10) + '0';
		n /= 10;
	}
	return (str);
}

char	*ft_itoa(int n)
{
	int		tempn;
	int		len;

	len = 0;
	tempn = n;
	if (n == 0)
		return (to_str(0, 1));
	while (tempn != 0)
	{
		tempn /= 10;
		len++;
	}
	return (to_str(n, len));
}
