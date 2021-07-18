/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validator.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oadhesiv <secondfry+school21@gmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/04 17:45:52 by pcarolei          #+#    #+#             */
/*   Updated: 2021/07/18 18:14:43 by oadhesiv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "color.h"
#include "vector.h"

static t_vector_4	get_vector(t_level *root)
{
	float	coord[3];
	t_byte	i;
	t_level	*level;

	coord[0] = 0;
	coord[1] = 0;
	coord[2] = 0;
	i = 0;
	while (i < root->data->used)
	{
		level = root->data->data[i];
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "x"))
			coord[0] = ft_atoi(level->value);
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "y"))
			coord[1] = ft_atoi(level->value);
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "z"))
			coord[2] = ft_atoi(level->value);
		i++;
	}
	return ((t_vector_4){coord[0], coord[1], coord[2], 1.f});
}

//	TODO: дописать валидатор
void		validate(t_rtv *rtv, t_level *root)
{
	for (t_byte i = 0; i < root->data->used; i++)
	{
		t_level *level = root->data->data[i];
		check(level->type != LTYPE_NODE, 1, ERR_VALIDATOR_ROOT_NODES);
		if (!ft_strcmp(level->key, "light"))
			validate_light(rtv, level);
		if (!ft_strcmp(level->key, "sphere"))
			validate_object(rtv, level, 2);
		if (!ft_strcmp(level->key, "plane"))
			validate_object(rtv, level, 3);
		if (!ft_strcmp(level->key, "cylinder"))
			validate_object(rtv, level, 4);
		if (!ft_strcmp(level->key, "cone"))
			validate_object(rtv, level, 5);
	}
}

t_byte		validate_light_ambient(t_rtv *rtv, t_level *root)
{
	rtv->ambient = (float) ft_atoi(root->value) / 100.f;
	return (1);
}

t_byte		validate_light_point_one_position(t_rtv *rtv, t_level *root, t_byte idx)
{
	const t_vector_4 position = get_vector(root);

	ft_memcpy((void *)&rtv->plights[idx].position, (void *)&position, sizeof(t_vector_4));
	return (1);
}

t_byte		validate_light_point_one_intensity(t_rtv *rtv, t_level *root, t_byte idx)
{
	rtv->plights[idx].intensity = (float) ft_atoi(root->value) / 100.f;
	return (1);
}

t_byte		validate_light_point_one(t_rtv *rtv, t_level *root, t_byte idx)
{
	t_byte res;

	root = root->data->data[0];
	check(root->type != LTYPE_NODE, 1, ERR_VALIDATOR_LIGHT_POINT_INVALID);
	res = 0;
	for (t_byte i = 0; i < root->data->used; i++)
	{
		t_level *level = root->data->data[i];
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "position"))
			res += validate_light_point_one_position(rtv, level, idx);
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "intensity"))
			res += validate_light_point_one_intensity(rtv, level, idx);
	}
	check(res != 2, 1, ERR_VALIDATOR_LIGHT_POINT_INVALID);
	rtv->plights[idx].traits = TRAIT_EXISTS;
	return (1);
}

t_byte		validate_light_point(t_rtv *rtv, t_level *root)
{
	t_byte res;

	res = 0;
	for (t_byte i = 0; i < root->data->used; i++)
	{
		t_level *level = root->data->data[i];
		check(level->type != LTYPE_LIST_NODE, 1, ERR_VALIDATOR_LIGHT_POINT_LIST);
		res += validate_light_point_one(rtv, level, i);
	}
	if (res == 0)
		ft_putendl("[validator] Scene has no point lights");
	return (1);
}

t_byte validate_light_directional_one_directional(t_rtv *rtv, t_level *root, t_byte idx)
{
	const t_vector_4 direction = vector_normalize(get_vector(root));
	const float w = 0.f;

	ft_memcpy((void *)&direction.w, (void *)&w, sizeof(float));
	ft_memcpy((void *)&rtv->dlights[idx].direction, (void *)&direction, sizeof(t_vector_4));
	return (1);
}


t_byte		validate_light_directional_one_intensity(t_rtv *rtv, t_level *root, t_byte idx)
{
	rtv->dlights[idx].intensity = (float) ft_atoi(root->value) / 100.f;
	return (1);
}

t_byte		validate_light_directional_one(t_rtv *rtv, t_level *root, t_byte idx)
{
	t_byte res;

	root = root->data->data[0];
	check(root->type != LTYPE_NODE, 1, ERR_VALIDATOR_LIGHT_DIRECTIONAL_INVALID);
	res = 0;
	for (t_byte i = 0; i < root->data->used; i++)
	{
		t_level *level = root->data->data[i];
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "direction"))
		{
			res += validate_light_directional_one_directional(rtv, level, idx);
			t_vector_4 *v = &(rtv->dlights[idx].direction);
		}
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "intensity"))
		{
			res += validate_light_directional_one_intensity(rtv, level, idx);
			float i = rtv->dlights[idx].intensity;
		}
	}
	check(res != 2, 1, ERR_VALIDATOR_LIGHT_DIRECTIONAL_INVALID);
	rtv->dlights[idx].traits = TRAIT_EXISTS;
	return (1);
}

t_byte		validate_light_directional(t_rtv *rtv, t_level *root)
{
	t_byte res;

	res = 0;
	for (t_byte i = 0; i < root->data->used; i++)
	{
		t_level *level = root->data->data[i];
		check(level->type != LTYPE_LIST_NODE, 1, ERR_VALIDATOR_LIGHT_DIRECTIONAL_LIST);
		res += validate_light_directional_one(rtv, level, i);
	}
	if (res == 0)
		ft_putendl("[validator] Scene has no directional lights");
	return (1);
}

/**
 *	Функция для печати информации об "уровне" в карте
 */
void		print_level_info(t_level *level)
{
	return ;
	if (level->type == 0)
	if (level->type == 1)
	if (level->type == 2)
	if (level->type == 3)
	if (level->type == 4)


	if (level->data)
	return ;
}

t_byte		validate_light(t_rtv *rtv, t_level *root)
{
	t_byte res;

	res = 0;
	for (t_byte i = 0; i < root->data->used; i++)
	{
		t_level *level = root->data->data[i];
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "ambient"))
			res += validate_light_ambient(rtv, level);
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "point"))
			res += validate_light_point(rtv, level);
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "directional"))
			res += validate_light_directional(rtv, level);
	}
	check(res != 3, 1, ERR_VALIDATOR_LIGHT_INCOMPLETE);
	return (1);
}

/**
 *	Функция для валидации списка объектов (света/сфер/плоскостей/конусов/цилиндров)
 */
t_byte		validate_object(t_rtv *rtv, t_level *root, t_byte obj_type)
{
	t_byte	res;
	t_level	*level;

	res = 0;

	//	Проверяем, что на вход пришла нода
	check(root->type != LTYPE_NODE, 1, "[ERR] EXPECTED NODE\n");
	//	Проверяем, что описано не менее одного объекта
	check(!root->data || !root->data->used, 0, "[ERR] EXPECTED ONE OR MORE OBJECTS\n");

	print_level_info(root);

	for (t_byte i = 0; i < root->data->used; i++)
	{
		//	Вытаскиваем список со сферами
		level = root->data->data[i];
		print_level_info(level);
		if (obj_type == 2)
			validate_sphere(rtv, level->data->data[0], i);
		if (obj_type == 3)
			validate_plane(rtv, level->data->data[0], i);
		if (obj_type == 4)
			validate_cylinder(rtv, level->data->data[0], i);
		if (obj_type == 5)
			validate_cone(rtv, level->data->data[0], i);
	}
	return (1);
}

/**
 *	Функция для валидации параметров конуса
 */
t_byte		validate_cone(t_rtv *rtv, t_level *root, t_byte idx)
{
	t_byte		res;

	res = 0;

	t_vector_4 vec_start = (t_vector_4) {0, 0, 0, 0};
	t_vector_4 vec_end = (t_vector_4) {0, 0, 0, 0};
	for (t_byte i = 0; i < root->data->used; i++)
	{
		t_level *level = root->data->data[i];

		print_level_info(level);
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "position"))
		{
			res += validate_vector(&vec_end, level);
			ft_memcpy((void *)&rtv->cones[idx].vectors[0], (void *)&vec_end, sizeof(t_vector_4));
		}
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "direction"))
		{
			res += validate_vector(&vec_start, level);
			ft_memcpy((void *)&rtv->cones[idx].vectors[1], (void *)&vec_start, sizeof(t_vector_4));
		}
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "color"))
		{
			rtv->cones[idx].color = validate_color(level);
			res++;
		}
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "angle"))
		{
			rtv->cones[idx].angle = validate_angle(level);
			rtv->cones[idx].cos = cosf(rtv->cones[idx].angle);
			rtv->cones[idx].cos2 = rtv->cones[idx].cos * rtv->cones[idx].cos;
			res++;
		}
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "specular"))
		{
			rtv->cones[idx].specular = validate_specular(level);
			res++;
		}
	}
	t_vector_4 vec_norm = vector_normalize(vector_sub(vec_start, vec_end));
	ft_memcpy((void *)&rtv->cones[idx].vectors[2], (void *)&vec_norm, sizeof(t_vector_4));
	check(res != 5, 1, "[ERR] CONE IS INVALID\n");
	rtv->cones[idx].traits = TRAIT_EXISTS;
	return (1);
}

/**
 *	Функция для валидации параметров цилиндра
 */
t_byte		validate_cylinder(t_rtv *rtv, t_level *root, t_byte idx)
{
	t_byte		res;

	res = 0;
	t_vector_4 vec_start = (t_vector_4) {0, 0, 0, 0};
	t_vector_4 vec_end = (t_vector_4) {0, 0, 0, 0};
	for (t_byte i = 0; i < root->data->used; i++)
	{
		t_level *level = root->data->data[i];

		print_level_info(level);
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "position"))
		{
			res += validate_vector(&vec_end, level);
			ft_memcpy((void *)&rtv->cylinders[idx].vectors[0], (void *)&vec_end, sizeof(t_vector_4));
		}
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "direction"))
		{
			res += validate_vector(&vec_start, level);
			ft_memcpy((void *)&rtv->cylinders[idx].vectors[1], (void *)&vec_start, sizeof(t_vector_4));
		}
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "color"))
		{
			rtv->cylinders[idx].color = validate_color(level);
			res++;
		}
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "radius"))
		{
			rtv->cylinders[idx].radius2 = validate_radius(level);
			res++;
		}
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "specular"))
		{
			rtv->cylinders[idx].specular = validate_specular(level);
			res++;
		}
	}
	t_vector_4 vec_norm = vector_normalize(vector_sub(vec_start, vec_end));
	ft_memcpy((void *)&rtv->cylinders[idx].vectors[2], (void *)&vec_norm, sizeof(t_vector_4));
	check(res != 5, 1, "[ERR] CYLINDER IS INVALID\n");
	rtv->cylinders[idx].traits = TRAIT_EXISTS;
	return (1);
}

/**
 *	Функция для валидации параметров плоскости
 */
t_byte		validate_plane(t_rtv *rtv, t_level *root, t_byte idx)
{
	t_byte	res;
	t_vector_4	*dest;

	res = 0;

	for (t_byte i = 0; i < root->data->used; i++)
	{
		t_level *level = root->data->data[i];

		print_level_info(level);
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "position"))
		{
			dest = &rtv->planes[idx].position;
			res += validate_vector(dest, level);
		}
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "normal"))
		{
			dest = &rtv->planes[idx].normal;
			res += validate_vector(dest, level);
		}
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "color"))
		{
			rtv->planes[idx].color = validate_color(level);
			res++;
		}
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "specular"))
		{
			rtv->planes[idx].specular = validate_specular(level);
			res++;
		}
	}
	check(res != 4, 1, "[ERR] PLANE IS INVALID\n");
	rtv->planes[idx].traits = TRAIT_EXISTS;
	return (1);
}

/**
 *	Функция для валидации параметров сферы
 */
t_byte		validate_sphere(t_rtv *rtv, t_level *root, t_byte idx)
{
	t_byte	res;
	t_vector_4	*dest;

	res = 0;

	for (t_byte i = 0; i < root->data->used; i++)
	{
		t_level *level = root->data->data[i];

		print_level_info(level);
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "position"))
		{
			dest = &rtv->spheres[idx].vectors[VCTR_SPHERE_C];
			res += validate_vector(dest, level);
		}
		if (level->type == LTYPE_NODE && !ft_strcmp(level->key, "color"))
		{
			rtv->spheres[idx].color = validate_color(level);
			res++;
		}
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "radius"))
		{
			rtv->spheres[idx].radius_squared = validate_radius(level);
			res++;
		}
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "specular"))
		{
			rtv->spheres[idx].specular = validate_specular(level);
			res++;
		}
	}
	check(res != 4, 1, "[ERR] SPHERE IS INVALID\n");
	rtv->spheres[idx].traits = TRAIT_EXISTS;
	return (1);
}

/**
 *	Функция для валидации вектора
 */
t_byte	validate_vector(t_vector_4 *dest, t_level *root)
{
	t_byte				res;

	res = 0;
	check(root->data->used != 3, 1, "[ERR] VECTOR HAVE WRONG AMOUNT OF PARAMETERS\n");
	for (t_byte i = 0; i < root->data->used; i++)
	{
		t_level *level = root->data->data[i];

		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "x"))
		{
			res++;
		}
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "y"))
		{
			res++;
		}
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "z"))
		{
			res++;
		}
	}
	check(res != 3, 1, "[ERR] VECTOR PARAMETERS ERROR\n");
	// const t_vector_4 *vector = get_vector(root);
	// const t_vector_4 *v = (t_vector_4 *)(dest);
	t_vector_4 r = get_vector(root);

	*dest = r;
	// dest = (void *)v;
	// const t_vector_4 v = get_vector(root);;
	// ft_memcpy(dest, (void *)&vector, sizeof(t_vector_4));
	return (1);
}


/**
 *	Функция для валидации компонента цвета (представленного целым числом от нуля до 255)
 */
t_byte	validate_color_component(char *value)
{
	t_byte component;

	component = ft_atoi(value);
	check((component < 0) || (component > 255), 1, "[ERR] COLOR COMPONENT HAS INCORRECT VALUE\n");
	return component;
}

/**
 *	Функция для валидации цвета
 */
t_color	validate_color(t_level *root)
{
	t_color				color;
	t_byte				red;
	t_byte				green;
	t_byte				blue;

	print_level_info(root);
	check(root->data->used != 3, 1, "[ERR] COLOR HAVE WRONG AMOUNT OF PARAMETERS\n");
	red = -1;
	green = -1;
	blue = -1;
	for (t_byte i = 0; i < root->data->used; i++)
	{
		t_level *level = root->data->data[i];

		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "red"))
		{
			red = validate_color_component(level->value);
		}
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "green"))
		{
			green = validate_color_component(level->value);
		}
		if (level->type == LTYPE_LEAF && !ft_strcmp(level->key, "blue"))
		{
			blue = validate_color_component(level->value);
		}
	}
	check((red < 0) || (green < 0) || (blue < 0), 1, "[ERR] COLOR PARAMETERS ERROR\n");
	color = color_new(red, green, blue);
	return (color);
}

/**aaa
 *	Функция для валидации радиуса
 *	@returns Квадрат радиуса
 */
t_byte	validate_radius(t_level *root)
{
	float	radius;

	print_level_info(root);
	radius = -1;
	if (root->type == LTYPE_LEAF && !ft_strcmp(root->key, "radius"))
	{
		radius = ft_atoi(root->value);
	}
	check(radius < 0, 1, "[ERR] INVALID RADIUS INPUT\n");
	return (radius * radius);
}

/**
 *	Функция для валидации зеркальности
 */
t_byte	validate_specular(t_level *root)
{
	float	specular;

	print_level_info(root);
	specular = -1;
	if (root->type == LTYPE_LEAF && !ft_strcmp(root->key, "specular"))
		specular = ft_atoi(root->value);
	return (specular);
}

/**
 *	Функция для валидации угла
 */
float	validate_angle(t_level *root)
{
	//	TODO: объединить в функцию валидации скаляра вместе с specular
	float	angle;

	print_level_info(root);
	angle = -1;
	if (root->type == LTYPE_LEAF && !ft_strcmp(root->key, "angle"))
		angle = (float)ft_atoi(root->value) * M_PI_F / 180.f,
	return (angle);
}
