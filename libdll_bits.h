#ifndef LIBDLL_BITS_H
# define LIBDLL_BITS_H

# ifndef LIBDLL_H
#  error "include only libdll.h"
# endif

# define __dll_is_bit(bits, bit) (((bits) & (bit)) == (bit))

/**
 * All macroses specified for all objects in a linked list has aprefix:
 * DLL_GBIT_* (dll_init only)
 * All macroses specification for only 1 object in a linked list has a prefix:
 * DLL_BIT_* (dll_new, dll_push*)
 */
typedef unsigned int dll_bits_t;

/**
 * Default behavior for object
 */
# define DLL_BIT_DFLT 0
/**
 * Ignore errors such as NULL pointer exceptions
 *  and if handlers return a negative value
 */
# define DLL_BIT_EIGN 1
/**
 * Doesn't Ignore errors as DLL_BIT_EIGN, but just doesn't print error messages
 */
# define DLL_BIT_EQUIET 2
/**
 * Duplicating a 'void*' what passed in
 *  dll_new\dll_pushfront\dll_pushback and free it when deleting
 *
 * \warning { IT'S MAY NOT BE SAFE TO USE IT. Allocate memory on your own }
 */
# define DLL_BIT_DUP 4
/**
 * Freeing memory with free(3) what you put in the object
 */
# define DLL_BIT_FREE 8

/**
 * Default behavior for list
 */
# define DLL_GBIT_DFLT 0
/**
 * Doesn't print error messages about in-correct work with the list,
 *  such as list is empty when you trying to delete\pop object or etc.
 */
# define DLL_GBIT_QUIET 1

#endif /* LIBDLL_BITS_H */
