# libdll
Header-only library for work with Double Linked Lists(or Bidirectional Lists)

# Explanation and examples:
another similar [example.c](https://github.com/Iipal/libdll/blob/master/example.c)

With this library you can do:
### Create new Double Linked List:
```c
/**
 * It's not required to cheks NULL-pointer exception
 *  for any memory allocations what provide libdll
 *  because it checks internally via assert(3)-macros
 *  and aborting a program correspondingly
 *
 * use DLL_GBIT* constants to setup bits mask
 */
dll_t   *my_list = dll_init(DLL_GBIT_DEFAULT);
```

### Create new list object:
```c
// using struct just for example, you can use whatever you want
struct s_test {
    char *str;
    size_t len;
} my_test = { strdup("data_str"), 9 };

void    free_test(void *restrict data) {
    struct s_test *restrict tptr = data;
    free(tptr->str);
    free(tptr); // Freeing pointer because of DLL_BIT_DUP
}

dll_obj_t   *my_obj = dll_new(&my_test, sizeof(my_test),
    DLL_BIT_DUP /* use DLL_BIT* constants to setup bits mask */,
    free_test /* Setup this argument if you
      are allocating memory inside first argument */);
```

### Create\Add new object to head\end of list:
```c
/**
 * All of these functions return a pointer to a new object
 */
dll_pushfrontobj(my_list, my_obj);
dll_pushbackobj(my_list, my_obj);

dll_pushfront(my_list, &my_test, sizeof(my_test), DLL_BIT_DUP, free_test);
dll_pushback(my_list, &my_test, sizeof(my_test), DLL_BIT_DUP, free_test);
```

### Delete last\fisrt(head) object in list:
```c
/**
 * Both of these functions returns a true if deleting
 *  was successfully or list isn't empty yet
 */
dll_popfront(my_list);
dll_popback(my_list);
```

### Get data about a list:
```c
// Get current count of objects in a list
dll_getsize(my_list);
// Get current first object in a list
dll_gethead(my_list);
// Get current last object in a list
dll_getlast(my_list);
```
### Get data about a list object:
```c
// Get a data what you put in dll_new/dll_push* (void* argument)
dll_getdata(my_obj);
// Get pointer to the previous object
dll_getprev(my_obj);
// Get pointer to next object
dll_getnext(my_obj);
```

### Find\Delete list object by specified data via handler function from head\end:
```c
int find_my_obj(const void *restrict data) {
    struct s_test *restrict tptr = data;
    int ret = 0;
    /**
     * if return is zero it means success and object founded
     * negative value - error
     * positive value - ignored
     */

    if (1 <= tptr->len)
        ret = -1;
    else if (10 < tptr->len)
        ret = 1;
    return ret;
}

// dll_findkeyr/dll_delkeyr just starts searcing from end of list
dll_obj_t *my_obj_ptr = dll_findkey(my_list, find_my_obj);
if (my_obj_ptr)
    dll_del(my_list, my_obj_ptr);

// OR in that case find-delete you can use it:
if (dll_delkey(my_list, find_my_obj))
    // matched object founded and deleted;
```

### Find\Delete list object by index from head\end:
```c
/**
 * dll_findidr/dll_delidr just starts searcing from end of list
 *
 * NULL returns only in case if searching index > dll_getsize(my_list)
 */
my_obj_ptr = dll_findid(my_list, 2);
if (my_obj_ptr)
    dll_del(my_list, my_obj_ptr);

// OR in that case find-delete you can use it:
if (dll_delid(my_list, 2))
    // matched object founded and deleted;
```

### Unlink object:
```c
/**
 * It's literally removes all links in the list to given object
 *  but not links in this object to next/prev objects,
 *  so just don't forget to save a pointer to an unlinked object or free it
 */
dll_obj_t *unlinked = dll_unlink(my_obj);
// or
dll_freeobj(dll_unlink(my_obj));
```

### Print all objects data from head\end via handler function:
```c
int print_my_obj(void *restrict data) {
    struct s_test *restrict tptr = data;
    printf("%s(%zu)\n", tptr->str, tptr->len);
    if (1 >= tptr->len)
        return -1;
    return 0;
}

/**
 * If print-handler returns a -1
 *  or any other negative value printing will stop
 *
 * dll_printr - start printing from end of list
 * dll_printone - print info about only given object
 *  and doesn't check handler return value
 */
dll_print(my_list, print_my_obj);
```

### Free memory:
```c
/**
 * Calls a handler, if it's, was setted-up in dll_new/dll_push*
 * or freeing void*data if DLL_BIT_DUP or DLL_BIT_FREE
 *  was setted-up and handler - not
 */
dll_freeobj(unlinked);
// just freeing all list using dll_popfront
dll_free(my_list);
```

### Set-up internal bits mask for list:
  - DLL_GBIT_DFLT: default behavior
  - DLL_GBIT_QUIET: Do not print output order and objects count in dll_print*

### Set-up internal bits mask for each object in list:
  - DLL_BIT_DFLT: default behavior
  - DLL_BIT_QUIET: if object handler returns neg.value - do not print error message(dll_delkey*, dll_print*)
  - DLL_BIT_EIGN: if object handler returns neg.value - do not stop processing list and no erroo message(dll_delkey*, dll_print*)
  - DLL_BIT_DUP: duplicating a 'void*' what passed in dll_new\dll_pushfront\dll_pushback and free it when deleting. ITS NOT SAFE TO USE IT, allocate memory by your own.


__Thanks and have fun =)__
