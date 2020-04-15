#include "header.h"

int	print_object(const void *restrict obj_data, size_t idx) {
	const struct s_test *restrict test_data = obj_data;
	printf("\t[%2zu]: %d -- '%s'\n", idx, test_data->val, test_data->str);
	return 0;
}

void	free_obj(void *restrict data) {
	struct s_test	*t = data;
	free(t->str);
}

int	match_slash(const void *restrict obj, void *restrict ptr) {
	(void)ptr;
	const struct s_test *restrict	match_obj = obj;

	if ('_' == *match_obj->str)
		return 0;
	return 1;
}

int	match_obj1(const void *restrict obj, void *restrict cmp) {
	struct s_test *restrict cmp_data = cmp;
	const struct s_test *restrict test_data = obj;

	return !(cmp_data->val == test_data->val
		&& !strcmp(cmp_data->str, test_data->str));
}

int	match_obj2(const void *restrict obj, void *restrict cmp) {
	struct s_test *restrict cmp_data = cmp;
	const struct s_test *restrict test_data = obj;
	return !(cmp_data->val == test_data->val
		&& !strcmp(cmp_data->str, test_data->str));
}

int	match_obj2err(const void *restrict obj, void *restrict cmp) {
	const struct s_test *restrict cmp_data = cmp;
	const struct s_test *restrict test_data = obj;
	const bool ret = (cmp_data->val == test_data->val
			&& !strcmp(cmp_data->str, test_data->str));
	if (true == ret)
		return -42;
	return !ret;
}
