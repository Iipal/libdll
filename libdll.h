#ifndef LIBDLL_H
# define LIBDLL_H

# include <stdbool.h>
# include "libdll_bits.h"
# include "libdll_structs.h"

/**
 * Allocate new list
 * For specifying behavior for all objects - set \param bits using DLL_GBIT*
 */
static inline dll_t	*dll_init(dll_bits_t bits);

/**
 * Create new object
 * For specifying behavior for this object - set \param bits using DLL_BIT*
 */
static inline dll_obj_t	*dll_new(void *restrict data,
		size_t size, dll_bits_t bits, f_dll_obj_data_del fn_del);

/**
 * Creating a new list object at the front of list from given parameters
 * !! Strongly recomended to setup \param fn_del if you allocate memory inside \param data
 * \return a pointer to new created object
 */
static inline dll_obj_t	*dll_pushfront(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		f_dll_obj_data_del fn_del);
static inline dll_obj_t	*dll_pushfrontobj(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj);

/**
 * Creating a new list object at the end of list from given parameters
 * !! Strongly recomended to setup \param fn_del if you allocate memory inside \param data
 * \return a pointer to new created list object
 */
static inline dll_obj_t	*dll_pushback(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		f_dll_obj_data_del fn_del);
static inline dll_obj_t	*dll_pushbackobj(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj);

// Delete first object in list
static inline bool	dll_popfront(dll_t *restrict dll);
// Delete last object in list
static inline bool	dll_popback(dll_t *restrict dll);

// Get a count of objects in list
static inline size_t	dll_getsize(const dll_t *restrict dll);
// Get a current first object in list
static inline dll_obj_t	*dll_gethead(const dll_t *restrict dll);
// Get a current last object in list
static inline dll_obj_t	*dll_getlast(const dll_t *restrict dll);

// Get a pointer to data in object
static inline void	*dll_getdata(const dll_obj_t *restrict dll_obj);
// Get a size of data in object
static inline size_t	dll_getdatasize(const dll_obj_t *restrict dll_obj);
// Get a previous object
static inline dll_obj_t	*dll_getprev(const dll_obj_t *restrict dll_obj);
// Get a next object
static inline dll_obj_t	*dll_getnext(const dll_obj_t *restrict dll_obj);

/**
 * Find object by data(key) from start
 * \return a pointer to a matched object if \param fn_search returns a zero. Otherwise, if the desired object does not exist or \param fn_search handler returns a negative value - NULL will be returned.
*/
static inline dll_obj_t	*dll_findkey(const dll_t *restrict dll,
		f_dll_obj_handler fn_search);
// The same as dll_findkey but starts seraching from end
static inline dll_obj_t	*dll_findkeyr(const dll_t *restrict dll,
		f_dll_obj_handler fn_search);

// Find object by index from start(starts from 1)
static inline dll_obj_t	*dll_findid(const dll_t *restrict dll, size_t index);
// Find object by index from end(starts from 1)
static inline dll_obj_t	*dll_findidr(const dll_t *restrict dll, size_t index);

// Print object data without any handler return value checks
static inline int	dll_printone(const dll_obj_t *restrict dll_obj,
		f_dll_obj_handler fn_print);
/**
 * Print all objects from begin
 * If \param fn_print handler returns a negative value - printing will stop
 */
static inline void	dll_print(const dll_t *restrict dll,
		f_dll_obj_handler fn_print);
// The same as dll_print but starts printing from end
static inline void	dll_printr(const dll_t *restrict dll,
		f_dll_obj_handler fn_print);

/**
 * Removing all links to a given object in the list and return it
 * Links to next and previous list object will be saved in the return object
 */
static inline dll_obj_t	*dll_unlink(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj);

// Delete links to given object and free it via dll_freeobj
static inline void	dll_del(dll_t *restrict dll, dll_obj_t *restrict dll_obj);

// Delete object by data(key) from start
static inline bool	dll_delkey(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del);
// Delete object by data(key) from end
static inline bool	dll_delkeyr(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del);

// Delete object by index from start(starts from 1)
static inline bool	dll_delid(dll_t *restrict dll, size_t index);
// Delete object by index from end(starts from 1)
static inline bool	dll_delidr(dll_t *restrict dll, size_t index);

// Free all objects and list
static inline void	dll_free(dll_t *restrict dll);
/**
 * Free given object
 * This function execute \param fn_del if its was setted-up in dll_new\dll_push*
 */
static inline void	dll_freeobj(dll_obj_t *restrict dll_obj);

# include "libdll_sources.h"

# undef __dll_is_bit

#endif /* LIBDLL_H */
