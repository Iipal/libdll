typedef struct s_int_test {
    int a;
} t_itest;

#include "libdll.h"

void    itest_printer(void *restrict obj_data) {
    const t_itest *restrict itest = (const t_itest *restrict)obj_data;
    printf(" ─ %d\n", itest->a);
}

void    str_printer(void *restrict obj_data) {
    const char *restrict    str = (const char *restrict)obj_data;
    printf(" ─ %s\n", str);
}

ssize_t ascii_sorter(void *restrict obj1_data, void *restrict obj2_data) {
    return strcmp((const char*)obj1_data, (const char*)obj2_data);
}

int main(void) {
    dll_t *restrict dll = dll_init();

    {
        dll_push_back(dll, dll_new_obj("back", sizeof("back"), NULL));
        /* the same as dll_push_back call above but with allocating memory and using LIBDLL_FN_FREE_CLEARANCE. */
        dll_emplace_front(dll,
            memcpy(calloc(1, sizeof("front")), "front", sizeof("front") - 1),
            sizeof("front"),
            LIBDLL_FN_FREE_CLEARANCE);

        printf("\n ----------\nlist size: %ld\n", dll_size(dll));
        dll_foreach(dll, str_printer);
    }

    {
        dll_insert(dll, dll_new_obj("middle", sizeof("middle"), NULL), dll_size(dll));
        /* the same as dll_insert call above */
        dll_emplace(dll, "last", sizeof("last"), NULL, dll_size(dll));
        dll_emplace(dll, "after first", sizeof("after first"), NULL, 1);

        printf("\n ----------\nafter insertion list size: %ld\n", dll_size(dll));
        dll_foreach(dll, str_printer);
    }

    {
        dll_sort(dll, ascii_sorter);

        printf("\n ----------\nsorted elements: %ld\n", dll_size(dll));
        dll_foreach(dll, str_printer);
    }

    {
        dll_t   *dll2 = dll_init();

        dll_emplace_back(dll2, "dll2_test1", sizeof("dll2_test1"), NULL);
        dll_emplace_back(dll2, "dll2_test2", sizeof("dll2_test2"), NULL);

        printf("\n ----------\nbefore dll_merge\n dll1:\n");
        dll_foreach(dll, str_printer);
        printf(" dll2:\n");
        dll_foreach(dll2, str_printer);

        dll_merge(dll, dll2, NULL);

        printf("\nafter dll_merge\n dll1:\n");
        dll_foreach(dll, str_printer);
        printf(" dll2:\n");
        dll_foreach(dll2, str_printer);

        dll_free(&dll2);
    }

    {
        dll_erase(dll, 1, 2);

        printf("after erasing elements in range[1:2]: %ld\n", dll_size(dll));
        dll_foreach(dll, str_printer);
    }

    dll_free(&dll);

    {
        dll_t *restrict dll2 = dll_init();
        dll = dll_init();

        dll_emplace_back(dll, &(t_itest){ 1 }, sizeof(t_itest), NULL);
        dll_emplace_back(dll, &(t_itest){ 2 }, sizeof(t_itest), NULL);
        dll_emplace_back(dll, &(t_itest){ 3 }, sizeof(t_itest), NULL);
        dll_emplace_back(dll, &(t_itest){ 4 }, sizeof(t_itest), NULL);
        dll_emplace_back(dll, &(t_itest){ 5 }, sizeof(t_itest), NULL);

        dll_emplace_back(dll2, &(t_itest){ 10 }, sizeof(t_itest), NULL);
        dll_emplace_back(dll2, &(t_itest){ 20 }, sizeof(t_itest), NULL);
        dll_emplace_back(dll2, &(t_itest){ 30 }, sizeof(t_itest), NULL);
        dll_emplace_back(dll2, &(t_itest){ 40 }, sizeof(t_itest), NULL);
        dll_emplace_back(dll2, &(t_itest){ 50 }, sizeof(t_itest), NULL);

        printf("\n ----------\n\ndll_splice tests:\n before:\n  dll1(%ld):\n", dll_size(dll));
        dll_foreach(dll, itest_printer);
        printf("  dll2(%ld):\n", dll_size(dll2));
        dll_foreach(dll2, itest_printer);

        dll_splice(dll2, dll, 0, 2, 10);

        printf("\n after:\n  dll1(%ld):\n", dll_size(dll));
        dll_foreach(dll, itest_printer);
        printf("  dll2(%ld):\n", dll_size(dll2));
        dll_foreach(dll2, itest_printer);

        dll_splice(dll, dll2, dll_size(dll), 0, 2);

        printf("\n return back all values with dll_splice:\n  dll1(%ld):\n", dll_size(dll));
        dll_foreach(dll, itest_printer);
        printf("  dll2(%ld):\n", dll_size(dll2));
        dll_foreach(dll2, itest_printer);

        dll_free(&dll);
        dll_free(&dll2);
    }
}
