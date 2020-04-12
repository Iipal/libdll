/**
 * Copyright (C) 2020 Taras Maliukh
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef LIBDLL_BITS_H
# define LIBDLL_BITS_H

# ifndef LIBDLL_H
#  error "include only libdll.h"
# endif

# define __dll_is_bit(bits, bit) (((bits) & (bit)) == (bit))

/**
 * All macroses specification for only 1 object in a linked list has a prefix:
 * DLL_BIT_* (dll_init, dll_new, dll_push*)
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
