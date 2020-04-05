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
 * Default behavior
 */
# define DLL_BIT_DFLT 0
/**
 * Ignore errors such as NULL pointer exceptions
 *  and if handlers return a negative value
 */
# define DLL_BIT_EIGN 1

// Maximum valid bits mask value for dll_init
# define __DLL_MAX_VALID_LIST_MASK 1

/**
 * Duplicating a 'void*' what passed in
 *  dll_new\dll_pushfront\dll_pushback and free it when deleting
 *
 * \warning { IT'S MAY NOT BE SAFE TO USE IT. Allocate memory on your own }
 */
# define DLL_BIT_DUP 2
/**
 * Freeing memory with free(3) what you put in the object
 */
# define DLL_BIT_FREE 4

// Maximum valid bits mask value for dll_new/dll_push*
# define __DLL_MAX_VALID_OBJ_MASK 7


#endif /* LIBDLL_BITS_H */
