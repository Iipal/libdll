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
 * for dll_init available only: DLL_BIT_DFLT and DLL_BIT_EIGN
 * for dll_new/dll_push* the same and all other
 */
typedef enum
# ifdef __clang__
	__attribute__((__enum_extensibility__(closed), __flag_enum__))
# endif
{
/** \brief Default behavior */
	DLL_BIT_DFLT = 0x0,

/** \brief Ignore errors such as NULL pointer exceptions and if handlers return a negative value */
	DLL_BIT_EIGN = 0x1,

/** \brief Duplicating a 'void*' and free it when deleting
 * \warning { IT'S MAY NOT BE SAFE TO USE IT. Allocate memory on your own }
 */
	DLL_BIT_DUP = 0x2,

/** \brief Freeing 'void*' memory with free(3) in the object */
	DLL_BIT_FREE = 0x4,

} __attribute__((packed)) dll_bits_t;

#endif /* LIBDLL_BITS_H */
