/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop_hook_3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oadhesiv <oadhesiv@student.21-school.ru>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/11 14:59:41 by oadhesiv          #+#    #+#             */
/*   Updated: 2021/01/22 18:52:06 by oadhesiv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "loop_hook.h"

int	loop_key_hook(int keycode, t_rtv *rtv)
{
	if (keycode == KEY_ESC)
		loop_destroy_hook(rtv);
	if (keycode == KEY_R)
		rtv->flags |= FLAG_REDRAW;
	if (keycode == KEY_A)
	{
		rtv->camera_angles[AYAW] += M_PI_4F;
		rtv->flags |= FLAG_INVALIDATE_ROTATION;
	}
	if (keycode == KEY_D)
	{
		rtv->camera_angles[AYAW] -= M_PI_4F;
		rtv->flags |= FLAG_INVALIDATE_ROTATION;
	}
	return (0);
}
