#include "intersection_cylinder.h"

// https://math.stackexchange.com/a/3248403

// t^2 = (r^2 - d^2) / b^2;

// xVec = cylinder direction cross D (ray direction)
// yVec = cylinder direction
// zVec = yVec cross xVec
// QP = (CO dot xVec) mult xVec

// d = QP dot xVec
// b = D dot zVec
// y = D dot yVec

// get t
// P = d * xVec + b * t * zVec + y * t * yVec

// then you calculate T and R matrixes, multiply them, invert and apply to P
// or...

// https://stackoverflow.com/a/37793274

static void	intersection_cylinder(
	t_rtv *rtv,
	t_intersect_params *params,
	t_byte idx,
	double t[2]
)
{
	const t_intersection_cylinder	data = {
		rtv->cylinders[idx].vectors[VCTR_CYLINDER_C0],
		vector_sub(params->O, data.C),
		rtv->cylinders[idx].vectors[VCTR_CYLINDER_C0C1],
		vector_cross(data.CO, data.CQ),
		vector_cross(params->D, data.CQ),
		vector_dot(data.Z, data.Z),
		2 * vector_dot(data.Z, data.X),
		vector_dot(data.X, data.X) \
			- (rtv->cylinders[idx].radius2 * vector_dot(data.CQ, data.CQ)),
		(data.b * data.b) - (4.0 * data.a * data.c),
		sqrt(data.cubed)
	};

	if (data.cubed < 0)
	{
		t[0] = 1.0 / 0.0;
		t[1] = 1.0 / 0.0;
		return ;
	}
	t[0] = (-1 * data.b - data.sqrt) / (2.0 * data.a);
	t[1] = (-1 * data.b + data.sqrt) / (2.0 * data.a);
}

t_intersection	intersection_cylinder_closest(
	t_rtv *rtv,
	t_intersect_params *params
)
{
	t_intersection	ret;
	t_byte			i;
	double			t[2];

	ret.distance = 1.0 / 0.0;
	ret.idx = -1;
	ret.type = ICYLINDER;
	i = 0;
	while (i < MAX_CYLINDERS)
	{
		if (!(rtv->cylinders[i].traits & TRAIT_EXISTS))
		{
			i++;
			continue ;
		}
		intersection_cylinder(rtv, params, i, t);
		check_intersection(&ret, params, t[0], i);
		check_intersection(&ret, params, t[1], i);
		i++;
	}
	return (ret);
}
