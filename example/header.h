#ifndef HEADER_H
# define HEADER_H

# include "../libdll.h"

struct s_test {
	char *restrict	str;
	int		val;
};

int	print_object(const void *restrict obj_data, size_t idx);
void	free_obj(void *restrict data);

int	match_slash(const void *restrict obj_data, void *restrict any_data);
int	match_obj1(const void *restrict obj_data, void *restrict any_data);
int	match_obj2(const void *restrict obj_data, void *restrict any_data);
int	match_obj2err(const void *restrict obj_data, void *restrict any_data);

#endif /* HEADER_H */
