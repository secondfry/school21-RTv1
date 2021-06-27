/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loop_hook_redraw.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oadhesiv <secondfry+school21@gmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/11 10:39:04 by oadhesiv          #+#    #+#             */
/*   Updated: 2021/06/27 16:45:04 by oadhesiv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "loop_hook.h"

static float	light_point(t_rtv *rtv, t_vector_4 P, t_vector_4 N, t_vector_4 V, float specular)
{
	float		intensity;
	float		dot;

	intensity = 0;
	for (t_byte i = 0; i < MAX_PLIGHTS; i++) {
		if (!(rtv->plights[i].traits & TRAIT_EXISTS))
			continue ;
		t_vector_4 L = vector_sub(rtv->plights[i].position, P);
		t_intersection res = intersection_closest(rtv, &((t_intersect_params) { P, L, EPSILON, 1.f }));
		if (res.distance != 1.0 / 0.0)
			continue ;
		dot = vector_dot(N, L);
		if (dot > 0) {
			float delta = rtv->plights[i].intensity *
				dot / vector_length(L);
			intensity += delta;
		}
		if (specular == -1)
			continue ;
		t_vector_4 R = vector_sub(vector_mult(N, 2 * vector_dot(N, L)), L);
		dot = vector_dot(R, V);
		if (dot > 0) {
			float delta = rtv->plights[i].intensity *
				powf(dot / vector_length(R) / vector_length(V), specular);
			intensity += delta;
		}
	}
	return (intensity);
}

static float	light_directional(t_rtv *rtv, t_vector_4 P, t_vector_4 N, t_vector_4 V, float specular)
{
	float		intensity;
	float		dot;

	intensity = 0;
	for (t_byte i = 0; i < MAX_DLIGHTS; i++) {
		if (!(rtv->dlights[i].traits & TRAIT_EXISTS))
			continue;
		t_vector_4 L = rtv->dlights[i].direction;
		t_intersection res = intersection_closest(rtv, &((t_intersect_params) { P, L, EPSILON, 1.0 / 0.0 }));
		if (res.distance != 1.0 / 0.0)
			continue;
		dot = vector_dot(N, L);
		if (dot > 0) {
			float delta = rtv->dlights[i].intensity *
				dot / vector_length(L);
			intensity += delta;
		}
		if (specular == -1)
			continue;
		t_vector_4 R = vector_sub(vector_mult(N, 2 * vector_dot(N, L)), L);
		dot = vector_dot(R, V);
		if (dot > 0) {
			float delta = rtv->dlights[i].intensity *
				powf(dot / vector_length(R) / vector_length(V), specular);
			intensity += delta;
		}
	}
	return intensity;
}

static float	light(t_rtv *rtv, t_vector_4 P, t_vector_4 N, t_vector_4 V, float specular)
{
	float	intensity;

	intensity = rtv->ambient +\
		light_point(rtv, P, N, V, specular) +\
		light_directional(rtv, P, N, V, specular);
	return intensity;
}

static t_color	raytrace(t_rtv *rtv, t_worker_data *data, float t_min, float t_max)
{
	t_intersection	res;

	res = intersection_closest(rtv, &((t_intersect_params) { data->vectors[VCTR_O], data->vectors[VCTR_D], t_min, t_max }));
	if (res.distance == 1.0 / 0.0)
		return color_new(0, 0, 0);
	t_vector_4 P = vector_add(data->vectors[VCTR_O], vector_mult(data->vectors[VCTR_D], res.distance));
	t_vector_4 N = vector_sub(P, rtv->spheres[res.idx].vectors[VCTR_C]);
	t_vector_4 NN = vector_normalize(N);
	t_vector_4 V = vector_mult(data->vectors[VCTR_D], -1);
	float intensity = light(rtv, P, NN, V, rtv->spheres[res.idx].specular);
	return color_mult(rtv->spheres[res.idx].color, intensity);
}

static void canvas_to_screen(t_rtv *rtv, short xc, short yc, t_color color)
{
	t_ushort	xs;
	t_ushort	ys;

	xs = WIDTH / 2 + xc;
	ys = HEIGHT / 2 - yc;
	rtv->mlx->img_data[ys * rtv->mlx->size_line_int + xs] = color_to_int(color);
}

static void	process_pixel(t_rtv *rtv, short xc, short yc)
{
	t_color 			color;
	const t_vector_4	D = { (float) xc / WIDTH, (float) yc / HEIGHT, 1.f, 0 };
	const t_vector_4	Drot = matrix_on_vector(rtv->camera_rotation, D);
	t_worker_data		data;

	vector_set(data.vectors + VCTR_O, &rtv->camera_position);
	vector_set(data.vectors + VCTR_D, &Drot);
	data.floats[D_DOT_D] = vector_dot(data.vectors[VCTR_D], data.vectors[VCTR_D]);
	color = raytrace(rtv, &data, 1.0f, 1.0 / 0.0);
	canvas_to_screen(rtv, xc, yc, color);
	free(color);
}

#include <pthread.h>

typedef struct	s_params {
	t_rtv *rtv;
	short ya;
	short yb;
	short xa;
	short xb;
}				t_params;

#define THREAD_COUNT 8

void	*hello(void *params) {
    for (short yc = ((t_params *)params)->ya; yc <= ((t_params *)params)->yb; yc++) {
		for (short xc = ((t_params *)params)->xa; xc < ((t_params *)params)->xb; xc++) {
			process_pixel(((t_params *)params)->rtv, xc, yc);
		}
	}
	
	pthread_exit(NULL);
}

void	loop_redraw(t_rtv *rtv)
{
	if (!(rtv->flags & FLAG_REDRAW))
		return ;
	ft_bzero(rtv->mlx->img_data, rtv->mlx->size_line_char * HEIGHT);
	rtv->flags -= FLAG_REDRAW;

	// pthread_t	tid[THREAD_COUNT];
	// t_params	param[THREAD_COUNT];
	// short		ya = -1 * HEIGHT / 2;
	// short		xa = -1 * WIDTH / 2;
	// for (t_byte y = 0; y < 2; y++) {
	// 	for (t_byte x = 0; x < 4; x++) {
	// 		param[y * 4 + x] = (t_params) {
	// 			rtv,
	// 			ya + y * HEIGHT / 2,
	// 			ya + (y + 1) * HEIGHT / 2,
	// 			xa + x * WIDTH / 4,
	// 			xa + (x + 1) * WIDTH / 4
	// 		};
	// 	}
	// }
	// param[0] = (t_params) {
	// 	rtv,
	// 	ya + 1,
	// 	ya + HEIGHT / 2,
	// 	xa,
	// 	xa + WIDTH / 4
	// };
	// for (t_byte i = 0; i < THREAD_COUNT; i++) {
    // 	pthread_create(&tid[i], NULL, hello, &param[i]);
	// }
	// for (t_byte i = 0; i < THREAD_COUNT; i++) {
    // 	pthread_join(tid[i], NULL);
	// }

	for (short yc = -1 * HEIGHT / 2 + 1; yc <= HEIGHT / 2; yc++) {
		for (short xc = -1 * WIDTH / 2; xc < WIDTH / 2; xc++) {
			process_pixel(rtv, xc, yc);
		}
	}
	mlx_put_image_to_window(\
		rtv->mlx->mlx, rtv->mlx->win, rtv->mlx->img, 0, 0);
}
