/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oadhesiv <oadhesiv@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/04 18:33:29 by oadhesiv          #+#    #+#             */
/*   Updated: 2021/01/06 18:00:00 by oadhesiv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	t_ulong			*dst_ulong;
	const t_ulong	*src_ulong;
	t_byte			*dst_byte;
	const t_byte	*src_byte;

	if (!dst || !src || !len)
		return (dst);
	if (dst < src)
		return (ft_memcpy(dst, src, len));
	dst_ulong = (t_ulong *)((t_byte *)dst + len);
	src_ulong = (const t_ulong *)((t_byte *)src + len);
	while (len > sizeof(long))
	{
		*--dst_ulong = *--src_ulong;
		len -= sizeof(long);
	}
	dst_byte = (t_byte *)dst_ulong;
	src_byte = (const t_byte *)src_ulong;
	while (len--)
		*--dst_byte = *--src_byte;
	return (dst);
}
