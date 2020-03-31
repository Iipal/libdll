#ifndef LIBDLL_H
# define LIBDLL_H

# include <stdbool.h>
# include "libdll_structs.h"

/**
 * Allocate new list
 * For specifying behavior for all objects - set \param _bits using DLL_GBIT*
 * \return dll_t*
 */
static inline dll_t	*dll_init(dll_bits_t bits);

/**
 * Create new object
 * For specifying behavior for this object - set \param _bits using DLL_BIT*
 */
static inline dll_obj_t	*dll_new(void *restrict data,
		size_t size, dll_bits_t bits);

/**
 * Creating a new dll_obj_t* at the front of list from given parameters
 * \return a pointer to new created dll_obj_t*
 */
static inline dll_obj_t	*dll_pushfront(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits);
static inline dll_obj_t	*dll_pushfrontobj(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj);
// Delete first object in list
static inline bool	dll_popfront(dll_t *restrict dll);

/**
 * Creating a new dll_obj_t* at the end of list from given parameters
 * \return a pointer to new created dll_obj_t*
 */
static inline dll_obj_t	*dll_pushback(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits);
static inline dll_obj_t	*dll_pushbackobj(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj);
// Delete last object in list
static inline bool	dll_popback(dll_t *restrict dll);

/**
 * Returns a pointer to data in object casted to \param _t
 * \param _dll_obj: dll_obj_t*
 * \param _t: cast to this type
 */
# define dll_getdata(_t, _dll_obj) ((_t)((_dll_obj)->data))

// Returns a count of objects in list
static inline size_t	dll_getsize(const dll_t *restrict dll);
// Returns a current first object in list
static inline dll_obj_t	*dll_gethead(const dll_t *restrict dll);
// Returns a current last object in list
static inline dll_obj_t	*dll_getlast(const dll_t *restrict dll);
// Returns a previous object
static inline dll_obj_t	*dll_getprev(const dll_obj_t *restrict dll_obj);
// Returns a next object
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
 * If at least for 1 object \param fn_print handler returns a negative value - printing will stop.
 */
static inline void	dll_print(const dll_t *restrict dll,
		f_dll_obj_handler fn_print);
// The same as dll_print but starts printing from end
static inline void	dll_printr(const dll_t *restrict dll,
		f_dll_obj_handler fn_print);

// Delete links to given object and free
static inline void	dll_del(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj);

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
// Free given object
static inline void	dll_freeobj(dll_obj_t *restrict dll_obj);

# include "libdll_sources.h"

# undef __dll_is_bit

#endif /* LIBDLL_H */
