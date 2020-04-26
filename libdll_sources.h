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

# include <stdint.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

# include "libdll_bits.h"
# include "libdll_internal.h"

static inline dll_t	*dll_init(dll_bits_t bits) {
	dll_t *restrict	out;
	__dll_assert_errno(out = calloc(1, sizeof(*out)), __DLL_EALLOC);
	out->bits = bits;
	return out;
}

static inline dll_obj_t	*dll_new(void *restrict data,
		size_t size,
		dll_bits_t obj_bits,
		dll_obj_free_fn_t fn_free) {
	dll_obj_t *restrict out;
	bool is_not_ign_err = !__dll_is_bit(obj_bits, DLL_BIT_EIGN);
	bool is_memdup = __dll_is_bit(obj_bits, DLL_BIT_DUP);

	__dll_assert_errno(out = calloc(1, sizeof(*out)), __DLL_EALLOC);
	if (is_not_ign_err) {
		__dll_assert_errno(data, __DLL_ENULL);
		__dll_assert_errno(size, __DLL_ENULL);
	}
	if (__dll_unlikely(is_memdup)) {
		__dll_assert_errno(out->data = calloc(1, size), __DLL_EDUP);
		out->data = memcpy(out->data, data, size);
	} else {
		out->data = data;
	}

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
	if (!new_obj)
		return NULL;
	return dll_pushfrontobj(dll, new_obj);
}

static inline dll_obj_t	*dll_pushfrontobj(dll_t *restrict dll,
		dll_obj_t *restrict obj) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_assert_errno(obj, __DLL_ENULL);

	++dll->objs_count;
	if (!dll->head) {
		dll->head = dll->last = obj;
	} else {
		dll->head->prev = obj;
		obj->next = dll->head;
		dll->head = obj;
	}
	return obj;
}

static inline dll_obj_t	*dll_pushback(dll_t *restrict dll,
		void *restrict data,
		size_t data_size,
		dll_bits_t obj_bits,
		dll_obj_free_fn_t fn_free) {
	dll_obj_t *restrict new_obj = dll_new(data, data_size, obj_bits, fn_free);
	if (!new_obj)
		return NULL;
	return dll_pushbackobj(dll, new_obj);
}

static inline dll_obj_t	*dll_pushbackobj(dll_t *restrict dll,
		dll_obj_t *restrict obj) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_assert_errno(obj, __DLL_ENULL);

	++dll->objs_count;
	if (!dll->head) {
		dll->head = obj;
		dll->last = obj;
	} else {
		dll->last->next = obj;
		obj->prev = dll->last;
		dll->last = obj;
	}
	return obj;
}

static inline bool	dll_popfront(dll_t *restrict dll) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_assert_errno(dll->head, __DLL_EEMPTY);

	dll_obj_t *restrict save = dll->head->next;
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely(!dll_del(dll, dll->head) && is_not_ign_err)) {
		return false;
	}
	dll->head = save;
	return true;
}

static inline bool	dll_popback(dll_t *restrict dll) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_assert_errno(dll->last, __DLL_EEMPTY);

	dll_obj_t *restrict save = dll->last->prev;
	bool is_not_ign_err = !__dll_is_bit(dll->bits, DLL_BIT_EIGN);
	if (__dll_unlikely(!dll_del(dll, dll->last) && is_not_ign_err)) {
		return false;
	}
	dll->last = save;
	return true;
}

static inline size_t	dll_getsize(const dll_t *restrict dll) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_assert_errno(dll->objs_count, __DLL_EEMPTY);
	return dll->objs_count;
}
static inline dll_obj_t	*dll_gethead(const dll_t *restrict dll) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_assert_errno(dll->head, __DLL_EEMPTY);
	return dll->head;
}
static inline dll_obj_t	*dll_getlast(const dll_t *restrict dll) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_assert_errno(dll->last, __DLL_EEMPTY);
	return dll->last;
}
static inline bool	dll_empty(const dll_t *restrict dll) {
	bool	isempty = true;
	if (dll && dll->objs_count)
		isempty = false;
	return isempty;
}

static inline void	*dll_getdata(const dll_obj_t *restrict obj) {
	__dll_assert_errno(obj, __DLL_ENULL);
	__dll_assert_errno(obj->data, __DLL_EEMPTY_OBJ);
	return obj->data;
}
static inline size_t	dll_getdatasize(const dll_obj_t *restrict obj) {
	__dll_assert_errno(obj, __DLL_ENULL);
	__dll_assert_errno(obj->data_size, __DLL_EEMPTY_OBJ);
	return obj->data_size;
}
static inline dll_obj_t	*dll_getprev(const dll_obj_t *restrict obj) {
	__dll_assert_errno(obj, __DLL_ENULL);
	return obj->prev;
}
static inline dll_obj_t	*dll_getnext(const dll_obj_t *restrict obj) {
	__dll_assert_errno(obj, __DLL_ENULL);
	return obj->next;
}

static inline size_t	dll_getid(dll_t *restrict dll,
		const dll_obj_t *restrict obj) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_assert_errno(obj, __DLL_ENULL);

	__dll_internal_retval	retval = __dll_internal_base(dll,
		dll_fnptr_ptrobj, obj->data, 1, dll->objs_count, __dlli_sdir_begin, true);
	return retval.idx;
}

static inline bool	dll_foreach(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_iter,
		void *restrict ptr) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_iter, ptr, 1, dll->objs_count, __dlli_sdir_bd, false);
	return retval.status;
}

static inline bool	dll_foreachn(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_iter,
		void *restrict ptr,
		size_t start,
		size_t n) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_iter, ptr, start, n, __dlli_sdir_bd, false);
	return retval.status;
}

static inline dll_obj_t	*dll_find(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search,
		void *restrict ptr) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_search, ptr, 1, dll->objs_count, __dlli_sdir_begin, true);
	if (__dll_likely(__dlli_status_match == retval.status))
		return retval.obj;
	else
		return NULL;
}
static inline dll_obj_t	*dll_findr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search,
		void *restrict ptr) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_search, ptr, 1, dll->objs_count, __dlli_sdir_end, true);
	if (__dll_likely(__dlli_status_match == retval.status))
		return retval.obj;
	else
		return NULL;
}
static inline dll_obj_t	*dll_findn(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search,
		void *restrict ptr,
		size_t start,
		size_t n) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_search, ptr, start, n, __dlli_sdir_begin, true);
	if (__dll_likely(__dlli_status_match == retval.status))
		return retval.obj;
	else
		return NULL;
}

static inline dll_obj_t	*dll_findid(dll_t *restrict dll, size_t index) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_internal_retval	retval = __dll_internal_base(dll,
		dll_fnptr_ptridx, &index, 1, dll->objs_count, __dlli_sdir_begin, true);
	if (__dll_likely(__dlli_status_match == retval.status))
		return retval.obj;
	else
		return NULL;
}

static inline dll_obj_t	*dll_findidr(dll_t *restrict dll, size_t index) {
	__dll_assert_errno(dll, __DLL_ENULL);
	size_t	real_end_idx = dll->objs_count - index + 1;
	__dll_internal_retval	retval = __dll_internal_base(dll,
		dll_fnptr_ptridx, &real_end_idx, 1, dll->objs_count, __dlli_sdir_end, true);
	if (__dll_likely(__dlli_status_match == retval.status))
		return retval.obj;
	else
		return NULL;
}

static inline ssize_t	dll_printone(dll_obj_t *restrict obj,
		dll_obj_handler_fn_t fn_print) {
	__dll_assert_errno(obj, __DLL_ENULL);

	bool is_not_ign_err = !__dll_is_bit(obj->bits, DLL_BIT_EIGN);
	if (is_not_ign_err) {
		__dll_assert_errno(fn_print, __DLL_EHANDLER_NULL);
		__dll_assert_errno(obj->data, __DLL_EEMPTY_OBJ);
	}

	size_t	obj_idx = 1;
	for (dll_obj_t *iobj = obj; iobj && iobj->prev; iobj = iobj->prev)
		++obj_idx;

	int ret = fn_print(obj->data, NULL, obj_idx);
	if (__dll_unlikely(is_not_ign_err && 0 > ret))
		__dll_seterrno(__DLL_EHANDLER_NEG);
	return ret;

}

static inline bool	dll_print(dll_t *restrict dll, dll_obj_handler_fn_t fn_print) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_print, NULL, 1, dll->objs_count, __dlli_sdir_begin, false);
	return retval.status;
}

static inline bool	dll_printr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_print) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_print, NULL, 1, dll->objs_count, __dlli_sdir_end, false);
	return retval.status;
}

static inline size_t	dll_printn(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_print,
		size_t start,
		size_t n) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_print, NULL, start, n, __dlli_sdir_begin, false);
	return retval.status;
}

static inline size_t	dll_printnr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_print,
		size_t start,
		size_t n) {
	__dll_internal_retval	retval = __dll_internal_base(dll,
		fn_print, NULL, start, n, __dlli_sdir_end, false);
	return retval.status;
}

static inline bool	dll_swap(dll_obj_t *restrict a, dll_obj_t *restrict b) {
	__dll_assert_errno(a, __DLL_ENULL);
	__dll_assert_errno(b, __DLL_ENULL);

	dll_obj_t c = *a;
	__dll_internal_memcpy(a, b);
	__dll_internal_memcpy(b, &c);
	return true;
}

static inline bool	dll_swapdll(dll_t *restrict a, dll_t *restrict b) {
	__dll_assert_errno(a, __DLL_ENULL);
	__dll_assert_errno(b, __DLL_ENULL);


	bool is_not_ign_err = (!__dll_is_bit(a->bits, DLL_BIT_EIGN)
						|| !__dll_is_bit(b->bits, DLL_BIT_EIGN));
	if (is_not_ign_err) {
		__dll_assert_errno(a->objs_count, __DLL_EEMPTY);
		__dll_assert_errno(b->objs_count, __DLL_EEMPTY);
	}
	dll_obj_t *restrict	ahead = a->head;
	dll_obj_t *restrict	bhead = b->head;
	for (; ahead && bhead; ahead = ahead->next, bhead = bhead->next) {
		dll_obj_t acopy = *ahead;
		__dll_internal_memcpy(ahead, bhead);
		__dll_internal_memcpy(bhead, &acopy);
	}

	struct __internal_unfinished_swap {
		dll_obj_t *restrict	tail;
		dll_t *restrict	unfinished;
		dll_t *restrict	trimm;
	} __uswap = { NULL, NULL, NULL };

	if (ahead) {
		__uswap = (struct __internal_unfinished_swap) { ahead, b, a };
	} else if (bhead) {
		__uswap = (struct __internal_unfinished_swap) { bhead, a, b };
	}
	for (; __uswap.tail; __uswap.tail = __uswap.tail->next) {
		dll_unlink(__uswap.trimm, __uswap.tail);
		dll_pushbackobj(__uswap.unfinished, __uswap.tail);
	}
	return true;
}

static inline dll_t	*dll_reverse(dll_t *restrict dll) {
	__dll_assert_errno(dll, __DLL_ENULL);

	for (dll_obj_t *restrict ihead = dll->head, *restrict ilast = dll->last;
	ihead != ilast;
	ihead = ihead->next, ilast = ilast->prev)
		dll_swap(ihead, ilast);
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
		if (__dll_unlikely(is_not_ign_err))
			__dll_assert_errno(iobj->data, __DLL_EEMPTY_OBJ);

		int	ret = fn_match(iobj->data, ptr, idx++);
		if (__dll_likely(!ret)) {
			dll_bits_t	new_bits = (iobj->bits & ~(DLL_BIT_DUP | DLL_BIT_EIGN));
			dll_obj_t *restrict	new_obj_ptr = dll_pushback(dst,
				iobj->data, iobj->data_size, new_bits, NULL);
			if (__dll_unlikely(is_not_ign_err && !new_obj_ptr))
				return NULL;
		} else if (__dll_unlikely(is_not_ign_err && 0 > ret)) {
			__dll_seterrno(__DLL_EHANDLER_NEG);
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

	dll_t *restrict	new_list;
	__dll_assert_errno(new_list = calloc(1, sizeof(*new_list)), __DLL_EALLOC);

	size_t	idx = start;
	for (dll_obj_t *restrict iobj = dll_findid(dll, start); iobj && n--; iobj = iobj->next) {
		bool is_not_ign_err = !__dll_is_bit(iobj->bits, DLL_BIT_EIGN);
		if (__dll_unlikely(is_not_ign_err && !iobj->data)) {
			__dll_seterrno(__DLL_EEMPTY_OBJ);
			dll_free(&new_list);
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
			__dll_seterrno(__DLL_EHANDLER_NEG);
			dll_free(&new_list);
			return NULL;
		}
	}
	if (__dll_unlikely(!new_list->objs_count))
		dll_free(&new_list);
	return new_list;
}

static inline dll_obj_t	*dll_unlink(dll_t *restrict dll,
		dll_obj_t *restrict obj) {
	__dll_assert_errno(dll, __DLL_ENULL);
	__dll_assert_errno(obj, __DLL_ENULL);

	__dll_internal_status	status = __dll_internal_bdcycle(dll, dll_fnptr_ptrobj, obj->data, true).status;
	if (__dlli_status_match != status)
		return obj;

	--dll->objs_count;
	if (obj->prev)
		obj->prev->next = obj->next;
	else
		dll->head = obj->next;
	if (obj->next)
		obj->next->prev = obj->prev;
	else
		dll->last = obj->prev;
	return obj;
}

static inline bool	dll_del(dll_t *restrict dll, dll_obj_t *restrict obj) {
	dll_obj_t *restrict	del_obj = dll_unlink(dll, obj);
	if (!del_obj)
		return NULL;
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
	dll_obj_t *restrict del = dll_find(dll, fn_search_del, ptr);
	if (!del)
		return NULL;
	return dll_del(dll, del);
}

static inline bool	dll_delkeyr(dll_t *restrict dll,
		dll_obj_handler_fn_t fn_search_del,
		void *restrict ptr) {
	dll_obj_t *restrict del = dll_findr(dll, fn_search_del, ptr);
	if (!del)
		return NULL;
	return dll_del(dll, del);
}

static inline bool	dll_delid(dll_t *restrict dll, size_t index) {
	dll_obj_t *restrict del = dll_findid(dll, index);
	if (!del)
		return NULL;
	return dll_del(dll, del);
}

static inline bool	dll_delidr(dll_t *restrict dll, size_t index) {
	dll_obj_t *restrict del = dll_findidr(dll, index);
	if (!del)
		return NULL;
	return dll_del(dll, del);
}

static inline bool	dll_free(dll_t *restrict *restrict dll) {
	__dll_assert_errno(*dll, __DLL_ENULL);

	while (dll_popfront(*dll))
		;
	free(*dll);
	*dll = NULL;
	return true;
}

static inline bool	dll_freeobj(dll_obj_t *restrict *restrict obj) {
	if (!dll_freeobjdata(obj))
		return false;
	free(*obj);
	*obj = NULL;
	return true;
}

static inline bool	dll_freeobjdata(dll_obj_t *restrict *restrict obj) {
	__dll_assert_errno(*obj, __DLL_ENULL);
	if ((*obj)->free) {
		(*obj)->free((*obj)->data);
	} else if (__dll_is_bit((*obj)->bits, DLL_BIT_DUP)
		|| __dll_is_bit((*obj)->bits, DLL_BIT_FREE)) {
		free((*obj)->data);
	}
	return true;
}

static ssize_t	dll_fnptr_ptrobj(void *restrict obj, void *restrict ptr, size_t idx) {
	(void)idx;
	return !(ptr == obj);
}
static ssize_t	dll_fnptr_ptridx(void *restrict obj, void *restrict ptr, size_t idx) {
	(void)obj;
	return !(*((size_t*)ptr) == idx);
};
static ssize_t	dll_fnptr_any(void *restrict obj, void *restrict ptr, size_t idx) {
	(void)obj; (void)ptr; (void)idx;
	return 0;
}

#endif /* LIBDLL_SOURCES_H */
