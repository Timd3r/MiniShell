/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tde-raev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 16:43:07 by tde-raev          #+#    #+#             */
/*   Updated: 2024/10/25 15:56:28 by tde-raev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	get_len(const char *s)
{
	size_t	len;

	len = 0;
	while (s[len] != '\0')
		len++;
	return (len);
}

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	dst_len;
	size_t	src_len;
	size_t	i;
	size_t	total_len;

	i = 0;
	src_len = get_len(src);
	dst_len = get_len(dst);
	total_len = src_len + dst_len;
	if (size <= dst_len)
		return (src_len + size);
	while (src[i] != '\0' && dst_len + 1 < size)
	{
		dst[dst_len] = src[i];
		dst_len++;
		i++;
	}
	dst[dst_len] = '\0';
	return (total_len);
}
/*
#include <stdio.h>

int main(void) {
    char dst[14] = "a";
    const char *src = "lorem ipsum dolor sit amet";
    size_t result = ft_strlcat(dst, src, 15);

    printf("Resulting string: %s\n", dst);
    printf("Total length: %zu\n", result);

    return 0;
}*/
