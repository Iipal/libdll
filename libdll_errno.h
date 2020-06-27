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

#ifndef LIBDLL_ERRNO_H
# define LIBDLL_ERRNO_H

# ifndef LIBDLL_H
#  error "include only libdll.h"
# endif

# include <assert.h>
# include <string.h>

static struct {
	const char *restrict	__errfn;
	const char *restrict	__errfile;
	const char *restrict	__errassertion;
	unsigned int	__errln;
	enum {
		__DLL_EMIN_ERRNO,
		__DLL_ESUCCESS = __DLL_EMIN_ERRNO,
		__DLL_EALLOC,
		__DLL_EDUP,
		__DLL_ENULL,
		__DLL_EEMPTY,
		__DLL_EEMPTY_OBJ,
		__DLL_ENO_OBJ_LINK,
		__DLL_EHANDLER_NULL,
		__DLL_EHANDLER_NEG,
		__DLL_EINSERT_TYPE,
		__DLL_EOUTOFRANGE,
		__DLL_NO_ERR,
		__DLL_EMAX_ERRNO = __DLL_NO_ERR
	} __attribute__((packed)) __errcode;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__))) __dll_internal_errdata = {
	"(null)", "(null)", "(null)", 0U, __DLL_ESUCCESS
};

static char	*__dll_internal_errstrs[__DLL_EMAX_ERRNO + 1] = {
	[__DLL_ESUCCESS]      = "Success",
	[__DLL_EALLOC]        = "Memory allocation error",
	[__DLL_EDUP]          = "Memory duplication error",
	[__DLL_ENULL]         = "Operations with NULL-pointer",
	[__DLL_EEMPTY]        = "Operations with empty list",
	[__DLL_EEMPTY_OBJ]    = "Operations with empty object",
	[__DLL_ENO_OBJ_LINK]  = "Given object is not a part of a given list",
	[__DLL_EHANDLER_NULL] = "No handler provided",
	[__DLL_EHANDLER_NEG]  = "Handler returned a negative value",
	[__DLL_EINSERT_TYPE]  = "Invalid insert type",
	[__DLL_EOUTOFRANGE]   = "List indexing out of range",
	[__DLL_NO_ERR]       = "All occurred errors ignored"
};

# define __dll_assert_errno(_expr, _errcode) __extension__({ \
	if (__dll_unlikely(!(_expr))) { \
		__dll_internal_errdata.__errfn = (char*)__ASSERT_FUNCTION; \
		__dll_internal_errdata.__errfile = __FILE__; \
		__dll_internal_errdata.__errassertion = #_expr; \
		__dll_internal_errdata.__errln = __LINE__; \
		__dll_internal_errdata.__errcode = (_errcode); \
		return 0; \
	} \
})

# define __dll_seterrno(_errcode) __extension__({ \
	__dll_internal_errdata.__errfn = (char*)__ASSERT_FUNCTION; \
	__dll_internal_errdata.__errfile = __FILE__; \
	__dll_internal_errdata.__errassertion = NULL; \
	__dll_internal_errdata.__errln = __LINE__; \
	__dll_internal_errdata.__errcode = (_errcode); \
	(bool)0; \
})

/**
 * Get a error message corresponding to \param errno_code error code
 */
static inline char	*dll_strerr(int errno_code) {
	char *restrict	ret = NULL;
	if (__DLL_EMIN_ERRNO > errno_code || __DLL_EMAX_ERRNO < errno_code) {
		ret = "invalid errno";
	} else {
		ret = __dll_internal_errstrs[errno_code];
	}
	return ret;
}

/**
 * Permanently print error message corresponding to last setted-up errno code in libdll
 */
static inline void	dll_perror(const char *restrict str) {
	char *errstr = dll_strerr(__dll_internal_errdata.__errcode);
	if (str && *str) {
		fwrite(str, strlen(str), 1, stderr);
		fputc(':', stderr);
		fputc(' ', stderr);
	}
	fwrite(errstr, strlen(errstr), 1, stderr);
	fputc('\n', stderr);
}

#endif /* LIBDLL_ERRNO_H */
