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

int	del_obj(const void *restrict obj_data) {
	const struct s_test *restrict test_data = obj_data;
	return !(1 == test_data->val && !strcmp("test2", test_data->str));
}

int	main(void) {
	struct s_test	s[3] = {
		{ "test1", 0 },
		{ "test2", 1 },
		{ "test3", 2 }
	};
	dll_t *restrict	list = dll_init();

	dll_pushfront(list, &s[0], sizeof(s[0]), DLL_BIT_EIGN);
	dll_pushfront(list, &s[1], sizeof(s[1]), DLL_BIT_EIGN);
	dll_pushback(list, &s[2], sizeof(s[2]), DLL_BIT_EIGN);
	dll_print(list, print_object);
	if (dll_delkey(list, del_obj)) {
		printf("the desired object successfully deleted\n");
	}
	dll_print(list, print_object);
	dll_free(list);
}
