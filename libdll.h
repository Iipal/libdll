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

#ifndef LIBDLL_H
# define LIBDLL_H

# define _GNU_SOURCE

# include <stdbool.h>
# include <limits.h>

# include "libdll_bits.h"
# include "libdll_assert.h"
# include "libdll_structs.h"
# include "libdll_errno.h"

/**
 * Allocate new list
 * For specifying behavior for list - set \param bits using DLL_BIT*
 */
static inline dll_t	*dll_init(dll_bits_t bits);

/**
 * Create new list object
 * For specifying behavior for this object - set \param bits using DLL_BIT*
 * !! Strongly recomended to setup \param fn_free
 *  if you allocating memory inside \param data
 */
static inline dll_obj_t	*dll_new(void *restrict data,
		size_t size,
		dll_bits_t bits,
		dll_obj_free_fn_t fn_free);

/**
 * Creating a new list object at the front of list from given parameters
 * !! Strongly recomended to setup \param fn_free if you allocate memory inside \param data
 */
static inline dll_obj_t	*dll_pushfront(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		dll_obj_free_fn_t fn_free);
/**
 * Add a \param obj object to \param dll list
 */
static inline dll_obj_t	*dll_pushfrontobj(dll_t *restrict dll,
		dll_obj_t *restrict obj);

/**
 * Creating a new list object at the end of list from given parameters
 * !! Strongly recomended to setup \param fn_free
 *  if you allocating memory inside \param data
 */
static inline dll_obj_t	*dll_pushback(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		dll_obj_free_fn_t fn_free);

/**
 * Add a \param obj object to \param dll list
 */
static inline dll_obj_t	*dll_pushbackobj(dll_t *restrict dll,
		dll_obj_t *restrict obj);

/**
 * Delete first object in list
 */
static inline bool	dll_popfront(dll_t *restrict dll);
/**
 * Delete last object in list
 */
static inline bool	dll_popback(dll_t *restrict dll);

/**
 * Get a count of objects in \param dll list
 */
static inline size_t	dll_getsize(const dll_t *restrict dll);
/**
 * Get a current first objectin \param dll list
 */
static inline dll_obj_t	*dll_gethead(const dll_t *restrict dll);
/**
 * Get a current last object in \param dll list
 */
static inline dll_obj_t	*dll_getlast(const dll_t *restrict dll);
/**
 * Check is \param dll list empty
 */
static inline bool	dll_empty(const dll_t *restrict dll);

/**
 * Get a pointer to data in object
 */
static inline void	*dll_getdata(const dll_obj_t *restrict obj);
/**
 * Get a size of data in object
 */
static inline size_t	dll_getdatasize(const dll_obj_t *restrict obj);
/**
 * Get a previous object
 */
static inline dll_obj_t	*dll_getprev(const dll_obj_t *restrict obj);
/**
 * Get a next object
 */
static inline dll_obj_t	*dll_getnext(const dll_obj_t *restrict obj);
/**
 * Get a index of object in list
 */
static inline size_t	dll_getid(dll_t *restrict dll,
		const dll_obj_t *restrict obj);

/**
 * Apply \param fn_iter for at most \param n list objects data starts from \param start index
 */
static inline bool	dll_foreachn(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_iter,
		size_t start,
		size_t n);
/**
 * just an "alias" for dll_foreachn(dll, fn_iter, 1, dll_getsize(dll));
 */
static inline bool	dll_foreach(dll_t *restrict dll, dll_obj_handler_fn_t fn_iter);

static inline dll_obj_t	*dll_findn(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_match,
		void *restrict ptr,
		size_t start,
		size_t n);
/**
 * just an "alias" for dll_find(dll, fn_match, 1, dll_getsize(dll));
 */
static inline dll_obj_t	*dll_find(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_match,
		void *restrict ptr);

/**
 * Find object by data(key) from start
 *
 * \return a pointer to a matched object if \param fn_search returns a zero.
 * Otherwise, if the desired object doesn't exist or \param fn_search handler returns a negative value - NULL will be returned.
 * \param ptr just going to the second argument of your handler if you need additional data to compare data inside the object
*/
static inline dll_obj_t	*dll_findkey(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search,
		void *restrict ptr);
// The same as dll_findkey but starts seraching from end
static inline dll_obj_t	*dll_findkeyr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search,
		void *restrict ptr);

/**
 * Find object by index from start(indexing starts from 1)
 */
static inline dll_obj_t	*dll_findid(dll_t *restrict dll, size_t index);
// The same as dll_findid but starts from end
static inline dll_obj_t	*dll_findidr(dll_t *restrict dll, size_t index);

/**
 * Print \param obj object data via \param fn_print handler
 */
static inline ssize_t	dll_printone(dll_obj_t *restrict obj,
		dll_obj_handler_fn_t fn_print);
/**
 * Print all objects via \param fn_print from begin
 */
static inline bool	dll_print(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_print);
/**
 * Print all objects via \param fn_print from end
 */
static inline bool	dll_printr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_print);

/**
 * The same as dll_print but printing at most \param n objects starts
 *  from index \param start via \param fn_print to end of list
 */
static inline size_t	dll_printn(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_print,
		size_t start,
		size_t n);
/**
 * The same as dll_print but printing at most \param n objects starts
 *  from index \param start via \param fn_print to start of list
 */
static inline size_t	dll_printnr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_print,
		size_t start,
		size_t n);

/**
 * Swap all data in \param a and \param b objects
 */
static inline bool	dll_swap(dll_obj_t *restrict a, dll_obj_t *restrict b);
/**
 * Swap all objects in \param a and \param b lists
 */
static inline bool	dll_swapdll(dll_t *restrict a, dll_t *restrict b);

/**
 * Reverse all objects in \param dll
 */
static inline dll_t	*dll_reverse(dll_t *restrict dll);

/**
 * Add to the of list \param dst all at most \param n objects starts fron index \param start what matches vid \param fn_match
 */
static inline dll_t	*dll_appenddll(dll_t *restrict src,
		dll_t *restrict dst,
		dll_obj_handler_fn_t fn_match,
		void *restrict ptr,
		size_t start,
		size_t n);
/**
 * Creating a duplicate list with at most \param n objects from \param dll list, starting from \param start indexed object
 */
static inline dll_t	*dll_dup(dll_t *restrict dll, size_t start, size_t n);
/**
 * The same as dll_dup but duplicate only objects for which \param fn_match return a zero.
 */
static inline dll_t	*dll_dupkey(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_match,
		void *restrict ptr,
		size_t start,
		size_t n);

/**
 * Removing all links to a given object in the list and return it
 * Links to next and previous list object will be saved in the return object
 */
static inline dll_obj_t	*dll_unlink(dll_t *restrict dll,
		dll_obj_t *restrict obj);

/**
 * Delete links to given object and free it via dll_freeobj
 */
static inline bool	dll_del(dll_t *restrict dll, dll_obj_t *restrict obj);
/**
 * Delete links and free it to at most \param n object in \param dll list, starting from \param start indexed object
 */
static inline size_t	dll_deln(dll_t *restrict dll, size_t start, size_t n);

/**
 * Delete object by data(key) from start via \param fn_search_del and dll_findkey
 * \param ptr just going to the second argument of your handler if you need additional data to compare data inside the object
 */
static inline bool	dll_delkey(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search_del,
		void *restrict ptr);
// The same as dll_delkey but using dll_findkeyr instead of dll_findkey
static inline bool	dll_delkeyr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search_del,
		void *restrict ptr);

/**
 * Delete object by index from start via dll_findid
 */
static inline bool	dll_delid(dll_t *restrict dll, size_t index);
/**
 * Delete object by index from end via dll_findidr
 */
static inline bool	dll_delidr(dll_t *restrict dll, size_t index);

/**
 * Free all objects and list
 */
static inline bool	dll_free(dll_t *restrict *restrict dll);
/**
 * Free all data and given object
 */
static inline bool	dll_freeobj(dll_obj_t *restrict *restrict obj);
/**
 * Free only all data inside object
 * This function execute \param fn_free if its was setted-up in dll_new\dll_push*
 */
static inline bool	dll_freeobjdata(dll_obj_t *restrict *restrict obj);

/**
 * Some basic handlers:
 */
/**
 * Return match only if data pointer \param obj inside object identical to pointer passed in \param ptr
 */
static ssize_t	dll_fnptr_ptrobj(void *restrict obj, void *restrict ptr, size_t idx);
/**
 * Return match only if \param idx inside object identical to index pointer passed in \param ptr
 */
static ssize_t	dll_fnptr_ptridx(void *restrict obj, void *restrict ptr, size_t idx);
/**
 * Return match for every object
 */
static ssize_t	dll_fnptr_any(void *restrict obj, void *restrict ptr, size_t idx);

# include "libdll_sources.h"

#endif /* LIBDLL_H */
