#ifndef LIBDLL_H
# define LIBDLL_H

# define _GNU_SOURCE

# include <stdbool.h>

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
 * !! Strongly recomended to setup \param fn_del
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
		size_t size, dll_bits_t bits, f_dll_obj_data_del fn_del);

/**
 * Creating a new list object at the front of list from given parameters
 * !! Strongly recomended to setup \param fn_del if you allocate memory inside \param data
 * \return NULL if dll_new can't allocate memory or some errors occurred.
 * Otherwise, a pointer to a new created object.
 */
static inline dll_obj_t	*dll_pushfront(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		f_dll_obj_data_del fn_del);
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
 * Delete first object in list
 *
 * \return false if:
 *  - \param dll is NULL or list \param dll hasn't any objects
 */
static inline bool	dll_popfront(dll_t *restrict dll);

/**
 * Creating a new list object at the end of list from given parameters
 * !! Strongly recomended to setup \param fn_del
 *  if you allocating memory inside \param data
 *
 * \return NULL if dll_new can't allocate memory or some error occurred.
 * Otherwise, a pointer to a new created object.
 */
static inline dll_obj_t	*dll_pushback(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		f_dll_obj_data_del fn_del);
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
 * \return NULL if:
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
 * Find object by data(key) from start
 *
 * \return a pointer to a matched object if \param fn_search returns a zero.
 * Otherwise, if the desired object doesn't exist or \param fn_search handler returns a negative value - NULL will be returned.
 *
 * Also NULL returns in cases when:
 * - \param dll is NULL
 * - if DLL_BIT_EIGN is not specified:
 * -- \param fn_search is NULL
 * -- list \param dll hasn't any objects
 * -- data inside at least 1 object is pointed to NULL
*/
static inline dll_obj_t	*dll_findkey(const dll_t *restrict dll,
		f_dll_obj_handler fn_search);
// The same as dll_findkey but starts seraching from end
static inline dll_obj_t	*dll_findkeyr(const dll_t *restrict dll,
		f_dll_obj_handler fn_search);

/**
 * Find object by index from start(indexing starts from 1)
 *
 * \return NULL if:
 * - \param dll is NULL
 * - if DLL_BIT_EIGN is not specified:
 * -- list \param dll hasn't any objects
 * -- object with \param index id \param dll list doesn't exist
 */
static inline dll_obj_t	*dll_findid(const dll_t *restrict dll, size_t index);
// The same as dll_findid but starts from end
static inline dll_obj_t	*dll_findidr(const dll_t *restrict dll, size_t index);

/**
 * Print object data via \param fn_print handler
 *
 * \return -1 if:
 * - \param dll_obj or \param fn_print is NULL
 * - if DLL_BIT_EIGN is not specified:
 * -- \param dll_obj data pointed to NULL
 */
static inline int	dll_printone(const dll_obj_t *restrict dll_obj,
		f_dll_obj_handler fn_print);
/**
 * Print all objects via \param fn_print from begin
 *
 * \return false if:
 * - \param dll is NULL
 * - if DLL_Bit_EIGN is not specified:
 * -- \param fn_print is NULL
 * -- list \param dll hasn't any objects
 * -- data inside at least 1 object is pointed to NULL
 * -- \param fn_print handler returns a negative value
 */
static inline bool	dll_print(const dll_t *restrict dll,
		f_dll_obj_handler fn_print);
// The same as dll_print but starts printing from end
static inline bool	dll_printr(const dll_t *restrict dll,
		f_dll_obj_handler fn_print);

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
 * Delete object by data(key) from start
 * via \param fn_search_del and dll_findkey
 *
 * \return false if:
 * - some errors occurred when calling dll_findkey or dll_del
 */
static inline bool	dll_delkey(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del);
// The same as dll_delkey but using dll_findkeyr instead of dll_findkey
static inline bool	dll_delkeyr(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del);

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
static inline bool	dll_free(dll_t *restrict dll);
/**
 * Free given object
 * This function execute \param fn_del if its was setted-up in dll_new\dll_push*
 *
 * \return false if:
 * - \param dll_obj is NULL
 */
static inline bool	dll_freeobj(dll_obj_t *restrict dll_obj);

# include "libdll_sources.h"

# undef __dll_is_bit

#endif /* LIBDLL_H */
