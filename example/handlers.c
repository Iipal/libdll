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

int	match_obj1(const void *restrict obj_data, void *restrict any_data) {
	(void)any_data;
	const struct s_test *restrict test_data = obj_data;
	return !(1 == test_data->val && !strcmp("test2", test_data->str));
}

int	match_obj2(const void *restrict obj_data, void *restrict any_data) {
	(void)any_data;
	const struct s_test *restrict test_data = obj_data;
	return !(2 == test_data->val && !strcmp("test3", test_data->str));
}

int	match_obj2err(const void *restrict obj_data, void *restrict any_data) {
	(void)any_data;
	const struct s_test *restrict test_data = obj_data;
	const bool ret = (2 == test_data->val && !strcmp("test3", test_data->str));
	if (true == ret)
		return -42;
	return !ret;
}
