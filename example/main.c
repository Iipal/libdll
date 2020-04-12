#include "header.h"

#include <time.h>

int	main(void) {
	struct s_test	s[5] = {
		{ strdup("test1"), 0 },
		{ strdup("test2"), 1 },
		{ strdup("test3"), 2 },
		{ strdup("test4"), 3 },
		{ strdup("test5"), 4 }
	};
	struct timespec start, end;
	clock_gettime(CLOCK_REALTIME, &start);
	dll_t *restrict	list = dll_init(DLL_BIT_DFLT);

	dll_pushfront(list, &s[0], sizeof(s[0]), DLL_BIT_DFLT, free_obj);
	dll_pushfront(list, &s[1], sizeof(s[1]), DLL_BIT_DFLT, free_obj);
	dll_pushback(list, &s[2], sizeof(s[2]), DLL_BIT_DFLT, free_obj);
	dll_pushfront(list, &s[3], sizeof(s[3]), DLL_BIT_DFLT, free_obj);
	dll_pushback(list, &s[4], sizeof(s[4]), DLL_BIT_DFLT, free_obj);
	dll_printr(list, print_object);

	printf("\ndeleteing object with val == 1 and str == 'test2':\n");
	dll_assert(dll_delkey(list, match_obj1, &s[1]));
	printf("\tthe desired object successfully deleted\n");
	dll_print(list, print_object);

	printf("\nPrint object founded by index 2 from end:\n");
	dll_obj_t *restrict	found = dll_findidr(list, 2);
	dll_printone(found, print_object, dll_getid(list, found));

	printf("Print object founded by index 1 from start:\n");
	found = dll_findid(list, 1);
	dll_printone(found, print_object, dll_getid(list, found));

	printf("Print 2 objects from start from index 2:\n");
	dll_printn(list, print_object, 2, 2);

	printf("\nDuplicate 2 objects from position 1 from list with %zu objects:\n", dll_getsize(list));
	dll_t *restrict	dup = dll_dup(list, 1, 2);
	dll_print(dup, print_object);

	printf("Duplicated list can be succesfully deleted without deleting anything from parent-list\n");
	dll_free(&dup);
	dll_print(list, print_object);

	dll_free(&list);

	clock_gettime(CLOCK_REALTIME, &end);
	double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
	fprintf(stderr, "\n --- time: %lf sec. ---\n\n", time_spent);

	printf("\t!!! Test dll_assert for free list !!!\n\n");
	dll_assert(dll_delkey(list, match_obj2, NULL));
}
