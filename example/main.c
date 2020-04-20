#include "header.h"

#include <time.h>

int	main(void) {
	struct s_test	s[3] = {
		{ strdup("_test1"), 0 },
		{ strdup(".test2"), 1 },
		{ strdup("_test3"), 2 }
	};
	struct s_test	t[2] = {
		{ strdup("append1"), 0 },
		{ strdup("append2"), 1 }
	};
	struct timespec start, end;
	clock_gettime(CLOCK_REALTIME, &start);
	dll_t *restrict	list = dll_init(DLL_BIT_DFLT);

	dll_pushfront(list, &s[0], sizeof(s[0]), DLL_BIT_DFLT, free_obj);
	dll_pushfront(list, &s[1], sizeof(s[1]), DLL_BIT_DFLT, free_obj);
	dll_pushback(list, &s[2], sizeof(s[2]), DLL_BIT_DFLT, free_obj);

	printf("Appending 1 dll to other:\n");
	dll_t *restrict	list2 = dll_init(DLL_BIT_DFLT);
	for (size_t i = 0; 2 > i; ++i)
		dll_assert(dll_pushfront(list2, &t[i], sizeof(t[i]), DLL_BIT_DFLT, free_obj));
	dll_swapdll(list, list2);
	printf("src list:\n");
	dll_print(list2, print_object);
	printf("dst list:\n");
	dll_print(list, print_object);

	dll_assert(list = dll_appenddll(list2, list, dll_fnptr_any, NULL, 1, dll_getsize(list2)));
	printf("result in dst list:\n");
	dll_print(list, print_object);

	printf("Swap first and last objects:\n");
	dll_swap(dll_gethead(list), dll_getlast(list));
	dll_print(list, print_object);

	printf("Reverse list\n");
	dll_reverse(list);
	dll_print(list, print_object);

	printf("\ndeleteing object with val == 1 and str == 'test2':\n");
	dll_assert(dll_delkey(list, match_obj1, &s[1]));
	printf("\tthe desired object successfully deleted\n");
	dll_print(list, print_object);

	printf("\nPrint object founded by index 2 from end:\n");
	dll_obj_t *restrict	found = dll_findidr(list, 2);
	dll_printone(found, print_object);

	printf("Print object founded by index 1 from start:\n");
	found = dll_findid(list, 1);
	dll_printone(found, print_object);

	printf("Print 2 objects from start from index 2:\n");
	dll_printn(list, print_object, 2, 2);

	printf("\nHandler return a negative value:\n");
	dll_obj_t *restrict	obj = dll_findkey(list, match_obj2err, &s[2]);
	if (!obj)
		dll_perror("\tdll_findkey");

	printf("\nDuplicate 2 objects from position 2 from list with %zu objects:\n", dll_getsize(list));
	dll_t *restrict	dup = dll_dup(list, 2, 2);
	dll_print(dup, print_object);

	printf("\nDuplicate all strings in objects what started with '_':\n");
	dll_t *restrict	dup_key = dll_dupkey(list, match_slash, NULL, 1, dll_getsize(list));
	dll_print(dup_key, print_object);

	printf("Duplicated lists can be succesfully deleted without deleting anything from parent-list\n");
	dll_free(&dup);
	dll_free(&dup_key);
	dll_print(list, print_object);

	printf("Delete only 2 objects starts from index 2:\n");
	dll_deln(list, 2, 2);
	dll_print(list, print_object);

	dll_free(&list2);
	dll_free(&list);

	clock_gettime(CLOCK_REALTIME, &end);
	double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
	fprintf(stderr, "\n --- time: %lf sec. ---\n\n", time_spent);

	printf("\t!!! Test dll_assert for free list !!!\n");
	printf("\ndll_assert_soft:\n");
	dll_assert_soft(dll_getdata(NULL));
	printf("\ndll_assert:\n");
	dll_assert(dll_new(NULL, 0, 0, NULL));
}
