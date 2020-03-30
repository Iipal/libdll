#ifndef LIBDLL_H
# define LIBDLL_H

# include <stdio.h>
# include <stddef.h>
# include <stdbool.h>
# include <assert.h>
# include <stdlib.h>

# define __dll_is_bit(bits, bit) (((bits) & (bit)) == (bit))

# define __dll_attr_align __attribute__((aligned(__BIGGEST_ALIGNMENT__)))

/**
 * For all functions which call an object handler - if the handler returns a negative value it's will be ignored
 */
# define DLL_BIT_EIGN 1

typedef unsigned int dll_obj_bits_t;

typedef struct s_dll_obj {
	struct s_dll_obj *restrict	next;
	struct s_dll_obj *restrict	prev;
	void *restrict	data;
	size_t	data_size;
	dll_obj_bits_t	bits;
} __dll_attr_align dll_obj_t;

typedef struct s_dll {
	dll_obj_t *restrict	head;
	dll_obj_t *restrict	last;
	size_t	objs_count;
} __dll_attr_align dll_t;

typedef int (*f_dll_obj_handler)(const void *restrict);

# define dll_init() __extension__({ \
	dll_t *restrict	__out = NULL; \
	assert((__out = calloc(1, sizeof(*__out)))); \
	__out; \
})

/**
 * Create new token
 * \param _data: void*
 * \param _size: size_t: size of _data
 * \param _bits: dll_obj_bits_t
 * \return dll_obj_t*
 */
# define dll_new(_data, _size, _bits) __extension__({ \
	dll_obj_t *restrict __out = NULL; \
	assert((__out = calloc(1, sizeof(*__out)))); \
	__out->data = (void *restrict)(_data); \
	__out->data_size = (size_t)(_size); \
	__out->bits = (dll_obj_bits_t)(_bits); \
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
 * Free given token
 * \param _fdll: dll_obj_t*
 */
# define dll_freetok(_fdll) __extension__({ \
	__typeof__(_fdll) __fdll = _fdll; \
	free(__fdll); \
	__fdll = NULL; \
})

/**
 * Delete links to given token
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
	dll_freetok(__dll_obj); \
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
		dll_obj_bits_t obj_type) {
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
		dll_obj_bits_t obj_type) {
	return dll_pushbacko(dll, dll_new(data, data_size, obj_type));
}

static inline int	dll_printone(const dll_obj_t *restrict dll_obj,
		f_dll_obj_handler fn_print) {
	return fn_print(dll_obj->data);
}

/**
 * Print all objects from begin
 * If at least for 1 object \param fn_print handler returns a negative value - printing will stop.
 */
static inline void	dll_print(const dll_t *restrict dll,
		f_dll_obj_handler fn_print) {
	printf("%zu elements\nFrom begin:\n", dll_getsize(dll));
	for (dll_obj_t *restrict i = dll->head; i; i = i->next) {
		if (0 > dll_printone(i, fn_print)
		&& !__dll_is_bit(i->bits, DLL_BIT_EIGN))
			break ;
	}
}

/**
 * Print all objects from end
 * If at least for 1 object \param fn_print handler returns a negative value - printing will stop.
 */
static inline void	dll_printr(const dll_t *restrict dll,
		f_dll_obj_handler fn_print) {
	printf("%zu elements\nFrom end:\n", dll_getsize(dll));
	for (dll_obj_t *restrict i = dll->last; i; i = i->prev) {
		if (0 > dll_printone(i, fn_print)
		&& !__dll_is_bit(i->bits, DLL_BIT_EIGN))
			break ;
	}
}

/**
 * Find object by data(key)
 * \return a pointer to a matched object if \param fn_search returns a zero. Otherwise, if the desired object does not exist or \param fn_search handler returns a negative value - NULL will be returned.
*/
static inline dll_obj_t	*dll_findkey(const dll_t *restrict dll,
		f_dll_obj_handler fn_search) {
	dll_obj_t *restrict	match = dll->head;

	while (match) {
		int	fn_ret = fn_search(match->data);
		if (!fn_ret) {
			return match;
		} else if (0 > fn_ret
		&& !__dll_is_bit(match->bits, DLL_BIT_EIGN)) {
			return NULL;
		}
		match = match->next;
	}
	return NULL;
}

/**
 * Delete object by data(key)
 */
static inline bool	dll_delkey(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del) {
	dll_obj_t *restrict	del = dll_findkey(dll, fn_search_del);
	if (!del)
		return false;
	dll_del(dll, del);
	if (!dll->head || !dll->last)
		dll->head = dll->last = NULL;
	return true;
}

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

/**
 * Delete object by index(starts from 1)
 */
static inline bool	dll_delid(dll_t *restrict dll, size_t index) {
	dll_obj_t *restrict	del = dll_findid(dll, index);
	if (!del)
		return false;
	dll_del(dll, del);
	if (!dll->head || !dll->last)
		dll->head = dll->last = NULL;
	return true;
}

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
