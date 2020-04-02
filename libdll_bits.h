#ifndef LIBDLL_BITS_H
# define LIBDLL_BITS_H

# ifndef LIBDLL_H
#  error "include only libdll.h"
# endif

# define __dll_is_bit(bits, bit) (((bits) & (bit)) == (bit))

/**
 * All macroses specified for all objects in linked list has prefix:
 * DLL_GBIT_* (use only with dll_init)
 * All macroses specified for only 1 object in linked list has prefix:
 * DLL_BIT_* (use only with dll_new, dll_push*)
 */
typedef unsigned int dll_bits_t;

/**
 * Default behavior for object
 */
# define DLL_BIT_DFLT 0
/**
 * For all functions which call an object handler - if the handler returns a negative value it's will be ignored
 */
# define DLL_BIT_EIGN 1
/**
 * Do not print any error-message for object
 */
# define DLL_BIT_EQUIET 2
/**
 * Duplicating a 'void*' what passed in dll_new\dll_pushfront\dll_pushback and free it when deleting
 * \warning { ITS NOT SAFE TO USE IT, allocate memory by your own }
 */
# define DLL_BIT_DUP 4
/**
 * Free memory with free(3) what you put in object void *data
 */
# define DLL_BIT_FREE 8

/**
 * Default behavior for list
 */
# define DLL_GBIT_DFLT 0
/**
 * Do not print output order and objects count in linked list in dll_print* function
 */
# define DLL_GBIT_QUIET 1

#endif /* LIBDLL_BITS_H */
