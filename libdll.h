#ifndef LIBDLL_H
# define LIBDLL_H

# include <stdio.h>
# include <stddef.h>
# include <stdbool.h>
# include <assert.h>
# include <stdlib.h>
# include <string.h>

# define __dll_is_bit(bits, bit) (((bits) & (bit)) == (bit))

# define __dll_attr_align __attribute__((aligned(__BIGGEST_ALIGNMENT__)))

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
 * Default behavior for list
 */
# define DLL_GBIT_DFLT 0

/**
 * Do not print output order and objects count in linked list in dll_print* function
 */
# define DLL_GBIT_QUIET 1

/**
 * All macroses specified for all objects in linked list has prefix:
 * DLL_GBIT_* (use only with dll_init)
 * All macroses specified for only 1 object in linked list has prefix:
 * DLL_BIT_* (use only with dll_new, dll_push*)
 */
typedef unsigned int dll_bits_t;

typedef struct s_dll_obj {
	struct s_dll_obj *restrict	next;
	struct s_dll_obj *restrict	prev;
	void *restrict	data;
	size_t	data_size;
	dll_bits_t	bits;
} __dll_attr_align dll_obj_t;

typedef struct s_dll {
	dll_obj_t *restrict	head;
	dll_obj_t *restrict	last;
	size_t	objs_count;
	dll_bits_t	bits;
} __dll_attr_align dll_t;

typedef int (*f_dll_obj_handler)(const void *restrict);

/**
 * Allocate new list
 * For specifying behavior for all objects - set \param _bits using DLL_GBIT*
 * \return dll_t*
 */
# define dll_init(_bits) __extension__({ \
	dll_t *restrict	__out = NULL; \
	assert((__out = calloc(1, sizeof(*__out)))); \
	__out->bits = _bits; \
	__out; \
})

/**
 * Create a duplicate \param size bytes of \param mem
 */
void	*__dll_memdup(const void *restrict mem, size_t size) {
	void	*dup;
	assert((dup = calloc(1, size)));
	return memcpy(dup, mem, size);
}

/**
 * Create new object
 * \param _data: void*
 * \param _size: size_t: size of _data
 * \param _bits: dll_bits_t
 * \return dll_obj_t*
 */
# define dll_new(_data, _size, _bits) __extension__({ \
	dll_obj_t *restrict __out = NULL; \
	assert((__out = calloc(1, sizeof(*__out)))); \
	__out->data = (void *restrict)(_data); \
	if (__dll_is_bit((_bits), DLL_BIT_DUP)) \
		__out->data = __dll_memdup(_data, _size);\
	__out->data_size = (size_t)(_size); \
	__out->bits = (dll_bits_t)(_bits); \
	__out; \
})

/**
 * Free all data
 * \param _dll: dll_t*
 */
# define dll_free(_dll) __extension__({ \
	__typeof__ (_dll)	__dll = (_dll); \
	while (dll_delhead(__dll)) \
		; \
	free(_dll); \
})

/**
 * Free given object
 * \param _dll_obj: dll_obj_t*
 */
# define dll_freeobj(_dll_obj) __extension__({ \
	__typeof__(_dll_obj) __fdll_obj = (_dll_obj); \
	if (__dll_is_bit(__fdll_obj->bits, DLL_BIT_DUP)) \
		free(__fdll_obj->data); \
	free(__fdll_obj); \
	__fdll_obj = NULL; \
})

/**
 * Delete links to given object
 * \param _dll: dll_t*
 * \param _dll_obj: dll_obj_t*
 */
# define dll_del(_dll, _dll_obj) __extension__({ \
	__typeof__ (_dll) __dll = (_dll); \
	__typeof__ (_dll_obj) __dll_obj = (_dll_obj); \
	if (__dll_obj->prev) { \
		__dll_obj->prev->next = __dll_obj->next; \
	} else { \
		__dll->head = __dll_obj->next; \
	} \
	if (__dll_obj->next) { \
		__dll_obj->next->prev = __dll_obj->prev; \
	} \
	--__dll->objs_count; \
	dll_freeobj(__dll_obj); \
})

/**
 * Returns a count of objects in linked list
 * \param _dll: dll_t*
 */
# define dll_getsize(_dll) __extension__({ (_dll)->objs_count; })

/**
 * Adding a \param dll_obj at the front of list
 * \return a pointer to added \param dll_obj
 */
static inline dll_obj_t	*dll_pushfronto(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj) {
	++dll->objs_count;
	if (!dll->head) {
		dll->head = dll->last = dll_obj;
	} else {
		dll->head->prev = dll_obj;
		dll_obj->next = dll->head;
		dll->head = dll_obj;
	}
	return dll_obj;
}

/**
 * Creating a new dll_obj_t* at the front of list from given parameters
 * \return a pointer to new created dll_obj_t*
 */
static inline dll_obj_t	*dll_pushfront(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_type) {
	return dll_pushfronto(dll, dll_new(data, data_size, obj_type));
}

/**
 * Adding a \param dll_obj at the end of list
 * \return a pointer to added \param dll_obj
 */
static inline dll_obj_t	*dll_pushbacko(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj) {
	++dll->objs_count;
	if (!dll->head) {
		dll->head = dll_obj;
		dll->last = dll_obj;
	} else {
		dll->last->next = dll_obj;
		dll_obj->prev = dll->last;
		dll->last = dll_obj;
	}
	return dll_obj;
}

/**
 * Creating a new dll_obj_t* at the end of list from given parameters
 * \return a pointer to new created dll_obj_t*
 */
static inline dll_obj_t	*dll_pushback(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_type) {
	return dll_pushbacko(dll, dll_new(data, data_size, obj_type));
}

static inline int	dll_printone(const dll_obj_t *restrict dll_obj,
		f_dll_obj_handler fn_print) {
	return fn_print(dll_obj->data);
}

# define __dll_print_logic(_fn_name, _fn, _i) __extension__({ \
	if (0 > dll_printone(_i, _fn) \
	&& !__dll_is_bit((_i)->bits, DLL_BIT_EIGN)) { \
		if (!__dll_is_bit((_i)->bits, DLL_BIT_EQUIET)) \
			fprintf(stderr, #_fn_name ": output processing function " \
										"return negative value\n"); \
		break ; \
	} \
})

/**
 * Print all objects from begin
 * If at least for 1 object \param fn_print handler returns a negative value - printing will stop.
 */
static inline void	dll_print(const dll_t *restrict dll,
		f_dll_obj_handler fn_print) {
	if (!__dll_is_bit(dll->bits, DLL_GBIT_QUIET))
		printf("%zu elements\nFrom begin:\n", dll_getsize(dll));
	for (dll_obj_t *restrict i = dll->head; i; i = i->next) {
		__dll_print_logic(dll_print, fn_print, i);
	}
}

/**
 * Print all objects from end
 * If at least for 1 object \param fn_print handler returns a negative value - printing will stop.
 */
static inline void	dll_printr(const dll_t *restrict dll,
		f_dll_obj_handler fn_print) {
	if (!__dll_is_bit(dll->bits, DLL_GBIT_QUIET))
		printf("%zu elements\nFrom end:\n", dll_getsize(dll));
	for (dll_obj_t *restrict i = dll->last; i; i = i->prev) {
		__dll_print_logic(dll_printr, fn_print, i);
	}
}

# undef __dll_print_logic

# define __dll_findkey_logic(_fn_name, _fn, _m) __extension__({ \
	int	_ret = _fn(_m->data); \
	if (!_ret) { \
		return (_m); \
	} else if (0 > _ret && !__dll_is_bit((_m)->bits, DLL_BIT_EIGN)) { \
		if (!__dll_is_bit((_m)->bits, DLL_BIT_EQUIET)) \
			fprintf(stderr, #_fn_name ": delete processing function " \
										"return negative value\n"); \
		return NULL; \
	} \
})

/**
 * Find object by data(key) from end
 * \return a pointer to a matched object if \param fn_search returns a zero. Otherwise, if the desired object does not exist or \param fn_search handler returns a negative value - NULL will be returned.
*/
static inline dll_obj_t	*dll_findkeyr(const dll_t *restrict dll,
		f_dll_obj_handler fn_search) {
	dll_obj_t *restrict	match = dll->last;

	while (match) {
		__dll_findkey_logic(dll_findkeyr, fn_search, match);
		match = match->prev;
	}
	return NULL;
}

/**
 * Find object by data(key) from start
 * \return a pointer to a matched object if \param fn_search returns a zero. Otherwise, if the desired object does not exist or \param fn_search handler returns a negative value - NULL will be returned.
*/
static inline dll_obj_t	*dll_findkey(const dll_t *restrict dll,
		f_dll_obj_handler fn_search) {
	dll_obj_t *restrict	match = dll->head;

	while (match) {
		__dll_findkey_logic(dll_findkey, fn_search, match);
		match = match->next;
	}
	return NULL;
}

# undef __dll_findkey_logic

# define __dll_delkey_logic(_dll, _find_fn, _fn) __extension__({ \
	dll_obj_t *restrict	del = _find_fn(_dll, _fn); \
	if (!del) \
		return false; \
	dll_del(_dll, del); \
	if (!_dll->head || !_dll->last) \
		dll->head = dll->last = NULL; \
	return true; \
})

/**
 * Delete object by data(key) from end
 */
static inline bool	dll_delkeyr(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del) {
	__dll_delkey_logic(dll, dll_findkeyr, fn_search_del);
}

/**
 * Delete object by data(key) from start
 */
static inline bool	dll_delkey(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del) {
	__dll_delkey_logic(dll, dll_findkey, fn_search_del);
}

# undef __dll_delkey_logic

/**
 * Find object by index from end(starts from 1)
 */
static inline dll_obj_t	*dll_findidr(const dll_t *restrict dll, size_t index) {
	if (index > dll->objs_count)
		return NULL;
	dll_obj_t *restrict	match = dll->last;
	for (size_t indx = 1; match && indx != index; ++indx, match = match->prev)
		;
	return match;
}

/**
 * Find object by index from start(starts from 1)
 */
static inline dll_obj_t	*dll_findid(const dll_t *restrict dll, size_t index) {
	if (index > dll->objs_count)
		return NULL;
	dll_obj_t *restrict	match = dll->head;
	for (size_t indx = 1; match && indx != index; ++indx, match = match->next)
		;
	return match;
}

# define __dll_delid_logic(_dll, _find_fn, _idx) __extension__({ \
	dll_obj_t *restrict	del = _find_fn(_dll, _idx); \
	if (!del) \
		return false; \
	dll_del(_dll, del); \
	if (!_dll->head || !_dll->last) \
		_dll->head = _dll->last = NULL; \
	return true; \
})

/**
 * Delete object by index from end(starts from 1)
 */
static inline bool	dll_delidr(dll_t *restrict dll, size_t index) {
	__dll_delid_logic(dll, dll_findid, index);
}

/**
 * Delete object by index from start(starts from 1)
 */
static inline bool	dll_delid(dll_t *restrict dll, size_t index) {
	__dll_delid_logic(dll, dll_findidr, index);
}

# undef __dll_delid_logic

/**
 * Delete first object
 */
static inline bool	dll_delhead(dll_t *restrict dll) {
	if (!dll || !dll->head)
		return false;
	dll_obj_t *restrict	save = dll->head->next;
	dll_del(dll, dll->head);
	dll->head = save;
	if (!dll->head)
		dll->last = NULL;
	return true;
}

/**
 * Delete last object
 */
static inline bool	dll_dellast(dll_t *restrict dll) {
	if (!dll || !dll->last)
		return false;
	dll_obj_t *restrict	save = dll->last->prev;
	dll_del(dll, dll->last);
	dll->last = save;
	if (!dll->last)
		dll->head = NULL;
	return true;
}

# undef __dll_is_bit
# undef __dll_attr_align

#endif /* LIBDLL_H */
