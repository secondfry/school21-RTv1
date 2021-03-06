/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ltoa_hex_static.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oadhesiv <oadhesiv@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/25 16:21:37 by oadhesiv          #+#    #+#             */
/*   Updated: 2021/01/28 14:17:24 by oadhesiv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	byte_to_hex(t_byte b)
{
	if (b < 16)
		return ("0123456789ABCDEF"[b]);
	return ('.');
}

char	*ft_ltoa_hex_static(long n)
{
	static char	buffer[DATA_MODEL_LONG_HEX_WIDTH];
	t_ulong		tmp;
	t_byte		i;

	if (n == 0)
		return ("0");
	ft_bzero(buffer, DATA_MODEL_LONG_HEX_WIDTH);
	if (n < 0)
		tmp = -n;
	else
		tmp = n;
	i = 0;
	while (tmp != 0)
	{
		buffer[i++] = byte_to_hex(tmp % 16);
		tmp /= 16;
	}
	if (n < 0)
		buffer[i] = '-';
	return (ft_strrev(buffer));
}
