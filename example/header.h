#ifndef HEADER_H
# define HEADER_H

# define LIBDLL_NO_ERRNO
# include "../libdll.h"

struct s_test {
	char *restrict	str;
	int		val;
};

ssize_t	print_object(void *restrict obj_data, void *restrict any_data, size_t idx);
ssize_t	match_slash(void *restrict obj_data, void *restrict any_data, size_t idx);
ssize_t	match_obj1(void *restrict obj_data, void *restrict any_data, size_t idx);
ssize_t	match_obj2(void *restrict obj_data, void *restrict any_data, size_t idx);
ssize_t	match_obj2err(void *restrict obj_data, void *restrict any_data, size_t idx);

void	free_obj(void *restrict data);

#endif /* HEADER_H */
