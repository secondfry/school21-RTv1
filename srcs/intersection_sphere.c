#include "intersection_sphere.h"

// https://www.gabrielgambetta.com/
// computer-graphics-from-scratch/04-shadows-and-reflections.html

static void	intersection_sphere(
	t_rtv *rtv,
	t_intersect_params *params,
	t_byte idx,
	double t[2]
)
{
	const t_intersection_sphere	data = {
		rtv->spheres[idx].vectors[VCTR_SPHERE_C],
		vector_sub(params->O, data.C),
		vector_dot(params->D, params->D),
		2 * vector_dot(params->D, data.CO),
		vector_dot(data.CO, data.CO) - rtv->spheres[idx].radius_squared,
		data.b * data.b - 4 * data.a * data.c,
		sqrt(data.cubed)
	};

	if (data.cubed < 0)
	{
		t[0] = 1.0 / 0.0;
		t[1] = 1.0 / 0.0;
		return ;
	}
	t[0] = (-1 * data.b + data.sqrt) / (2 * data.a);
	t[1] = (-1 * data.b - data.sqrt) / (2 * data.a);
}

t_intersection	intersection_sphere_closest(
	t_rtv *rtv,
	t_intersect_params *params
)
{
	t_intersection	ret;
	t_byte			i;
	double			t[2];

	ret.distance = 1.0 / 0.0;
	ret.idx = -1;
	ret.type = ISPHERE;
	i = 0;
	while (i < MAX_SPHERES)
	{
		if (!(rtv->spheres[i].traits & TRAIT_EXISTS))
		{
			i++;
			continue ;
		}
		intersection_sphere(rtv, params, i, t);
		check_intersection(&ret, params, t[0], i);
		check_intersection(&ret, params, t[1], i);
		i++;
	}
	return (ret);
}
