# libdll

Header-only library for work with Double Linked Lists(or Bidirectional Lists)

This is a simplest libdll implementation based on C++11 `std::list` methods and **without** errno codes, messages also without asserts.

**Thanks and have fun =)**

## Example
Simplest example with a few comments
```c
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
    dll_t   *list = dll_init();

    dll_emplace_back(list, str, str_len, LIBDLL_FN_FREE_NULL);
    // equals to:
    // dll_push_back(list, dll_new_obj(str, str_len, LIBDLL_FN_FREE_NULL));

    dll_foreach(list, print_list_str_data);

    // be ceraful with this method, it's erases all objects data in list
    dll_clear(list);

    /*
     * Also by passing any data in to list it's will not be duplicated.
     * If you need this behavior - made it by yourself,
     * by following this steps for all the data before it will be passed to the list:
     *  1. allocating memory amd copying to it any data.
     *  2. passing it to dll_emplace\dll_push with specifying an fn_free function(
     *      or use a LIBDLL_FN_FREE_CLEARANCE if you need just free(3) apply to allocated data
     *     ).
     */
    size_t i;
    for (i = 0; 10 > i; ++i) {
        dll_emplace_back(list, &i, sizeof(i), LIBDLL_FN_FREE_NULL);
    }
    dll_foreach(list, print_list_number_data);

    // For correct work without memory leaks - dll_free is required.
    dll_free(&list);
}

```

## By the way: everything has it's own, well-written, documentation.
![](https://i.ibb.co/kXBDNZm/Screenshot-2021-02-19-213753.png)
> theme on the screenshot: Twilight Pro for VSCode..
