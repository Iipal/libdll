# libdll

Header-only library for work with Double Linked Lists(or Bidirectional Lists)

This is a simplest libdll implementation based on C++11 `std::list` methods and **without** errno codes, messages also without asserts.
If you need all of this, check [this](https://github.com/Iipal/libdll/tree/empty-objs) branch

**Thanks and have fun =)**

## Example
Simplest example with a few comments
```c
#define LIBDLL_LOGGER // INCLUDE BEFORE "libdll.h" !!

#include "libdll.h"

char    *str = "just a str";
size_t  str_len = sizeof(str);

void    print_list_str_data(void *restrict obj_data) {
    const char *restrict    obj_str_data = (const char *restrict)obj_data;

    printf("%s\n", obj_str_data);
}

void    print_list_number_data(void *restrict obj_data) {
    const size_t *restrict  obj_number_data = (const size_t *restrict)obj_data;

    printf("%zu\n", *obj_number_data);
}

int     main(void) {
    /**
     * REQUIRED(!!!) before first call of any dll_* method in each file for creation of log-file, otherwise nothing will works.
     */
    dll_init_logger;

    dll_t   *list = dll_init();

    /**
     * equals to:
     * dll_push_back(list, dll_new_obj(str, str_len, LIBDLL_DESTRUCTOR_NULL));
     */
    dll_emplace_back(list, str, str_len, LIBDLL_DESTRUCTOR_NULL);

    dll_foreach(list, print_list_str_data);

    // be ceraful with this method, it's erases all objects data in list
    dll_clear(list);

    /*
     * Also by passing any data in the list object it's will not be duplicated.
     * If you need this behavior - made it by yourself,
     * by following this steps for all the data before it will be passed to the list:
     *  1. allocate memory and copy to it any data.
     *  2. pass it to dll_emplace\dll_push with specifying a desctructor function(
     *      or use a LIBDLL_DESTRUCTOR_DEFAULT if you need to apply just
     *      a free(3) to allocated data).
     */
    size_t          i;
    const size_t    iterator_size = sizeof(i);

    for (i = 0; 10 > i; ++i) {
        size_t  *iterator_mem_chunk = calloc(1, iterator_size);
        memcpy(iterator_mem_chunk, &i, iterator_size);

        dll_emplace_back(list, iterator_mem_chunk, iterator_size, LIBDLL_DESTRUCTOR_DEFAULT);
    }

    dll_foreach(list, print_list_number_data);

    // For correct work without memory leaks - dll_free is required.
    dll_free(&list);
}
```

## By the way: everything has it's own, well-written, documentation.
![](https://i.ibb.co/kXBDNZm/Screenshot-2021-02-19-213753.png)
> theme on the screenshot: Twilight Pro for VSCode..
