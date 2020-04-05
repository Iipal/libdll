#include "header.h"

int	main(void) {
	struct s_test	s[5] = {
		{ strdup("test1"), 0 },
		{ strdup("test2"), 1 },
		{ strdup("test3"), 2 },
		{ strdup("test4"), 3 },
		{ strdup("test5"), 4 }
	};

	dll_t *restrict	list = dll_init(DLL_BIT_DFLT);

	dll_pushfront(list, &s[0], sizeof(s[0]), DLL_BIT_DFLT, free_obj);
	dll_pushfront(list, &s[1], sizeof(s[1]), DLL_BIT_DFLT, free_obj);
	dll_pushback(list, &s[2], sizeof(s[2]), DLL_BIT_DFLT, free_obj);
	dll_pushfront(list, &s[3], sizeof(s[3]), DLL_BIT_DFLT, free_obj);
	dll_pushback(list, &s[4], sizeof(s[4]), DLL_BIT_DFLT, free_obj);
	dll_printr(list, print_object);

	printf("\ndeleteing object with val == 1 and str == 'test2':\n");
	if (dll_delkey(list, match_obj1)) {
		printf("\tthe desired object successfully deleted\n");
	}
	dll_print(list, print_object);

	printf("\nPrint object founded by index 2 from end:\n");
	dll_printone(dll_findidr(list, 2), print_object);

	printf("Print object founded by index 1 from start:\n");
	dll_printone(dll_findid(list, 1), print_object);

	dll_free(list);

	printf("dll_assert test after freed list:\n");
	dll_assert(dll_delkey(list, match_obj2err));
}
