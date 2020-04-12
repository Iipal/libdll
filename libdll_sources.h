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

#ifndef LIBDLL_SOURCES_H
# define LIBDLL_SOURCES_H

# ifndef LIBDLL_H
#  error "include only libdll.h"
# endif

# include <stdbool.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <sys/cdefs.h>

# ifdef __glibc_likely
#  define __dll_likely(_cond) __glibc_likely(_cond)
# else
#  define __dll_likely(_cond) (_cond)
# endif

# ifdef __glibc_unlikely
#  define __dll_unlikely(_cond) __glibc_unlikely(_cond)
# else
#  define __dll_unlikely(_cond) (_cond)
# endif

# include "libdll_structs.h"
# include "libdll_bits.h"
# include "libdll_errno.h"

static inline dll_t	*dll_init(dll_bits_t bits) {
	bool is_not_ign = !__dll_is_bit(bits, DLL_BIT_EIGN);
	if (__dll_unlikely(__DLL_MAX_VALID_LIST_MASK < bits && is_not_ign)) {
		__dll_seterrno(__DLL_EMASK);
		return NULL;
	}

	dll_t *restrict out;
	if (__dll_unlikely(!(out = calloc(1, sizeof(*out))))) {
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
	if (__dll_unlikely((!data || !size || __DLL_MAX_VALID_OBJ_MASK < obj_bits) && is_not_ign)) {
		__dll_seterrno((!data || !size) ? __DLL_ENULL : __DLL_EMASK);
		return NULL;
	}

	dll_obj_t *restrict out;
	if (!(out = calloc(1, sizeof(*out)))) {
		__dll_seterrno(__DLL_ECALLOC);
		return NULL;
	}
	if (__dll_unlikely(is_duplicate_memory && !(out->data = calloc(1, size)))) {
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
	if (__dll_unlikely(!!new_obj))
		dll_pushfrontobj(dll, new_obj);
	return new_obj;
}

static inline dll_obj_t	*dll_pushfrontobj(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj) {
	if (__dll_unlikely(!dll || !dll_obj)) {
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

static inline dll_obj_t	*dll_pushback(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		f_dll_obj_data_del fn_del) {
	dll_obj_t *restrict new_obj = dll_new(data, data_size, obj_bits, fn_del);
	if (__dll_unlikely(!!new_obj))
		dll_pushbackobj(dll, new_obj);
	return new_obj;
}

static inline dll_obj_t	*dll_pushbackobj(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj) {
	if (__dll_unlikely(!dll || !dll_obj)) {
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

static inline bool	dll_popfront(dll_t *restrict dll) {
	if (__dll_unlikely(!dll || !dll->head)) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
		return false;
	}

	dll_obj_t *restrict save = dll->head->next;
	dll_del(dll, dll->head);
	dll->head = save;
	if (!dll->head) {
		__dll_seterrno(__DLL_EEMPTY);
		dll->last = NULL;
		return false;
	}
	return true;
}

static inline bool	dll_popback(dll_t *restrict dll) {
	if (__dll_unlikely(!dll || !dll->last)) {
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
	if (__dll_unlikely(!dll || !dll->objs_count)) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
		return 0;
	}
	return dll->objs_count;
}
static inline dll_obj_t	*dll_gethead(const dll_t *restrict dll) {
	if (__dll_unlikely(!dll || !dll->head)) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
		return NULL;
	}
	return dll->head;
}
static inline dll_obj_t	*dll_getlast(const dll_t *restrict dll) {
	if (__dll_unlikely(!dll || !dll->last)) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
		return NULL;
	}
	return dll->last;
}

static inline void	*dll_getdata(const dll_obj_t *restrict dll_obj) {
	if (__dll_unlikely(!dll_obj || !dll_obj->data)) {
		__dll_seterrno(!dll_obj ? __DLL_ENULL : __DLL_EEMPTY_OBJ);
		return NULL;
	}
	return dll_obj->data;
}
static inline size_t	dll_getdatasize(const dll_obj_t *restrict dll_obj) {
	if (__dll_unlikely(!dll_obj || !dll_obj->data_size)) {
		__dll_seterrno(!dll_obj ? __DLL_ENULL : __DLL_EEMPTY_OBJ);
		return 0;
	}
	return dll_obj->data_size;
}
static inline dll_obj_t	*dll_getprev(const dll_obj_t *restrict dll_obj) {
	if (__dll_unlikely(!dll_obj)) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	return dll_obj->prev;
}
static inline dll_obj_t	*dll_getnext(const dll_obj_t *restrict dll_obj) {
	if (__dll_unlikely(!dll_obj)) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	return dll_obj->next;
}
static inline size_t	dll_getid(const dll_t *restrict dll, const dll_obj_t *restrict dll_obj) {
	if (__dll_unlikely(!dll || !dll_obj)) {
		__dll_seterrno(__DLL_ENULL);
		return __SIZE_MAX__;
	}
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely(!dll->head && is_not_ign_err)) {
		__dll_seterrno(__DLL_EEMPTY);
		return __SIZE_MAX__;
	}

	size_t idx = 0;
	for (dll_obj_t *restrict iobj = dll->head; iobj; iobj = iobj->next) {
		++idx;
		if (iobj == dll_obj)
			return idx;
	}
	__dll_seterrno(__DLL_EOUTOFRANGE);
	return __SIZE_MAX__;
}

static inline dll_obj_t	*dll_findkey(const dll_t *restrict dll,
		f_dll_obj_handler fn_search) {
	if (__dll_unlikely(!dll)) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely((!fn_search || !dll->head) && is_not_ign_err)) {
		__dll_seterrno(!fn_search ? __DLL_ENOHANDLER : __DLL_EEMPTY);
		return NULL;
	}

	dll_obj_t *restrict match = dll->head;
	while (match) {
		is_not_ign_err = !__dll_is_bit(match->bits, DLL_BIT_EIGN);
		if (__dll_unlikely(!match->data && is_not_ign_err)) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			return NULL;
		}

		int ret = fn_search(match->data, any_data);
		if (__dll_likely(!ret)) {
			return (match);
		} else if (__dll_unlikely(0 > ret && is_not_ign_err)) {
			__dll_seterrno(__DLL_ENEGHANDLER);
			return NULL;
		}
		match = match->next;
	}
	return NULL;
}

static inline dll_obj_t	*dll_findkeyr(const dll_t *restrict dll,
		f_dll_obj_handler fn_search, void *restrict any_data) {
	if (__dll_unlikely(!dll)) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely((!fn_search || !dll->last) && is_not_ign_err)) {
		__dll_seterrno(!fn_search ? __DLL_ENOHANDLER : __DLL_EEMPTY);
		return NULL;
	}

	dll_obj_t *restrict	match = dll->last;
	while (match) {
		is_not_ign_err = !__dll_is_bit(match->bits, DLL_BIT_EIGN);
		if (__dll_unlikely(!match->data && is_not_ign_err)) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			return NULL;
		}

		int ret = fn_search(match->data, any_data);
		if (__dll_likely(!ret)) {
			return (match);
		} else if (__dll_unlikely(0 > ret && is_not_ign_err)) {
			__dll_seterrno(__DLL_ENEGHANDLER);
			return NULL;
		}
		match = match->prev;
	}
	return NULL;
}

static inline dll_obj_t	*dll_findid(const dll_t *restrict dll, size_t index) {
	if (__dll_unlikely(!dll)) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely((!dll->head || index > dll->objs_count) && is_not_ign_err)) {
		__dll_seterrno(!dll->head ? __DLL_EEMPTY : __DLL_EOVERFLOW);
		return NULL;
	}

	dll_obj_t *restrict	match = dll->head;
	for (size_t i = 1; match && (i != index); i++)
		match = match->next;
	return match;
}

static inline dll_obj_t	*dll_findidr(const dll_t *restrict dll, size_t index) {
	if (__dll_unlikely(!dll)) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely((!dll->last || index > dll->objs_count) && is_not_ign_err)) {
		__dll_seterrno(!dll->last ? __DLL_EEMPTY : __DLL_EOVERFLOW);
		return NULL;
	}

	dll_obj_t *restrict	match = dll->last;
		for (size_t i = 1; match && (i != index); i++)
		match = match->prev;
	return match;
}

static inline dll_t	*dll_dup(const dll_t *restrict dll, size_t start, size_t n) {
	if (__dll_unlikely(!dll)) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely((!dll->head || start > dll_getsize(dll)) && is_not_ign_err)) {
		__dll_seterrno(!dll->head ? __DLL_EEMPTY : __DLL_EOVERFLOW);
		return NULL;
	}

	dll_t *restrict	new_list = dll_init(dll->bits);
	if (__dll_unlikely(!new_list)) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}

	dll_obj_t *restrict	iobj = dll_findid(dll, start);
	while (iobj && n--) {
		dll_bits_t	new_bits = (iobj->bits & ~(DLL_BIT_DUP | DLL_BIT_FREE));
		dll_obj_t *restrict	new_obj_ptr = dll_pushback(new_list,
			iobj->data, iobj->data_size, new_bits, NULL);
		if (__dll_unlikely(!new_obj_ptr && is_not_ign_err)) {
			dll_free(new_list);
			return NULL;
		}
	}
	return new_list;
}

static inline int	dll_printone(const dll_obj_t *restrict dll_obj,
		f_dll_obj_handler_index fn_print_idx, size_t idx) {
	if (__dll_unlikely(!dll_obj || !fn_print_idx)) {
		__dll_seterrno(!fn_print_idx ? __DLL_ENOHANDLER : __DLL_ENULL);
		return -1;
	}
	bool is_not_ign_err = !__dll_is_bit(dll_obj->bits, DLL_BIT_EIGN);
	if (__dll_unlikely(!dll_obj->data && is_not_ign_err)) {
		__dll_seterrno(__DLL_EEMPTY_OBJ);
		return -1;
	}

	int ret = fn_print_idx(dll_obj->data, idx);
	if (__dll_unlikely(0 > ret && is_not_ign_err))
		__dll_seterrno(__DLL_ENEGHANDLER);
	return ret;

}

static inline bool	dll_print(const dll_t *restrict dll,
		f_dll_obj_handler_index fn_print_idx) {
	if (__dll_unlikely(!dll)) {
		__dll_seterrno(__DLL_ENULL);
		return false;
	}

	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely((!fn_print_idx || !dll->head) && is_not_ign_err)) {
		__dll_seterrno(!fn_print_idx ? __DLL_ENOHANDLER : __DLL_EEMPTY);
		return false;
	}

	size_t idx = 0;
	for (dll_obj_t *restrict i = dll->head; i; i = i->next) {
		is_not_ign_err = !__dll_is_bit(i->bits, DLL_BIT_EIGN);
		if (__dll_unlikely(!i->data && is_not_ign_err)) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			return false;
		}
		int	ret = fn_print_idx(i->data, ++idx);
		if (__dll_unlikely(0 > ret && is_not_ign_err)) {
			__dll_seterrno(__DLL_ENEGHANDLER);
			return false;
		}
	}
	return true;
}

static inline bool	dll_printr(const dll_t *restrict dll,
		f_dll_obj_handler_index fn_print_idx) {
	if (__dll_unlikely(!dll)) {
		__dll_seterrno(__DLL_ENULL);
		return false;
	}

	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely((!fn_print_idx || !dll->last) && is_not_ign_err)) {
		__dll_seterrno(!fn_print_idx ? __DLL_ENOHANDLER : __DLL_EEMPTY);
		return false;
	}

	size_t idx = 0;
	for (dll_obj_t *restrict i = dll->last; i; i = i->prev) {
		is_not_ign_err = !__dll_is_bit(i->bits, DLL_BIT_EIGN);
		if (__dll_unlikely(!i->data && is_not_ign_err)) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			return false;
		}
		int	ret = fn_print_idx(i->data, ++idx);
		if (__dll_unlikely(0 > ret && is_not_ign_err)) {
			__dll_seterrno(__DLL_ENEGHANDLER);
			return false;
		}
	}
	return true;
}

static inline size_t	dll_printn(const dll_t *restrict dll,
	f_dll_obj_handler_index fn_print_idx, size_t start, size_t n) {
	if (__dll_unlikely(!dll)) {
		__dll_seterrno(__DLL_ENULL);
		return false;
	}

	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely((!fn_print_idx || !dll->head || start > dll->objs_count)
	&& is_not_ign_err)) {
		__dll_seterrno(!fn_print_idx
			? __DLL_ENOHANDLER
			: (!dll->head
				? __DLL_EEMPTY
				: __DLL_EOVERFLOW));
		return 0;
	}

	size_t	n_iters = 0;
	size_t	idx = start;
	for (dll_obj_t *restrict iobj = dll_findid(dll, start); iobj; iobj = iobj->next) {
		if (idx == (start + n))
			break ;
		++n_iters;
		is_not_ign_err = !__dll_is_bit(iobj->bits, DLL_BIT_EIGN);
		if (__dll_unlikely(!iobj->data && is_not_ign_err)) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			return 0;
		}
		int	ret = fn_print_idx(iobj->data, idx++);
		if (__dll_unlikely(0 > ret && is_not_ign_err)) {
			__dll_seterrno(__DLL_ENEGHANDLER);
			return 0;
		}
	}
	return n_iters;
}

static inline size_t	dll_printnr(const dll_t *restrict dll,
	f_dll_obj_handler_index fn_print_idx, size_t start, size_t n) {
	if (__dll_unlikely(!dll)) {
		__dll_seterrno(__DLL_ENULL);
		return 0;
	}

	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely((!fn_print_idx || !dll->last || start > dll->objs_count)
	&& is_not_ign_err)) {
		__dll_seterrno(!fn_print_idx
			? __DLL_ENOHANDLER
			: (!dll->head
				? __DLL_EEMPTY
				: __DLL_EOVERFLOW));
		return 0;
	}

	size_t	n_iters = 0;
	size_t	idx = start;
	size_t	end = ((n >= start) ? 0 : (start - n));
	for (dll_obj_t *restrict iobj = dll_findid(dll, start); iobj; iobj = iobj->prev) {
		if (idx == end)
			break ;
		++n_iters;
		is_not_ign_err = !__dll_is_bit(iobj->bits, DLL_BIT_EIGN);
		if (__dll_unlikely(!iobj->data && is_not_ign_err)) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			return 0;
		}
		int	ret = fn_print_idx(iobj->data, idx--);
		if (__dll_unlikely(0 > ret && is_not_ign_err)) {
			__dll_seterrno(__DLL_ENEGHANDLER);
			return 0;
		}
	}
	return n_iters;
}

static inline dll_obj_t	*dll_unlink(dll_t *restrict dll,
		dll_obj_t *restrict dll_obj) {
	if (__dll_unlikely(!dll || !dll_obj)) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}

	--dll->objs_count;
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
	if (__dll_unlikely(!del_obj))
		return false;
	return dll_freeobj(del_obj);
}

static inline size_t	dll_deln(dll_t *restrict dll, size_t start, size_t n) {
	dll_obj_t *restrict	iobj = dll_findid(dll, start);
	if (!iobj)
		return 0;

	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	size_t	i = 0;
	for (; iobj && n; iobj = dll_findid(dll, start)) {
		if (__dll_unlikely(!dll_del(dll, iobj) && is_not_ign_err))
			return 0;
		--n;
		++i;
	}
	return i;
}

static inline bool	dll_delkey(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del, void *restrict any_data) {
	dll_obj_t *restrict del = dll_findkey(dll, fn_search_del, any_data);
	if (__dll_unlikely(!del))
		return false;
	bool ret = dll_del(dll, del);
	if (!dll->head || !dll->last) {
		__dll_seterrno(__DLL_EEMPTY);
		dll->head = dll->last = NULL;
	}
	return ret;
}

static inline bool	dll_delkeyr(dll_t *restrict dll,
		f_dll_obj_handler fn_search_del, void *restrict any_data) {
	dll_obj_t *restrict del = dll_findkeyr(dll, fn_search_del, any_data);
	if (__dll_unlikely(!del))
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
	if (__dll_unlikely(!del))
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
	if (__dll_unlikely(!del))
		return false;
	bool ret = dll_del(dll, del);
	if (!dll->head || !dll->last) {
		__dll_seterrno(__DLL_EEMPTY);
		dll->head = dll->last = NULL;
	}
	return ret;
}

static inline bool	dll_free(dll_t *restrict dll) {
	if (__dll_unlikely(!dll || !dll->head)) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
		if (__dll_likely(!!dll))
			free(dll);
		return false;
	}
	while (dll_popfront(dll))
		;
	free(dll);
	return true;
}

static inline bool	dll_freeobj(dll_obj_t *restrict dll_obj) {
	if (__dll_likely(!dll_obj)) {
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
