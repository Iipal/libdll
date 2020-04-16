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
 *
 * \return NULL if:
 *  - memory allocation failed.
 *  - if DLL_BIT_EIGN is not specified:
 *  -- passed invalid \param bits value
 *
 *  Otherwise, pointer to allocated memory for new list
 */
static inline dll_t	*dll_init(dll_bits_t bits);

/**
 * Create new list object
 * For specifying behavior for this object - set \param bits using DLL_BIT*
 * !! Strongly recomended to setup \param fn_free
 *  if you allocating memory inside \param data
 *
 * \return NULL if:
 *  - memory allocation failed.
 *  - if DLL_BIT_DUP specified:
 *  -- failed to allocate memory for duplicate \param data
 *  - if DLL_BIT_EIGN is not specified:
 *  -- passed invalid \param bits value
 *  -- \param data is NULL
 *  -- \param size is 0
 *
 *  Otherwise, pointer to allocated memory for new list object
 */
static inline dll_obj_t	*dll_new(void *restrict data,
		size_t size,
		dll_bits_t bits,
		dll_obj_free_fn_t fn_free);

/**
 * Creating a new list object at the front of list from given parameters
 * !! Strongly recomended to setup \param fn_free if you allocate memory inside \param data
 * \return NULL if dll_new can't allocate memory or some errors occurred.
 * Otherwise, a pointer to a new created object.
 */
static inline dll_obj_t	*dll_pushfront(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		dll_obj_free_fn_t fn_free);
/**
 * Add a \param dll_obj object to \param dll list
 * \return NULL if:
 *  - \param dll or \param dll_obj is NULL
 *
 * Otherwise, return a \param dll_obj pointer
 */
static inline dll_obj_t	*dll_pushfrontobj(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj);

/**
 * Creating a new list object at the end of list from given parameters
 * !! Strongly recomended to setup \param fn_free
 *  if you allocating memory inside \param data
 *
 * \return NULL if dll_new can't allocate memory or some error occurred.
 * Otherwise, a pointer to a new created object.
 */
static inline dll_obj_t	*dll_pushback(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		dll_obj_free_fn_t fn_free);

/**
 * Add a \param dll_obj object to \param dll list
 * \return NULL if:
 *  - \param dll or \param dll_obj is NULL
 *
 * Otherwise, return a \param dll_obj pointer
 */
static inline dll_obj_t	*dll_pushbackobj(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj);

/**
 * Delete first object in list
 *
 * \return false if:
 *  - \param dll is NULL or list \param dll hasn't any objects
 */
static inline bool	dll_popfront(dll_t *restrict dll);
/**
 * Delete last object in list
 *
 * \return false if:
 *  - \param dll is NULL or list \param dll hasn't any objects
 */
static inline bool	dll_popback(dll_t *restrict dll);

/**
 * Get a count of objects in list
 * \return 0 if:
 *  - \param dll is NULL or list \param dll hasn't any objects
 */
static inline size_t	dll_getsize(const dll_t *restrict dll);
/**
 * Get a current first object in list
 * \return NULL if:
 *  - \param dll is NULL or list \param dll hasn't any objects
 */
static inline dll_obj_t	*dll_gethead(const dll_t *restrict dll);
/**
 * Get a current last object in list
 * \return NULL if:
 *  - \param dll is NULL or list \param dll hasn't any objects
 */
static inline dll_obj_t	*dll_getlast(const dll_t *restrict dll);

/**
 * Get a pointer to data in object
 * \return NULL if:
 *  - \param dll_obj is NULL or data inside object is pointed to NULL
 */
static inline void	*dll_getdata(const dll_obj_t *restrict dll_obj);
/**
 * Get a size of data in object
 * \return 0 if:
 *  - \param dll_obj is NULL or data size inside object is 0
 */
static inline size_t	dll_getdatasize(const dll_obj_t *restrict dll_obj);
/**
 * Get a previous object
 * \return NULL if:
 *  - \param dll_obj is NULL
 */
static inline dll_obj_t	*dll_getprev(const dll_obj_t *restrict dll_obj);
/**
 * Get a next object
 * \return NULL if:
 *  - \param dll_obj is NULL
 */
static inline dll_obj_t	*dll_getnext(const dll_obj_t *restrict dll_obj);
/**
 * Get a index of object in list
 * \return __SIZE_MAX__ if:
 *  - \param dll or \param dll_obj is NULL
 *  - if a pointer to \param dll_obj is not pointing to any object in \param dll list
 *  - if DLL_BIT_EIGN is not specified:
 *  -- list \param dll hasn't any objects
 *
 */
static inline size_t	dll_getid(const dll_t *restrict dll,
		const dll_obj_t *restrict dll_obj);

/**
 * Apply \param fn_iter for at most \param n list objects data starts from \param start index
 *
 * \return false if:
 *  - \param dll is NULL
 *  - if DLL_BIT_EIGN is not specified:
 *  -- \param fn_iter is NULL
 *  -- list \param dll hasn't any objects
 *  -- object with \param index doesn't exist in \param dll list
 *  -- data inside at least 1 object is pointed to NULL
 *  -- \param fn_iter returns a negative value
 */
static inline bool	dll_foreachn(dll_t *restrict dll,
		dll_obj_iter_fn_t fn_iter,
		size_t start,
		size_t n);
/**
 * just an "alias" for dll_foreachn(dll, fn_iter, 1, dll_getsize(dll));
 */
static inline bool	dll_foreach(dll_t *restrict dll, dll_obj_iter_fn_t fn_iter);

/**
 * Find object by data(key) from start
 *
 * \return a pointer to a matched object if \param fn_search returns a zero.
 * Otherwise, if the desired object doesn't exist or \param fn_search handler returns a negative value - NULL will be returned.
 * \param any_data just going to the second argument of your handler if you need additional data to compare data inside the object
 *
 * Also NULL returns in cases when:
 * - \param dll is NULL
 * - if DLL_BIT_EIGN is not specified:
 * -- \param fn_search is NULL
 * -- list \param dll hasn't any objects
 * -- data inside at least 1 object is pointed to NULL
*/
static inline dll_obj_t	*dll_findkey(const dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search,
		void *restrict any_data);
// The same as dll_findkey but starts seraching from end
static inline dll_obj_t	*dll_findkeyr(const dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search,
		void *restrict any_data);

/**
 * Find object by index from start(indexing starts from 1)
 *
 * \return NULL if:
 * - \param dll is NULL
 * - if DLL_BIT_EIGN is not specified:
 * -- list \param dll hasn't any objects
 * -- object with \param index doesn't exist in \param dll list
 */
static inline dll_obj_t	*dll_findid(const dll_t *restrict dll, size_t index);
// The same as dll_findid but starts from end
static inline dll_obj_t	*dll_findidr(const dll_t *restrict dll, size_t index);

/**
 * Creating a duplicate list with at most \param n objects from \param dll list, starting from \param start indexed object
 *
 * \return NULL if:
 * - \param dll is NULL
 * - failed to allocate memory for new list
 * - if DLL_BIT_EIGN is not specified:
 * -- list \param dll hasn't any objects
 * -- object with index \param start doesn't exist in \param dll list
 * -- failed to duplicate at least 1 object
 */
static inline dll_t	*dll_dup(const dll_t *restrict dll, size_t start, size_t n);
/**
 * The same as dll_dup but duplicate only objects for which \param fn_match return a zero.
 */
static inline dll_t	*dll_dupkey(const dll_t *restrict dll,
		dll_obj_handler_fn_t fn_match,
		void *restrict any_data,
		size_t start,
		size_t n);

/**
 * Print \param dll_obj object data via \param fn_print_idx handler
 * \param idx for external-use in most cases is useless,
 *  it's just go to second argument of \param fn_print_idx
 *
 * \return -1 if:
 * - \param dll_obj or \param fn_print_idx is NULL
 * - if DLL_BIT_EIGN is not specified:
 * -- \param dll_obj data pointed to NULL
 */
static inline int	dll_printone(const dll_obj_t *restrict dll_obj,
		dll_obj_handleridx_fn_t fn_print_idx,
		size_t idx);
/**
 * Print all objects via \param fn_print_idx from begin
 *
 * \return false if:
 * - \param dll is NULL
 * - if DLL_BIT_EIGN is not specified:
 * -- \param fn_print_idx is NULL
 * -- list \param dll hasn't any objects
 * -- data inside at least 1 object is pointed to NULL
 * -- \param fn_print_idx handler returns a negative value
 */
static inline bool	dll_print(const dll_t *restrict dll,
		dll_obj_handleridx_fn_t fn_print_idx);
// The same as dll_print but starts printing from end
static inline bool	dll_printr(const dll_t *restrict dll,
		dll_obj_handleridx_fn_t fn_print_idx);

/**
 * The same as dll_print but printing at most \param n objects starts
 *  from index \param start via \param fn_print_idx to end of list
 *
 * \return a count of printed objects
 */
static inline size_t	dll_printn(const dll_t *restrict dll,
		dll_obj_handleridx_fn_t fn_print_idx,
		size_t start,
		size_t n);
/**
 * The same as dll_print but printing at most \param n objects starts
 *  from index \param start via \param fn_print_idx to start of list
 *
 * \return a count of printed objects
 */
static inline size_t	dll_printnr(const dll_t *restrict dll,
		dll_obj_handleridx_fn_t fn_print_idx,
		size_t start,
		size_t n);
/**
 * Removing all links to a given object in the list and return it
 * Links to next and previous list object will be saved in the return object
 *
 * \return NULL if:
 * - \param dll or \param dll_obj is NULL
 */
static inline dll_obj_t	*dll_unlink(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj);

/**
 * Delete links to given object and free it via dll_freeobj
 *
 * \return false if:
 * - some errors occurred when calling dll_unlink or dll_freeobj
 */
static inline bool	dll_del(dll_t *restrict dll, dll_obj_t *restrict dll_obj);
/**
 * Delete links and free it to at most \param n object in \param dll list, starting from \param start indexed object
 *
 * \return count of deleted objects otherwise 0 if:
 * - \param dll is NULL
 * - if DLL_BIT_EIGN is not specified:
 * -- list \param dll hasn't any objects
 * -- object with index \param start doesn't exist in \param dll list
 * -- some errors occured when calling dll_del for at least 1 object
 */
static inline size_t	dll_deln(dll_t *restrict dll, size_t start, size_t n);

/**
 * Delete object by data(key) from start via \param fn_search_del and dll_findkey
 * \param any_data just going to the second argument of your handler if you need additional data to compare data inside the object
 * \return false if:
 * - some errors occurred when calling dll_findkey or dll_del
 */
static inline bool	dll_delkey(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search_del,
		void *restrict any_data);
// The same as dll_delkey but using dll_findkeyr instead of dll_findkey
static inline bool	dll_delkeyr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search_del,
		void *restrict any_data);

/**
 * Delete object by index via dll_findid
 *
 * \return false if:
 * - some errors occurred when calling dll_findid or dll_del
 */
static inline bool	dll_delid(dll_t *restrict dll, size_t index);
// The same as dll_delid but using dll_findidr instead of dll_findid
static inline bool	dll_delidr(dll_t *restrict dll, size_t index);

/**
 * Free all objects and list
 *
 * \return false if:
 * - \param dll is NULL or list \param dll hasn't any objects
 */
static inline bool	dll_free(dll_t *restrict *restrict dll);
/**
 * Free given object
 * This function execute \param fn_free if its was setted-up in dll_new\dll_push*
 *
 * \return false if:
 * - \param dll_obj is NULL
 */
static inline bool	dll_freeobj(dll_obj_t *restrict *restrict dll_obj);

# include "libdll_sources.h"

# undef __dll_is_bit

#endif /* LIBDLL_H */
