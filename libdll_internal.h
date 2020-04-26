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

#ifndef LIBDLL_INTERNAL_H
# define LIBDLL_INTERNAL_H

# ifndef LIBDLL_SOURCES_H
#  error "include only libdll.h"
# endif /* LIBDLL_SOURCES_H */

# include <stdbool.h>
# include <sys/cdefs.h>

# include "libdll_errno.h"
# include "libdll_structs.h"

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

/*****************************
 * Internal structs and types:
 ****************************/
typedef enum {
	__dlli_sdir_begin,
	__dlli_sdir_end,
	__dlli_sdir_bd /* bidirectional search, from begin and end at once */
} __attribute__((packed)) __dll_internal_search_direction;

typedef enum {
	__dlli_status_invalid,
	__dlli_status_valid,
	__dlli_status_match
} __attribute__((packed)) __dll_internal_status;

typedef struct {
	dll_obj_t *restrict	obj;
	size_t	idx;
	__dll_internal_status	status;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__))) __dll_internal_retval;

/**********************
 * Internal prototypes:
 *********************/
static inline bool	__dll_internal_errno(dll_t *restrict dll_srcptr,
		dll_t *restrict dll_dstptr, dll_obj_handler_fn_t fnptr, size_t start);

static inline void	__dll_internal_memcpy(dll_obj_t *restrict a, dll_obj_t *restrict b);
static inline void	__dll_internal_bzero(dll_obj_t *restrict a);

static inline __dll_internal_status	__dll_internal_basecycle(__dll_internal_retval *restrict rv,
		dll_obj_t *restrict obj, dll_obj_handler_fn_t fnptr, void *restrict ptr, bool is_success_ret);
static inline __dll_internal_retval	__dll_internal_bdcycle(dll_t *restrict dll,
		dll_obj_handler_fn_t fnptr, void *restrict ptr, bool is_success_ret);
static inline __dll_internal_retval	__dll_internal_base(dll_t *restrict dll,
		dll_obj_handler_fn_t fnptr, void *restrict ptr, size_t start, size_t n,
		__dll_internal_search_direction sd, bool is_success_ret);

/***********************
 * Internal realization:
 **********************/
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
			__errcode = __DLL_EHANDLER_NULL;
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

static inline void	__dll_internal_memcpy(dll_obj_t *restrict a, dll_obj_t *restrict b) {
	u_char *restrict	aptr = (u_char *restrict)a;
	u_char *restrict	bptr = (u_char *restrict)b;
	const size_t	offsetnp = offsetof(dll_obj_t, data);
	const size_t	sizecopy = sizeof(*a) - offsetnp;

	memcpy(aptr + offsetnp, bptr + offsetnp, sizecopy);
}
static inline void	__dll_internal_bzero(dll_obj_t *restrict a) {
	u_char *restrict	aptr = (u_char *restrict)a;
	const size_t	offsetnp = offsetof(dll_obj_t, data);
	const size_t	sizecopy = sizeof(*a) - offsetnp;

	memset(aptr + offsetnp, 0, sizecopy);
}

static inline __dll_internal_status	__dll_internal_basecycle(__dll_internal_retval *restrict rv,
		dll_obj_t *restrict obj, dll_obj_handler_fn_t fnptr, void *restrict ptr, bool is_success_ret) {
	bool is_not_ign_err = !__dll_is_bit(obj->bits, DLL_BIT_EIGN);
	if (__dll_unlikely(is_not_ign_err && !obj->data))
		return __dll_seterrno(__DLL_EEMPTY_OBJ);

	ssize_t ret = fnptr(obj->data, ptr, rv->idx);
	if (__dll_likely(!ret)) {
		rv->obj = obj;
		if (is_success_ret)
			return __dlli_status_match;
	} else if (__dll_unlikely(is_not_ign_err && 0 > ret)) {
		return __dll_seterrno(__DLL_EHANDLER_NEG);
	}
	return __dlli_status_valid;
}

static inline __dll_internal_retval	__dll_internal_bdcycle(dll_t *restrict dll,
		dll_obj_handler_fn_t fnptr, void *restrict ptr, bool is_success_ret) {
	__dll_internal_retval rv = { NULL, 0, __dlli_status_valid };
	size_t	headidx = 1;
	size_t	endidx = dll->objs_count;

	for (dll_obj_t *restrict ihead = dll->head, *restrict ilast = dll->last;
	ihead && ilast;
	ihead = ihead->next, ilast = ilast->prev) {
		rv.idx = headidx++;
		rv.status = __dll_internal_basecycle(&rv, ihead, fnptr, ptr, is_success_ret);
		if (__dlli_status_match == rv.status || __dlli_status_invalid == rv.status)
			return rv;
		rv.idx = endidx--;
		rv.status = __dll_internal_basecycle(&rv, ilast, fnptr, ptr, is_success_ret);
		if (__dlli_status_match == rv.status || __dlli_status_invalid == rv.status)
			return rv;
	}
	return rv;
}

static inline __dll_internal_retval	__dll_internal_getobjbyidx(dll_t *restrict dll,
		size_t search_index, __dll_internal_search_direction sd) {
	__dll_internal_retval	rv = {
		(__dlli_sdir_begin == sd) ? dll->head : dll->last,
		(__dlli_sdir_begin == sd) ? 0 : (dll->objs_count + 1),
		__dlli_status_valid
	};
	const size_t	abs_index = ((__dlli_sdir_begin == sd)
		? search_index : (dll->objs_count - search_index + 1));

	while (rv.obj && rv.idx != abs_index) {
		rv.idx += ((__dlli_sdir_begin == sd) ? 1 : -1);
		if (abs_index == rv.idx)
			return rv;
		rv.obj = ((__dlli_sdir_begin == sd) ? rv.obj->next : rv.obj->prev);
	}
	rv = (__dll_internal_retval) { NULL, 0, __dlli_status_invalid };
	return rv;
}

static inline __dll_internal_retval	__dll_internal_base(dll_t *restrict dll,
		dll_obj_handler_fn_t fnptr, void *restrict ptr, size_t start, size_t n,
		__dll_internal_search_direction sd, bool is_success_ret) {
	__dll_internal_retval	retval = { NULL, 0, __dlli_status_valid };

	retval.status = __dll_internal_errno(dll, dll, fnptr, start);
	if (__dlli_status_valid != retval.status)
		return retval;

	if (__dlli_sdir_bd == sd) {
		retval = __dll_internal_bdcycle(dll, fnptr, ptr, is_success_ret);
	} else {
		retval = __dll_internal_getobjbyidx(dll, start, sd);
		while (retval.obj && n--) {
			retval.status = __dll_internal_basecycle(&retval, retval.obj, fnptr, ptr, is_success_ret);
			if (__dlli_status_valid != retval.status)
				return retval;
			retval.idx += ((__dlli_sdir_begin == sd) ? 1 : -1);
			retval.obj = ((__dlli_sdir_begin == sd) ? retval.obj->next : retval.obj->prev);
		}
	}
	return retval;
}

#endif /* LIBDLL_INTERNAL_H */
