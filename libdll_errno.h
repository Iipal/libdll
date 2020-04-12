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

typedef enum {
	__DLL_ESUCCESS,
# define __DLL_ESUCCESS __DLL_ESUCCESS
	__DLL_ECALLOC,
# define __DLL_ECALLOC __DLL_ECALLOC
	__DLL_EDUP,
# define __DLL_EDUP __DLL_EDUP
	__DLL_ENULL,
# define __DLL_ENULL __DLL_ENULL
	__DLL_EEMPTY,
# define __DLL_EEMPTY __DLL_EEMPTY
	__DLL_EEMPTY_OBJ,
# define __DLL_EEMPTY_OBJ __DLL_EEMPTY_OBJ
	__DLL_ENOHANDLER,
# define __DLL_ENOHANDLER __DLL_ENOHANDLER
	__DLL_ENEGHANDLER,
# define __DLL_ENEGHANDLER __DLL_ENEGHANDLER
	__DLL_EOUTOFRANGE,
# define __DLL_EOUTOFRANGE __DLL_EOUTOFRANGE
	__DLL_EMASK,
# define __DLL_EMASK __DLL_EMASK
} __attribute__((packed)) __dll_internal_errcode_t;

typedef struct {
	char	*__errfn;
	char	*__errfile;
	unsigned int	__errln;
	__dll_internal_errcode_t	__errcode;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__))) __dll_internal_errdata_t;

static __dll_internal_errdata_t	*__dll_geterrdata(void) {
	static __dll_internal_errdata_t	__errno_data;
	return &__errno_data;
}

static inline void	__dll_errflush(void) {
	*__dll_geterrdata() = (__dll_internal_errdata_t) {
		"(null)", "(null)", 0, __DLL_ESUCCESS
	};
}

# define __dll_seterrno(_errcode) __extension__({ \
	*__dll_geterrdata() = (__dll_internal_errdata_t) { \
		(char*)__ASSERT_FUNCTION, __FILE__, __LINE__, (_errcode) \
	}; \
	__ASSERT_VOID_CAST(0); \
})

/**
 * Get a error message corresponding to \param errno_code error code
 */
static inline char	*dll_strerr(int errno_code) {
	static char	*__err_strs[] = {
		"Success", "Memory calloc-ation error", "Memory duplication error",
		"Operations with NULL-pointer", "Operations with empty list",
		"Operations with empty object", "No handler provided",
		"Handler returned a negative value", "List indexing out of range",
		"Invalid bits mask"
	};

	return __err_strs[errno_code];
}

/**
 * Permanently print error message corresponding to last setted-up errno code in libdll
 */
static inline void	dll_perror(const char *restrict str) {
	char *errstr = dll_strerr(__dll_geterrdata()->__errcode);
	if (str && *str) {
		fwrite(str, strlen(str), 1, stderr);
		fputc(':', stderr);
		fputc(' ', stderr);
	}
	fwrite(errstr, strlen(errstr), 1, stderr);
	fputc('\n', stderr);
}

#endif /* LIBDLL_ERRNO_H */
