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
# include "libdll_errno.h"

static inline dll_t	*dll_init(dll_bits_t bits) {
	bool is_not_ign = !__dll_is_bit(bits, DLL_BIT_EIGN);
	if (__DLL_MAX_VALID_LIST_MASK < bits && is_not_ign) {
		__dll_seterrno(__DLL_EMASK);
		return NULL;
	}

	dll_t *restrict out;
	if (!(out = calloc(1, sizeof(*out)))) {
		__dll_seterrno(__DLL_ECALLOC);
		return NULL;
	}
	out->bits = bits;
	return out;
}

static inline dll_obj_t	*dll_new(void *restrict data,
		size_t size, dll_bits_t obj_bits, f_dll_obj_data_del fn_del) {
	bool is_duplicate_memory = __dll_is_bit(obj_bits, DLL_BIT_DUP);
	bool is_not_ign = !__dll_is_bit(obj_bits, DLL_BIT_EIGN);
	if ((!data || !size || __DLL_MAX_VALID_OBJ_MASK < obj_bits) && is_not_ign) {
		__dll_seterrno((!data || !size) ? __DLL_ENULL : __DLL_EMASK);
		return NULL;
	}

	dll_obj_t *restrict out;
	if (!(out = calloc(1, sizeof(*out)))) {
		__dll_seterrno(__DLL_ECALLOC);
		return NULL;
	}
	if (is_duplicate_memory && !(out->data = calloc(1, size))) {
		__dll_seterrno(__DLL_EDUP);
		free(out);
		return NULL;
	}
	out->data = is_duplicate_memory ? memcpy(out->data, data, size) : data;
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
	dll_obj_t *restrict new_obj = dll_new(data, data_size, obj_bits, fn_del);
	if (new_obj)
		dll_pushfrontobj(dll, new_obj);
	return NULL;
}

static inline dll_obj_t	*dll_pushfrontobj(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj) {
	if (!dll || !dll_obj) {
		__dll_seterrno(__DLL_ENULL);
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
	if (!dll || !dll->head) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
		return false;
	}

	dll_obj_t *restrict save = dll->head->next;
	dll_del(dll, dll->head);
	dll->head = save;
	if (!dll->head) {
		__dll_seterrno(__DLL_EEMPTY);
		dll->last = NULL;
	}
	return true;
}

static inline dll_obj_t	*dll_pushback(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		f_dll_obj_data_del fn_del) {
	dll_obj_t *restrict new_obj = dll_new(data, data_size, obj_bits, fn_del);
	if (new_obj)
		dll_pushbackobj(dll, new_obj);
	return new_obj;
}

static inline dll_obj_t	*dll_pushbackobj(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj) {
	if (!dll || !dll_obj) {
		__dll_seterrno(__DLL_ENULL);
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
	if (!dll || !dll->last) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
		return false;
	}

	dll_obj_t *restrict save = dll->last->prev;
	dll_del(dll, dll->last);
	dll->last = save;
	if (!dll->last) {
		__dll_seterrno(__DLL_EEMPTY);
		dll->head = NULL;
	}
	return true;
}

static inline size_t	dll_getsize(const dll_t *restrict dll) {
	if (!dll || !dll->objs_count) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
		return 0;
	}
	return dll->objs_count;
}
static inline dll_obj_t	*dll_gethead(const dll_t *restrict dll) {
	if (!dll || !dll->head) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
		return NULL;
	}
	return dll->head;
}
static inline dll_obj_t	*dll_getlast(const dll_t *restrict dll) {
	if (!dll || !dll->last) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
		return NULL;
	}
	return dll->last;
}

static inline void	*dll_getdata(const dll_obj_t *restrict dll_obj) {
	if (!dll_obj || !dll_obj->data) {
		__dll_seterrno(!dll_obj ? __DLL_ENULL : __DLL_EEMPTY_OBJ);
		return NULL;
	}
	return dll_obj->data;
}
static inline size_t	dll_getdatasize(const dll_obj_t *restrict dll_obj) {
	if (!dll_obj || !dll_obj->data_size) {
		__dll_seterrno(!dll_obj ? __DLL_ENULL : __DLL_EEMPTY_OBJ);
		return 0;
	}
	return dll_obj->data_size;
}
static inline dll_obj_t	*dll_getprev(const dll_obj_t *restrict dll_obj) {
	if (!dll_obj) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	return dll_obj->prev;
}
static inline dll_obj_t	*dll_getnext(const dll_obj_t *restrict dll_obj) {
	if (!dll_obj) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	return dll_obj->next;
}

static inline dll_obj_t	*dll_findkey(const dll_t *restrict dll,
		f_dll_obj_handler fn_search) {
	if (!dll) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if ((!fn_search || !dll->head) && is_not_ign_err) {
		__dll_seterrno(!fn_search ? __DLL_ENOHANDLER : __DLL_EEMPTY);
		return NULL;
	}

	dll_obj_t *restrict match = dll->head;
	while (match) {
		is_not_ign_err = !__dll_is_bit(match->bits, DLL_BIT_EIGN);
		if (!match->data && is_not_ign_err) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			return NULL;
		}

		int ret = fn_search(match->data);
		if (!ret) {
			return (match);
		} else if (0 > ret && is_not_ign_err) {
			__dll_seterrno(__DLL_ENEGHANDLER);
			return NULL;
		}
		match = match->next;
	}
	return NULL;
}

static inline dll_obj_t	*dll_findkeyr(const dll_t *restrict dll,
		f_dll_obj_handler fn_search) {
	if (!dll) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if ((!fn_search || !dll->last) && is_not_ign_err) {
		__dll_seterrno(!fn_search ? __DLL_ENOHANDLER : __DLL_EEMPTY);
		return NULL;
	}

	dll_obj_t *restrict	match = dll->last;
	while (match) {
		is_not_ign_err = !__dll_is_bit(match->bits, DLL_BIT_EIGN);
		if (!match->data && is_not_ign_err) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			return NULL;
		}

		int ret = fn_search(match->data);
		if (!ret) {
			return (match);
		} else if (0 > ret && is_not_ign_err) {
			__dll_seterrno(__DLL_ENEGHANDLER);
			return NULL;
		}
		match = match->prev;
	}
	return NULL;
}

static inline dll_obj_t	*dll_findid(const dll_t *restrict dll, size_t index) {
	if (!dll) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if ((!dll->head || index > dll->objs_count) && is_not_ign_err) {
		__dll_seterrno(!dll->head ? __DLL_EEMPTY : __DLL_EOVERFLOW);
		return NULL;
	}

	dll_obj_t *restrict	match = dll->head;
	for (size_t indx = 1; match && indx != index; ++indx, match = match->next)
		;
	return match;
}

static inline dll_obj_t	*dll_findidr(const dll_t *restrict dll, size_t index) {
	if (!dll) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if ((!dll->last || index > dll->objs_count) && is_not_ign_err) {
		__dll_seterrno(!dll->last ? __DLL_EEMPTY : __DLL_EOVERFLOW);
		return NULL;
	}

	dll_obj_t *restrict	match = dll->last;
	for (size_t indx = 1; match && indx != index; ++indx, match = match->prev)
		;
	return match;
}

static inline int	dll_printone(const dll_obj_t *restrict dll_obj,
		f_dll_obj_handler fn_print) {
	if (!dll_obj || !fn_print) {
		__dll_seterrno(!fn_print ? __DLL_ENOHANDLER : __DLL_ENULL);
		return -1;
	}
	bool is_not_ign_err = !__dll_is_bit(dll_obj->bits, DLL_BIT_EIGN);
	if (!dll_obj->data && is_not_ign_err) {
		__dll_seterrno(__DLL_EEMPTY_OBJ);
		return -1;
	}

	int ret = fn_print(dll_obj->data);
	if (0 > ret && is_not_ign_err)
		__dll_seterrno(__DLL_ENEGHANDLER);
	return ret;

}

static inline bool	dll_print(const dll_t *restrict dll,
		f_dll_obj_handler fn_print) {
	if (!dll) {
		__dll_seterrno(__DLL_ENULL);
		return false;
	}

	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if ((!fn_print || !dll->head) && is_not_ign_err) {
		__dll_seterrno(!fn_print ? __DLL_ENOHANDLER : __DLL_EEMPTY);
		return false;
	}

	for (dll_obj_t *restrict i = dll->head; i; i = i->next) {
		is_not_ign_err = !__dll_is_bit(i->bits, DLL_BIT_EIGN);
		if (!i->data && is_not_ign_err) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			return false;
		}
		int	ret = fn_print(i->data);
		if (0 > ret && is_not_ign_err) {
			__dll_seterrno(__DLL_ENEGHANDLER);
			return false;
		}
	}
	return true;
}

static inline bool	dll_printr(const dll_t *restrict dll,
		f_dll_obj_handler fn_print) {
	if (!dll) {
		__dll_seterrno(__DLL_ENULL);
		return false;
	}

	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if ((!fn_print || !dll->last) && is_not_ign_err) {
		__dll_seterrno(!fn_print ? __DLL_ENOHANDLER : __DLL_EEMPTY);
		return false;
	}

	for (dll_obj_t *restrict i = dll->last; i; i = i->prev) {
		is_not_ign_err = !__dll_is_bit(i->bits, DLL_BIT_EIGN);
		if (!i->data && is_not_ign_err) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			return false;
		}
		int	ret = fn_print(i->data);
		if (0 > ret && is_not_ign_err) {
			__dll_seterrno(__DLL_ENEGHANDLER);
			return false;
		}
	}
	return true;
}

static inline dll_obj_t	*dll_unlink(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj) {
	if (!dll || !dll_obj) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}

	if (dll_obj->prev)
		dll_obj->prev->next = dll_obj->next;
	else
		dll->head = dll_obj->next;
	if (dll_obj->next)
		dll_obj->next->prev = dll_obj->prev;
	return dll_obj;
}

static inline bool	dll_del(dll_t *restrict dll, dll_obj_t *restrict dll_obj) {
	dll_obj_t *restrict	del_obj = dll_unlink(dll, dll_obj);
	if (!del_obj)
		return false;
	return dll_freeobj(del_obj);
}

static inline bool	dll_delkey(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del) {
	dll_obj_t *restrict del = dll_findkey(dll, fn_search_del);
	if (!del)
		return false;
	bool ret = dll_del(dll, del);
	if (!dll->head || !dll->last) {
		__dll_seterrno(__DLL_EEMPTY);
		dll->head = dll->last = NULL;
	}
	return ret;
}

static inline bool	dll_delkeyr(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del) {
	dll_obj_t *restrict del = dll_findkeyr(dll, fn_search_del);
	if (!del)
		return false;
	bool ret = dll_del(dll, del);
	if (!dll->head || !dll->last) {
		__dll_seterrno(__DLL_EEMPTY);
		dll->head = dll->last = NULL;
	}
	return ret;
}

static inline bool	dll_delid(dll_t *restrict dll, size_t index) {
	dll_obj_t *restrict del = dll_findid(dll, index);
	if (!del)
		return false;
	bool ret = dll_del(dll, del);
	if (!dll->head || !dll->last) {
		__dll_seterrno(__DLL_EEMPTY);
		dll->head = dll->last = NULL;
	}
	return ret;
}

static inline bool	dll_delidr(dll_t *restrict dll, size_t index) {
	dll_obj_t *restrict del = dll_findidr(dll, index);
	if (!del)
		return false;
	bool ret = dll_del(dll, del);
	if (!dll->head || !dll->last) {
		__dll_seterrno(__DLL_EEMPTY);
		dll->head = dll->last = NULL;
	}
	return ret;
}

static inline bool	dll_free(dll_t *restrict dll) {
	if (!dll || !dll->head) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
		if (dll)
			free(dll);
		return false;
	}
	while (dll_popfront(dll))
		;
	free(dll);
	return true;
}

static inline bool	dll_freeobj(dll_obj_t *restrict dll_obj) {
	if (!dll_obj) {
		__dll_seterrno(__DLL_ENULL);
		return false;
	}

	if (dll_obj->del)
		dll_obj->del(dll_obj->data);
	else if (__dll_is_bit(dll_obj->bits, DLL_BIT_DUP)
		|| __dll_is_bit(dll_obj->bits, DLL_BIT_FREE))
		free(dll_obj->data);
	free(dll_obj);
	dll_obj = NULL;
	return true;
}

#endif /* LIBDLL_SOURCES_H */
