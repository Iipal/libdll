#ifndef LIBDLL_SOURCES_H
# define LIBDLL_SOURCES_H

# ifndef LIBDLL_H
#  error "include only libdll.h"
# endif

# include <stdbool.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

# include "libdll_structs.h"
# include "libdll_bits.h"

static inline dll_t	*dll_init(dll_bits_t bits) {
	dll_t *restrict	out;
	if (!(out = calloc(1, sizeof(*out)))) {
		bool is_not_quiet = !__dll_is_bit(bits, DLL_GBIT_QUIET);
		if (is_not_quiet)
			fprintf(stderr, "%s: failed to calloc-ate memory\n", __func__);
		return NULL;
	}
	out->bits = bits;
	return out;
}

static inline dll_obj_t	*dll_new(void *restrict data,
		size_t size, dll_bits_t obj_bits, f_dll_obj_data_del fn_del) {
	bool	is_not_ignore = !__dll_is_bit(obj_bits, DLL_BIT_EIGN);
	bool	is_not_quiet = !__dll_is_bit(obj_bits, DLL_BIT_EQUIET);
	if (!data && is_not_ignore) {
		if (is_not_quiet)
			fprintf(stderr, "%s: passed NULL pointer to data\n", __func__);
		return NULL;
	}

	dll_obj_t *restrict	out;
	if (!(out = calloc(1, sizeof(*out)))) {
		if (is_not_quiet)
			fprintf(stderr, "%s: failed to calloc-ate memory\n", __func__);
		return NULL;
	}
	if (__dll_is_bit(obj_bits, DLL_BIT_DUP)) {
		if (!(out->data = calloc(1, size))) {
			if (is_not_quiet)
				fprintf(stderr, "%s: failed to calloc-ate memory\n", __func__);
			free(out);
			return NULL;
		}
		out->data = memcpy(out->data, data, size);
	} else {
		out->data = data;
	}
	out->data_size = size;
	out->del = fn_del;
	out->bits = obj_bits;
	return out;
}

static inline dll_obj_t	*dll_pushfront(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		f_dll_obj_data_del fn_del) {
	return dll_pushfrontobj(dll, dll_new(data, data_size, obj_bits, fn_del));
}

static inline dll_obj_t	*dll_pushfrontobj(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj) {
	if (!dll)
		return NULL;

	if (!dll_obj) {
		bool is_not_quiet = !__dll_is_bit(dll->bits, DLL_GBIT_QUIET);
		if (is_not_quiet) {
			fprintf(stderr, "%s: trying to add NULL pointer to list\n",
				__func__);
		}
		return NULL;
	}

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

static inline bool	dll_popfront(dll_t *restrict dll) {
	if (!dll)
		return false;

	if (!dll->head) {
		bool is_not_quiet = !__dll_is_bit(dll->bits, DLL_GBIT_QUIET);
		if (is_not_quiet)
			fprintf(stderr, "%s: list is empty\n", __func__);
		return false;
	}

	dll_obj_t *restrict	save = dll->head->next;
	dll_del(dll, dll->head);
	dll->head = save;
	if (!dll->head)
		dll->last = NULL;
	return true;
}

static inline dll_obj_t	*dll_pushback(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		f_dll_obj_data_del fn_del) {
	return dll_pushbackobj(dll, dll_new(data, data_size, obj_bits, fn_del));
}

static inline dll_obj_t	*dll_pushbackobj(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj) {
	if (!dll)
		return NULL;

	if (!dll_obj) {
		bool is_not_quiet = !__dll_is_bit(dll->bits, DLL_GBIT_QUIET);
		if (is_not_quiet) {
			fprintf(stderr, "%s: trying to add NULL pointer to list\n",
				__func__);
		}
		return NULL;
	}

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

static inline bool	dll_popback(dll_t *restrict dll) {
	if (!dll)
		return false;

	if (!dll->last) {
		bool is_not_quiet = !__dll_is_bit(dll->bits, DLL_GBIT_QUIET);
		if (is_not_quiet)
			fprintf(stderr, "%s: list is empty\n", __func__);
		return false;
	}

	dll_obj_t *restrict	save = dll->last->prev;
	dll_del(dll, dll->last);
	dll->last = save;
	if (!dll->last)
		dll->head = NULL;
	return true;
}

static inline size_t	dll_getsize(const dll_t *restrict dll) {
	return dll ? dll->objs_count : 0;
}
static inline dll_obj_t	*dll_gethead(const dll_t *restrict dll) {
	return dll ? dll->head : NULL;
}
static inline dll_obj_t	*dll_getlast(const dll_t *restrict dll) {
	return dll ? dll->last : NULL;
}

static inline void	*dll_getdata(const dll_obj_t *restrict dll_obj) {
	return dll_obj ? dll_obj->data : NULL;
}
static inline size_t	dll_getdatasize(const dll_obj_t *restrict dll_obj) {
	return dll_obj ? dll_obj->data_size : 0;
}
static inline dll_obj_t	*dll_getprev(const dll_obj_t *restrict dll_obj) {
	return dll_obj ? dll_obj->prev : NULL;
}
static inline dll_obj_t	*dll_getnext(const dll_obj_t *restrict dll_obj) {
	return dll_obj ? dll_obj->next : NULL;
}

static inline dll_obj_t	*dll_findkey(const dll_t *restrict dll,
		f_dll_obj_handler fn_search) {
	if (!dll)
		return NULL;
	if (!fn_search) {
		bool is_not_quiet = !__dll_is_bit(dll->bits, DLL_GBIT_QUIET);
		if (is_not_quiet)
			fprintf(stderr, "%s: handler isn't setted-up\n", __func__);
		return NULL;
	}

	dll_obj_t *restrict	match = dll->head;
	while (match) {
		int ret = fn_search(match->data);
		bool is_not_ignore_error = !__dll_is_bit(match->bits, DLL_BIT_EIGN);
		if (!ret) {
			return (match);
		} else if (0 > ret && is_not_ignore_error) {
			bool is_not_quiet_obj = !__dll_is_bit(match->bits, DLL_BIT_EQUIET);
			if (is_not_quiet_obj) {
				fprintf(stderr, "%s: handler return a negative value: %d\n",
					__func__, ret);
			}
			return NULL;
		}
		match = match->next;
	}
	return NULL;
}

static inline dll_obj_t	*dll_findkeyr(const dll_t *restrict dll,
		f_dll_obj_handler fn_search) {
	if (!dll)
		return NULL;
	if (!fn_search) {
		bool is_not_quiet = !__dll_is_bit(dll->bits, DLL_GBIT_QUIET);
		if (is_not_quiet)
			fprintf(stderr, "%s: handler isn't setted-up\n", __func__);
		return NULL;
	}

	dll_obj_t *restrict	match = dll->last;
	while (match) {
		int ret = fn_search(match->data);
		bool is_not_ignore_error = !__dll_is_bit(match->bits, DLL_BIT_EIGN);
		if (!ret) {
			return (match);
		} else if (0 > ret && is_not_ignore_error) {
			bool is_not_quiet_obj = !__dll_is_bit(match->bits, DLL_BIT_EQUIET);
			if (is_not_quiet_obj) {
				fprintf(stderr, "%s: handler return a negative value: %d\n",
					__func__, ret);
			}
			return NULL;
		}
		match = match->prev;
	}
	return NULL;
}

static inline dll_obj_t	*dll_findid(const dll_t *restrict dll, size_t index) {
	if (!dll || !dll->head || index > dll->objs_count)
		return NULL;

	dll_obj_t *restrict	match = dll->head;
	for (size_t indx = 1; match && indx != index; ++indx, match = match->next)
		;
	return match;
}

static inline dll_obj_t	*dll_findidr(const dll_t *restrict dll, size_t index) {
	if (!dll || !dll->last || index > dll->objs_count)
		return NULL;

	dll_obj_t *restrict	match = dll->last;
	for (size_t indx = 1; match && indx != index; ++indx, match = match->prev)
		;
	return match;
}

static inline int	dll_printone(const dll_obj_t *restrict dll_obj,
		f_dll_obj_handler fn_print) {
	return fn_print(dll_obj->data);
}

static inline void	dll_print(const dll_t *restrict dll,
		f_dll_obj_handler fn_print) {
	if (!dll)
		return ;

	bool is_not_quiet = !__dll_is_bit(dll->bits, DLL_GBIT_QUIET);
	if (is_not_quiet) {
		if (!fn_print) {
			fprintf(stderr, "%s: handler isn't setted-up\n", __func__);
			return ;
		}
		printf("%zu elements\nFrom begin:\n", dll_getsize(dll));
	}

	for (dll_obj_t *restrict i = dll->head; i; i = i->next) {
		int	ret = dll_printone(i, fn_print);
		bool is_not_ignore_error = !__dll_is_bit(i->bits, DLL_BIT_EIGN);
		if (0 > ret && is_not_ignore_error) {
			bool is_not_quiet_obj = !__dll_is_bit(i->bits, DLL_BIT_EQUIET);
			if (is_not_quiet_obj) {
				fprintf(stderr, "%s: handler return a negative value: %d\n",
					__func__, ret);
			}
			break ;
		}
	}
}

static inline void	dll_printr(const dll_t *restrict dll,
		f_dll_obj_handler fn_print) {
	if (!dll)
		return ;

	bool is_not_quiet = !__dll_is_bit(dll->bits, DLL_GBIT_QUIET);
	if (is_not_quiet) {
		if (!fn_print) {
			fprintf(stderr, "%s: handler isn't setted-up\n", __func__);
			return ;
		}
		printf("%zu elements\nFrom begin:\n", dll_getsize(dll));
	}

	for (dll_obj_t *restrict i = dll->last; i; i = i->prev) {
		int	ret = dll_printone(i, fn_print);
		bool is_not_ignore_error = !__dll_is_bit(i->bits, DLL_BIT_EIGN);
		if (0 > ret && is_not_ignore_error) {
			bool is_not_quiet_obj = !__dll_is_bit(i->bits, DLL_BIT_EQUIET);
			if (is_not_quiet_obj) {
				fprintf(stderr, "%s: handler return a negative value: %d\n",
					__func__, ret);
			}
			break ;
		}
	}
}

static inline dll_obj_t	*dll_unlink(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj) {
	if (!dll || !dll_obj)
		return NULL;

	if (dll_obj->prev)
		dll_obj->prev->next = dll_obj->next;
	else
		dll->head = dll_obj->next;
	if (dll_obj->next)
		dll_obj->next->prev = dll_obj->prev;
	return dll_obj;
}

static inline void	dll_del(dll_t *restrict dll, dll_obj_t *restrict dll_obj) {
	dll_freeobj(dll_unlink(dll, dll_obj));
}

static inline bool	dll_delkey(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del) {
	dll_obj_t *restrict del = dll_findkey(dll, fn_search_del);
	if (!del)
		return false;
	dll_del(dll, del);
	if (!dll->head || !dll->last)
		dll->head = dll->last = NULL;
	return true;
}

static inline bool	dll_delkeyr(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del) {
	dll_obj_t *restrict del = dll_findkeyr(dll, fn_search_del);
	if (!del)
		return false;
	dll_del(dll, del);
	if (!dll->head || !dll->last)
		dll->head = dll->last = NULL;
	return true;
}

static inline bool	dll_delid(dll_t *restrict dll, size_t index) {
	dll_obj_t *restrict del = dll_findid(dll, index);
	if (!del)
		return false;
	dll_del(dll, del);
	if (!dll->head || !dll->last)
		dll->head = dll->last = NULL;
	return true;
}

static inline bool	dll_delidr(dll_t *restrict dll, size_t index) {
	dll_obj_t *restrict del = dll_findidr(dll, index);
	if (!del)
		return false;
	dll_del(dll, del);
	if (!dll->head || !dll->last)
		dll->head = dll->last = NULL;
	return true;
}

static inline void	dll_free(dll_t *restrict dll) {
	while (dll_popfront(dll))
		;
	if (dll)
		free(dll);
}

static inline void	dll_freeobj(dll_obj_t *restrict dll_obj) {
	if (!dll_obj)
		return ;

	if (dll_obj->del)
		dll_obj->del(dll_obj->data);
	else if (__dll_is_bit(dll_obj->bits, DLL_BIT_DUP)
		|| __dll_is_bit(dll_obj->bits, DLL_BIT_FREE))
		free(dll_obj->data);
	free(dll_obj);
	dll_obj = NULL;
}

#endif /* LIBDLL_SOURCES_H */
