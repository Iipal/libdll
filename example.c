#include "libdll.h"

#include <string.h>

struct s_test {
	char *restrict	str;
	int		val;
};

int	print_object(const void *restrict obj_data) {
	const struct s_test *restrict test_data = obj_data;
	printf("\t%d -- '%s'\n", test_data->val, test_data->str);
	return 0;
}

int	del_obj1(const void *restrict obj_data) {
	const struct s_test *restrict test_data = obj_data;
	return !(1 == test_data->val && !strcmp("test2", test_data->str));
}

int	del_obj2(const void *restrict obj_data) {
	const struct s_test *restrict test_data = obj_data;
	return !(2 == test_data->val && !strcmp("test3", test_data->str));
}

int	del_obj2err(const void *restrict obj_data) {
	const struct s_test *restrict test_data = obj_data;
	const bool ret = !(2 == test_data->val && !strcmp("test3", test_data->str));
	if (false == ret)
		return -1;
	return ret;
}

int	main(void) {
	struct s_test	s[5] = {
		{ "test1", 0 },
		{ "test2", 1 },
		{ "test3", 2 },
		{ "test4", 3 },
		{ "test5", 4 }
	};
	dll_t *restrict	list = dll_init(DLL_GBIT_QUIET);

	dll_pushfront(list, &s[0], sizeof(s[0]), DLL_BIT_DFLT);
	dll_pushfront(list, &s[1], sizeof(s[1]), DLL_BIT_DFLT);
	dll_pushback(list, &s[2], sizeof(s[2]), DLL_BIT_DFLT);
	dll_pushfront(list, &s[3], sizeof(s[3]), DLL_BIT_DFLT);
	dll_pushback(list, &s[4], sizeof(s[4]), DLL_BIT_DFLT);
	dll_printr(list, print_object);

	printf("\ndeleteing object with val == 1 and str == 'test2':\n");
	if (dll_delkey(list, del_obj1)) {
		printf("\tthe desired object successfully deleted\n");
	}
	dll_print(list, print_object);
	printf("Error test:\n");
	dll_delkey(list, del_obj2err);

	printf("\nPrint object founded by index 2 from end:\n");
	dll_printone(dll_findidr(list, 2), print_object);
	printf("Print object founded by index 1 from start:\n");
	dll_printone(dll_findid(list, 1), print_object);
	dll_free(list);
}
