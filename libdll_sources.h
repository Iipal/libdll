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
# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>
# include <sys/cdefs.h>

# include "libdll_structs.h"
# include "libdll_bits.h"
# include "libdll_errno.h"

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


static inline bool	__dll_internal_errno(dll_t *restrict dll_srcptr,
		dll_t *restrict dll_dstptr,
		dll_obj_handler_fn_t fnptr,
		size_t start) {
	if (__dll_unlikely(!dll_srcptr || !dll_dstptr)) {
		__dll_seterrno(__DLL_ENULL);
		return false;
	}
	bool is_not_ign_err = (!__dll_is_bit(dll_srcptr->bits, DLL_BIT_EIGN)
						|| !__dll_is_bit(dll_dstptr->bits, DLL_BIT_EIGN));
	if (__dll_unlikely(is_not_ign_err)) {
		int	__errcode = __DLL_ESUCCESS;

		if (!fnptr) {
			__errcode = __DLL_ENOHANDLER;
		} else if (!dll_srcptr->head) {
			__errcode = __DLL_EEMPTY;
		} else if (!start || start > dll_srcptr->objs_count) {
			__errcode = __DLL_EOUTOFRANGE;
		}

		if (__DLL_ESUCCESS != __errcode) {
			__dll_seterrno(__errcode);
			return false;
		}
	}
	return true;
}

static inline void	__dll_internal_objcopy(dll_obj_t *restrict a, dll_obj_t *restrict b) {
	u_char *restrict	aptr = (u_char *restrict)a;
	u_char *restrict	bptr = (u_char *restrict)b;
	const size_t	offsetnp = offsetof(dll_obj_t, data);
	const size_t	sizecopy = sizeof(*a) - offsetnp;

	memcpy(aptr + offsetnp, bptr + offsetnp, sizecopy);
}

typedef enum {
	__dll_begin_dir,
	__dll_end_dir,
} __attribute__((packed)) __dll_internal_search_direction;

typedef struct {
	dll_obj_t *restrict	obj;
	size_t	idx;
	bool	is_valid;
} __dll_internal_retval;

static inline __dll_internal_retval	__dll_internal_base(dll_t *restrict dll,
		dll_obj_handler_fn_t fnptr,
		void *restrict ptr,
		size_t start,
		size_t n,
		__dll_internal_search_direction dir,
		bool is_success_ret) {
	__dll_internal_retval	retval = { NULL, 0, true };

	if (!__dll_internal_errno(dll, dll, fnptr, start)) {
		retval.is_valid = false;
		return retval;
	}

	dll_obj_t *restrict	iobj;
	size_t	idx;

	if (__dll_begin_dir == dir) {
		iobj = dll->head;
		idx = 1;
		if (1 != start)
			while (iobj && idx != start) {
				iobj = iobj->next;
				++idx;
			}
	} else /* if (__dll_end_dir == (_dir)) */ {
		iobj = dll->last;
		idx = dll->objs_count;
		if (dll->objs_count != start) {
			while (iobj && idx != start) {
				iobj = iobj->prev;
				--idx;
			}
		}
	}

	while (iobj && n--) {
		retval.idx = idx;
		bool is_not_ign_err = !__dll_is_bit(iobj->bits, DLL_BIT_EIGN);
		if (__dll_unlikely(is_not_ign_err && !iobj->data)) {
			retval.is_valid = __dll_seterrno(__DLL_EEMPTY_OBJ);
			return retval;
		}

		int ret = fnptr(iobj->data, ptr, idx);
		if (__dll_likely(!ret)) {
			retval.obj = iobj;
			if (is_success_ret) {
				return retval;
			}
		} else if (__dll_unlikely(is_not_ign_err && 0 > ret)) {
			retval.is_valid = __dll_seterrno(__DLL_ENEGHANDLER);
			return retval;
		}

		if (__dll_begin_dir == (dir)) {
			iobj = iobj->next;
			++idx;
		} else /* if (__dll_end_dir == (dir)) */ {
			iobj = iobj->prev;
			--idx;
		}
	}

	return retval;
}

static inline dll_t	*dll_init(dll_bits_t bits) {
	dll_t *restrict out;
	if (__dll_unlikely(!(out = calloc(1, sizeof(*out))))) {
		__dll_seterrno(__DLL_ECALLOC);
		return NULL;
	}
	out->bits = bits;
	return out;
}

static inline dll_obj_t	*dll_new(void *restrict data,
		size_t size,
		dll_bits_t obj_bits,
		dll_obj_free_fn_t fn_free) {
	bool is_not_ign = !__dll_is_bit(obj_bits, DLL_BIT_EIGN);
	if (__dll_unlikely(is_not_ign && (!data || !size))) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}

	dll_obj_t *restrict out;
	if (!(out = calloc(1, sizeof(*out)))) {
		__dll_seterrno(__DLL_ECALLOC);
		return NULL;
	}

	bool is_duplicate_memory = __dll_is_bit(obj_bits, DLL_BIT_DUP);
	if (__dll_unlikely(is_duplicate_memory && !(out->data = calloc(1, size)))) {
		__dll_seterrno(__DLL_EDUP);
		free(out);
		return NULL;
	}
	out->data = is_duplicate_memory ? memcpy(out->data, data, size) : data;
	out->data_size = size;
	out->free = fn_free;
	out->bits = obj_bits;
	return out;
}

static inline dll_obj_t	*dll_pushfront(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		dll_obj_free_fn_t fn_free) {
	dll_obj_t *restrict new_obj = dll_new(data, data_size, obj_bits, fn_free);
	return dll_pushfrontobj(dll, new_obj);
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
		dll_obj_free_fn_t fn_free) {
	dll_obj_t *restrict new_obj = dll_new(data, data_size, obj_bits, fn_free);
	return dll_pushbackobj(dll, new_obj);
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
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely(!dll_del(dll, dll->head) && is_not_ign_err)) {
		return false;
	}
	dll->head = save;
	return true;
}

static inline bool	dll_popback(dll_t *restrict dll) {
	if (__dll_unlikely(!dll || !dll->last)) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
		return false;
	}

	dll_obj_t *restrict save = dll->last->prev;
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely(!dll_del(dll, dll->last) && is_not_ign_err)) {
		return false;
	}
	dll->last = save;
	return true;
}

static inline size_t	dll_getsize(const dll_t *restrict dll) {
	size_t	objs_count = 0;
	if (__dll_unlikely(!dll || !dll->objs_count)) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
	} else {
		objs_count = dll->objs_count;
	}
	return objs_count;
}
static inline dll_obj_t	*dll_gethead(const dll_t *restrict dll) {
	dll_obj_t *restrict head = NULL;
	if (__dll_unlikely(!dll || !dll->head)) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
	} else {
		head = dll->head;
	}
	return head;
}
static inline dll_obj_t	*dll_getlast(const dll_t *restrict dll) {
	dll_obj_t *restrict last = NULL;
	if (__dll_unlikely(!dll || !dll->last)) {
		__dll_seterrno(!dll ? __DLL_ENULL : __DLL_EEMPTY);
	} else {
		last = dll->last;
	}
	return last;
}
static inline bool	dll_empty(const dll_t *restrict dll) {
	bool	isempty = true;
	if (dll && dll->objs_count)
		isempty = false;
	return isempty;
}

static inline void	*dll_getdata(const dll_obj_t *restrict dll_obj) {
	void *restrict	data = NULL;
	if (__dll_unlikely(!dll_obj || !dll_obj->data)) {
		__dll_seterrno(!dll_obj ? __DLL_ENULL : __DLL_EEMPTY_OBJ);
	} else {
		data = dll_obj->data;
	}
	return data;
}
static inline size_t	dll_getdatasize(const dll_obj_t *restrict dll_obj) {
	size_t	data_size = 0;
	if (__dll_unlikely(!dll_obj || !dll_obj->data_size)) {
		__dll_seterrno(!dll_obj ? __DLL_ENULL : __DLL_EEMPTY_OBJ);
	} else {
		data_size = dll_obj->data_size;
	}
	return data_size;
}
static inline dll_obj_t	*dll_getprev(const dll_obj_t *restrict dll_obj) {
	dll_obj_t *restrict prev = NULL;
	if (__dll_unlikely(!dll_obj)) {
		__dll_seterrno(__DLL_ENULL);
	} else {
		prev = dll_obj->prev;
	}
	return prev;
}
static inline dll_obj_t	*dll_getnext(const dll_obj_t *restrict dll_obj) {
	dll_obj_t *restrict next = NULL;
	if (__dll_unlikely(!dll_obj)) {
		__dll_seterrno(__DLL_ENULL);
	} else {
		next = dll_obj->next;
	}
	return next;
}

static inline size_t	dll_getid(dll_t *restrict dll,
		const dll_obj_t *restrict dll_obj) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		dll_fnptr_ptrobj, dll_obj->data, 1, dll->objs_count, __dll_begin_dir, true);
	return retval.idx;
}

static inline bool	dll_foreachn(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_iter,
		size_t start,
		size_t n) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_iter, NULL, start, n, __dll_begin_dir, false);
	return retval.is_valid;
}

static inline bool	dll_foreach(dll_t *restrict dll, dll_obj_handler_fn_t fn_iter) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_iter, NULL, 1, dll->objs_count, __dll_begin_dir, false);
	return retval.is_valid;
}

static inline dll_obj_t	*dll_findn(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_match,
		void *restrict ptr,
		size_t start,
		size_t n) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_match, ptr, start, n, __dll_begin_dir, true);
	return retval.obj;
}

static inline dll_obj_t	*dll_find(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_match,
		void *restrict ptr) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_match, ptr, 1, dll->objs_count, __dll_begin_dir, true);
	return retval.obj;
}

static inline dll_obj_t	*dll_findkey(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search,
		void *restrict ptr) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_search, ptr, 1, dll->objs_count, __dll_begin_dir, true);
	return retval.obj;
}

static inline dll_obj_t	*dll_findkeyr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search,
		void *restrict ptr) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_search, ptr, 1, dll->objs_count, __dll_end_dir, true);
	return retval.obj;
}

static inline dll_obj_t	*dll_findid(dll_t *restrict dll, size_t index) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		dll_fnptr_ptridx, &index, 1, dll->objs_count, __dll_begin_dir, true);
	return retval.obj;
}

static inline dll_obj_t	*dll_findidr(dll_t *restrict dll, size_t index) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		dll_fnptr_ptridx, &index, 1, dll->objs_count, __dll_end_dir, true);
	return retval.obj;
}

static inline int	dll_printone(dll_obj_t *restrict dll_obj,
		dll_obj_handler_fn_t fn_print) {
	if (__dll_unlikely(!dll_obj)) {
		__dll_seterrno(__DLL_ENULL);
		return -1;
	}

	bool is_not_ign_err = !__dll_is_bit(dll_obj->bits, DLL_BIT_EIGN);
	if (__dll_unlikely(is_not_ign_err && (!fn_print || !dll_obj->data))) {
		__dll_seterrno(!fn_print ? __DLL_ENOHANDLER : __DLL_EEMPTY_OBJ);
		return -1;
	}

	size_t	obj_idx = 1;
	for (dll_obj_t *iobj = dll_obj; iobj && iobj->prev; iobj = iobj->prev)
		++obj_idx;

	int ret = fn_print(dll_obj->data, NULL, obj_idx);
	if (__dll_unlikely(is_not_ign_err && 0 > ret))
		__dll_seterrno(__DLL_ENEGHANDLER);
	return ret;

}

static inline bool	dll_print(dll_t *restrict dll, dll_obj_handler_fn_t fn_print) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_print, NULL, 1, dll->objs_count, __dll_begin_dir, false);
	return retval.is_valid;
}

static inline bool	dll_printr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_print) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_print, NULL, 1, dll->objs_count, __dll_end_dir, false);
	return retval.is_valid;
}

static inline size_t	dll_printn(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_print,
		size_t start,
		size_t n) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_print, NULL, start, n, __dll_begin_dir, false);
	return retval.is_valid;
}

static inline size_t	dll_printnr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_print,
		size_t start,
		size_t n) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_print, NULL, start, n, __dll_end_dir, false);
	return retval.is_valid;
}

static inline bool	dll_swap(dll_obj_t *restrict a, dll_obj_t *restrict b) {
	if (__dll_unlikely(!a || !b)) {
		__dll_seterrno(__DLL_ENULL);
		return false;
	}

	dll_obj_t c = *a;
	__dll_internal_objcopy(a, b);
	__dll_internal_objcopy(b, &c);
	return true;
}

static inline bool	dll_swapdll(dll_t *restrict a, dll_t *restrict b) {
	if (__dll_unlikely(!a || !b)) {
		__dll_seterrno(__DLL_ENULL);
		return false;
	}
	bool is_not_ign_err = (!__dll_is_bit(a->bits, DLL_BIT_EIGN)
						|| !__dll_is_bit(b->bits, DLL_BIT_EIGN));
	if (__dll_unlikely(is_not_ign_err && (!a->head || !b->head))) {
		__dll_seterrno(__DLL_EEMPTY);
		return false;
	}
	dll_obj_t *restrict	ahead = a->head;
	dll_obj_t *restrict	bhead = b->head;
	for (; ahead && bhead; ahead = ahead->next, bhead = bhead->next)
		dll_swap(ahead, bhead);

	struct __internal_unfinished_swap {
		dll_obj_t *restrict	tail;
		dll_t *restrict	unfinished;
		dll_t *restrict	trimm;
	} __uswap = { NULL, NULL, NULL };

	if (ahead)
		__uswap = (struct __internal_unfinished_swap) { ahead, b, a };
	else if (bhead)
		__uswap = (struct __internal_unfinished_swap) { bhead, a, b };
	for (; __uswap.tail; __uswap.tail = __uswap.tail->next) {
		dll_unlink(__uswap.trimm, __uswap.tail);
		dll_pushbackobj(__uswap.unfinished, __uswap.tail);
	}
	return true;
}

static inline dll_t	*dll_reverse(dll_t *restrict dll) {
	if (__dll_unlikely(!dll)) {
		__dll_seterrno(__DLL_ENULL);
		return NULL;
	}

	for (dll_obj_t *restrict ihead = dll->head, *restrict ilast = dll->last;
	ihead != ilast;
	ihead = ihead->next, ilast = ilast->prev) {
		dll_swap(ihead, ilast);
	}
	return dll;
}

static inline dll_t	*dll_appenddll(dll_t *restrict src,
		dll_t *restrict dst,
		dll_obj_handler_fn_t fn_match,
		void *restrict ptr,
		size_t start,
		size_t n) {
	if (!__dll_internal_errno(dst, src, fn_match, start))
		return false;

	size_t	idx = start;
	for (dll_obj_t *restrict iobj = dll_findid(src, start); iobj && n--; iobj = iobj->next) {
		bool is_not_ign_err = !__dll_is_bit(iobj->bits, DLL_BIT_EIGN);
		if (__dll_unlikely(is_not_ign_err && !iobj->data)) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			return NULL;
		}

		int	ret = fn_match(iobj->data, ptr, idx++);
		if (__dll_likely(!ret)) {
			dll_bits_t	new_ibts = (iobj->bits & ~(DLL_BIT_DUP | DLL_BIT_EIGN));
			dll_obj_t *restrict	new_obj_ptr = dll_pushback(dst,
				iobj->data, iobj->data_size, new_ibts, NULL);
			if (__dll_unlikely(is_not_ign_err && !new_obj_ptr))
				return NULL;
		} else if (__dll_unlikely(is_not_ign_err && 0 > ret)) {
			__dll_seterrno(__DLL_ENEGHANDLER);
			return NULL;
		}
	}
	return dst;
}

static inline dll_t	*dll_dup(dll_t *restrict dll, size_t start, size_t n) {
	dll_t *restrict	new_list = dll_dupkey(dll, dll_fnptr_any, NULL, start, n);
	return new_list;
}

static inline dll_t	*dll_dupkey(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_match,
		void *restrict ptr,
		size_t start,
		size_t n) {
	if (!__dll_internal_errno(dll, dll, fn_match, start))
		return false;

	dll_t *restrict	new_list = dll_init(dll->bits);
	if (__dll_unlikely(!new_list))
		return NULL;

	size_t	idx = start;
	for (dll_obj_t *restrict iobj = dll_findid(dll, start); iobj && n--; iobj = iobj->next) {
		bool is_not_ign_err = !__dll_is_bit(iobj->bits, DLL_BIT_EIGN);
		if (__dll_unlikely(is_not_ign_err && !iobj->data)) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			return NULL;
		}

		int	ret = fn_match(iobj->data, ptr, idx++);
		if (__dll_likely(!ret)) {
			dll_bits_t	new_bits = (iobj->bits & ~(DLL_BIT_DUP | DLL_BIT_EIGN));
			dll_obj_t *restrict	new_obj_ptr = dll_pushback(new_list,
				iobj->data, iobj->data_size, new_bits, NULL);
			if (__dll_unlikely(is_not_ign_err && !new_obj_ptr)) {
				dll_free(&new_list);
				return NULL;
			}
		} else if (__dll_unlikely(is_not_ign_err && 0 > ret)) {
			__dll_seterrno(__DLL_ENEGHANDLER);
			dll_free(&new_list);
			return NULL;
		}
	}
	return new_list;
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
	else
		dll->last = dll_obj->prev;
	return dll_obj;
}

static inline bool	dll_del(dll_t *restrict dll, dll_obj_t *restrict dll_obj) {
	dll_obj_t *restrict	del_obj = dll_unlink(dll, dll_obj);
	return dll_freeobj(&del_obj);
}

static inline size_t	dll_deln(dll_t *restrict dll, size_t start, size_t n) {
	if (!__dll_internal_errno(dll, dll, dll_fnptr_any, start))
		return 0;

	size_t	i = 0;
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	for (dll_obj_t *restrict iobj = dll_findid(dll, start);
	iobj && n--;
	iobj = dll_findid(dll, start)) {
		++i;
		if (__dll_unlikely(!dll_del(dll, iobj) && is_not_ign_err))
			return i;
	}
	return i;
}

static inline bool	dll_delkey(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search_del,
		void *restrict ptr) {
	dll_obj_t *restrict del = dll_findkey(dll, fn_search_del, ptr);
	return dll_del(dll, del);
}

static inline bool	dll_delkeyr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search_del,
		void *restrict ptr) {
	dll_obj_t *restrict del = dll_findkeyr(dll, fn_search_del, ptr);
	return dll_del(dll, del);
}

static inline bool	dll_delid(dll_t *restrict dll, size_t index) {
	dll_obj_t *restrict del = dll_findid(dll, index);
	return dll_del(dll, del);
}

static inline bool	dll_delidr(dll_t *restrict dll, size_t index) {
	dll_obj_t *restrict del = dll_findidr(dll, index);
	return dll_del(dll, del);
}

static inline bool	dll_free(dll_t *restrict *restrict dll) {
	bool	ret = true;
	if (__dll_unlikely(!*dll || !(*dll)->head)) {
		__dll_seterrno(!*dll ? __DLL_ENULL : __DLL_EEMPTY);
		ret = false;
	}

	while (dll_popfront(*dll))
		;
	free(*dll);
	*dll = NULL;
	return ret;
}

static inline bool	dll_freeobj(dll_obj_t *restrict *restrict dll_obj) {
	if (!dll_freeobjdata(dll_obj))
		return false;
	free(*dll_obj);
	*dll_obj = NULL;
	return true;
}

static inline bool	dll_freeobjdata(dll_obj_t *restrict *restrict dll_obj) {
	if (__dll_unlikely(!*dll_obj)) {
		__dll_seterrno(__DLL_ENULL);
		return false;
	}
	if ((*dll_obj)->free) {
		(*dll_obj)->free((*dll_obj)->data);
	} else if (__dll_is_bit((*dll_obj)->bits, DLL_BIT_DUP)
		|| __dll_is_bit((*dll_obj)->bits, DLL_BIT_FREE)) {
		free((*dll_obj)->data);
	}
	return true;
}

static int	dll_fnptr_ptrobj(void *restrict obj, void *restrict ptr, size_t idx) {
	(void)idx;
	return !(ptr == obj);
}
static int	dll_fnptr_ptridx(void *restrict obj, void *restrict ptr, size_t idx) {
	(void)obj;
	return !(*((size_t*)ptr) == idx);
};
static int	dll_fnptr_any(void *restrict obj, void *restrict ptr, size_t idx) {
	(void)obj; (void)ptr; (void)idx;
	return 0;
}

#endif /* LIBDLL_SOURCES_H */
