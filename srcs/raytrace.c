#include "raytrace.h"

static t_light_params	get_light_params(t_rtv *rtv, t_intersection *intr)
{
	if (intr->type == ISPHERE)
		return ((t_light_params){\
			.C = rtv->spheres[intr->idx].vectors[VCTR_SPHERE_C], \
			.color = rtv->spheres[intr->idx].color, \
			.specular = rtv->spheres[intr->idx].specular \
		});
	if (intr->type == IPLANE)
		return ((t_light_params){\
			.C = rtv->planes[intr->idx].position, \
			.color = rtv->planes[intr->idx].color, \
			.specular = rtv->planes[intr->idx].specular \
		});
	if (intr->type == ICYLINDER)
		return ((t_light_params){\
			.C = rtv->cylinders[intr->idx].vectors[VCTR_CYLINDER_C0], \
			.color = rtv->cylinders[intr->idx].color, \
			.specular = rtv->cylinders[intr->idx].specular \
		});
	if (intr->type == ICONE)
		return ((t_light_params){\
			.C = rtv->cones[intr->idx].vectors[VCTR_CONE_C0], \
			.color = rtv->cones[intr->idx].color, \
			.specular = rtv->cones[intr->idx].specular \
		});
	return ((t_light_params){});
}

static t_color	pre_light(t_rtv *rtv, t_worker_data *data, t_intersection *intr)
{
	const t_light_params	params = get_light_params(rtv, intr);
	float					intensity;

	vector_set_by_value(&params.P, vector_add(\
		data->vectors[VCTR_O], \
		vector_mult(data->vectors[VCTR_D], intr->distance) \
	));
	vector_set_by_value(\
		&params.N, find_normal(rtv, intr, &params));
	vector_set_by_value(&params.V, vector_mult(data->vectors[VCTR_D], -1));
	intensity = light(rtv, &params);
	return (*color_mult(&params.color, intensity));
}

static t_color	raytrace(
	t_rtv *rtv,
	t_worker_data *data,
	float t_min,
	float t_max
)
{
	t_intersection	intr;

	intr = intersection_closest(\
		rtv, \
		&((t_intersect_params){\
			data->vectors[VCTR_O], \
			data->vectors[VCTR_D], \
			t_min, \
			t_max \
		}) \
	);
	if (intr.distance == 1.0 / 0.0)
		return ((t_color){0, 0, 0});
	return (pre_light(rtv, data, &intr));
}

static void	canvas_to_screen(t_rtv *rtv, short xc, short yc, t_color color)
{
	t_ushort	xs;
	t_ushort	ys;

	////printf("IN CANVAS TO SCREEN. xc = %d, yc = %d\n", xc, yc);
	////printf("IN CANVAS TO SCREEN. red = %d, green = %d, blue = %d\n", color[0], color[1], color[2]);
	xs = WIDTH / 2 + xc;
	ys = HEIGHT / 2 - yc;
	////printf("IN CANVAS TO SCREEN. xs = %d, ys = %d\n", xs, ys);
	rtv->mlx->img_data[ys * rtv->mlx->size_line_int + xs] = color_to_int(&color);
}

void	process_pixel(t_rtv *rtv, short xc, short yc)
{
	t_color				color;
	// ////printf("before vector normalize\n");
	const t_vector_4	D = vector_normalize(matrix_on_vector(
		rtv->camera_rotation,
		(t_vector_4){(float) xc / WIDTH, (float) yc / HEIGHT, 1.f, 0}
	));
	t_worker_data		data;

	//////printf("before vector set1\n");
	vector_set(data.vectors + VCTR_O, &rtv->camera_position);
	////printf("before vector set2\n");
	vector_set(data.vectors + VCTR_D, &D);
	////printf("before vector dot\n");
	data.floats[D_DOT_D] = vector_dot(\
		data.vectors[VCTR_D], \
		data.vectors[VCTR_D] \
	);
	////printf("before raytrayce\n");
	color = raytrace(rtv, &data, 1.0f, 1.0 / 0.0);
	////printf("before canvas to screen\n");
	canvas_to_screen(rtv, xc, yc, color);
	////printf("before free\n");
	// free(color);
}
