#ifndef PARSER_H
# define PARSER_H

# include <fcntl.h>
# include <stdlib.h>
# include <unistd.h>
# include "array.h"
# include "defines.h"
# include "get_next_line.h"
# include "graceful.h"
# include "color.h"
# include "vector.h"

# define LTYPE_UNSET		0
# define LTYPE_NODE			1
# define LTYPE_LEAF			2
# define LTYPE_LIST_NODE	3
# define LTYPE_LIST_LEAF	4

typedef struct s_level
{
	char			*key;
	char			*value;
	t_ptr_array		*data;
	t_byte			type;
	struct s_level	*parent;
	short			offset;
	short			child_offset;
}				t_level;

typedef struct s_process_type_args
{
	int			fd;
	char		**memory;
	const char	*line;
}				t_process_type_args;

# define ERR_PARSER_NO_COLON "Line must either be filled with spaces which I \
disgrace because you should really not fill your configuration files with \
extra unneeded spaces but for some reason I have to workaround this exact \
condition...\n or have colon in it."
# define ERR_PARSER_ARGC "Provide scene argument, please. Must be an \
accesible oadYAML file."

/**
 *	Коды ошибок парсера
 */
# define ERR_PARSER_NO_COLON "Line must either be filled with spaces which \
I disgrace because you should really not fill your configuration files \
with extra unneeded spaces but for some reason I have to workaround \
this exact condition...\n or have colon in it."
# define ERR_PARSER_ARGC "Provide scene argument, please. \
Must be an accesible oadYAML file."
# define ERR_PARSER_OPEN "Can't open provided argument as file."
# define ERR_PARSER_READ "Can't read from file. Is it a file?"
# define ERR_PARSER_GNL "Could read from file, but can't anymore. Strange..."
# define ERR_PARSER_INVALID_OFFSET "Your oadYAML has wrong line offsets."
# define ERR_PARSER_EMPTY_NODE "Your oadYAML has empty nodes."
# define ERR_PARSER_OADYAML_LIST_NAN "Your oadYAML has empty list entry \
definition."
# define ERR_VALIDATOR_ROOT_NODES "[validator] Root parts of configuration \
expected to be nodes."
# define ERR_VALIDATOR_LIGHT_INCOMPLETE "[validator] Light configuration is \
incomplete."
# define ERR_VALIDATOR_LIGHT_POINT_LIST "[validator] Point lights \
configuration members expected to list nodes."
# define ERR_VALIDATOR_LIGHT_POINT_INVALID "[validator] Point light \
configuration is invalid."
# define ERR_VALIDATOR_LIGHT_DIRECTIONAL_LIST "[validator] Directional \
lights configuration members expected to list nodes."
# define ERR_VALIDATOR_LIGHT_DIRECTIONAL_INVALID "[validator] Directional \
light configuration is invalid."
# define ERR_VALIDATOR_CAMERA_INCOMPLETE "[validator] Camera configuration is \
incomplete."
# define ERR_PARSER_OADYAML_LIST_NAN "Your oadYAML has empty \
list entry definition."

# define ERR_VALIDATOR_SPHERE_INCOMPLETE "[validator] Sphere configuration \
is incomplete."

t_vector_4	get_vector(t_level *root);
void		validate(t_rtv *rtv, t_level *root);
//	Валидация света
t_byte		validate_light(t_rtv *rtv, t_level *root);
t_byte		validate_light_point(t_rtv *rtv, t_level *root);
t_byte		validate_light_directional(t_rtv *rtv, t_level *root);
//	Общие функции валидации
t_byte		validate_object(t_rtv *rtv, t_level *root, t_byte obj_type);
t_byte		validate_sphere(t_rtv *rtv, t_level *root, t_byte idx);
t_byte		validate_plane(t_rtv *rtv, t_level *root, t_byte idx);
t_byte		validate_cylinder(t_rtv *rtv, t_level *root, t_byte idx);
t_byte		validate_cone(t_rtv *rtv, t_level *root, t_byte idx);
t_byte		validate_vector(t_vector_4 *dest, t_level *root);
t_color		validate_color(t_level *root);
double		validate_radius(t_level *root);
double		validate_specular(t_level *root);
double		validate_angle(t_level *root);
t_byte		validate_camera(t_rtv *rtv, t_level *root);

int			check_arguments(int argc, char **argv);

t_level		*level_new(void);
t_level		*level_from_line(const char *line);
t_level		*parse(int fd, char **memory);
void		parser(t_rtv *rtv, int argc, char **argv);

short		get_offset(char *line);
void		init_root(t_level **root, char **memory);
void		process_parent_child(t_level **parent, t_level *child);

#endif
