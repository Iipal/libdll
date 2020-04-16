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

#ifndef LIBDLL_ASSERT_H
# define LIBDLL_ASSERT_H

# ifndef LIBDLL_H
#  error "include only libdll.h"
# endif

# include <assert.h>

# ifdef	NDEBUG
#  define dll_assert(expr) (__ASSERT_VOID_CAST (0))
# else

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

# include "libdll_errno.h"
# include "libdll_bits.h"

typedef enum {
	__dll_assert_tnormal             = 1,
	__dll_assert_tsoft               = 2,
	__dll_assert_tsoft_normal        = 3,
	__dll_assert_tperror             = 4,
	__dll_assert_tperror_normal      = 5,
	__dll_assert_tperror_soft        = 6,
	__dll_assert_tperror_soft_normal = 7,
} __attribute__((packed)) __dll_internal_assert_type;

extern char	*__progname;

static inline void	__dll_assert_fail(const char *assertion,
		const char *file,
		unsigned int line,
		const char *function,
		__dll_internal_assert_type atype) {
	if (__dll_is_bit(atype, __dll_assert_tnormal)) {
		fprintf(stderr, "%s%s%s:%u: %s%sAssertion `%s` failed.\n",
			__progname, __progname[0] ? ": " : "", file, line,
			function ? function : "", function ? ": " : "", assertion);
	}
	if (__dll_is_bit(atype, __dll_assert_tperror)) {
		__dll_internal_errdata_t *restrict errdata = __dll_internal_geterrdata();
		fprintf(stderr, " -- libdll: %s%s%u: %s%s%s.\n",
			errdata->__errfile, errdata->__errfile[0] ? ": " : "",
			errdata->__errln,
			errdata->__errfn ? errdata->__errfn : "",
			errdata->__errfn ? ": " : "", dll_strerr(errdata->__errcode));
	}
	fflush(stderr);
	if (!__dll_is_bit(atype, __dll_assert_tsoft)) {
		abort();
	}
}

#  define dll_assert(expr) ((expr) \
	? __ASSERT_VOID_CAST (0) \
	: __dll_assert_fail(#expr, __FILE__, __LINE__, \
		__ASSERT_FUNCTION, __dll_assert_tnormal))

#  define dll_assert_soft(expr) ((expr) \
	? __ASSERT_VOID_CAST (0) \
	: __dll_assert_fail(#expr, __FILE__, __LINE__, \
		__ASSERT_FUNCTION, __dll_assert_tsoft_normal))

#  define dll_assert_perror(expr) ((expr) \
	? __ASSERT_VOID_CAST (0) \
	: __dll_assert_fail(#expr, __FILE__, __LINE__, \
		__ASSERT_FUNCTION, __dll_assert_tperror_normal))

#  define dll_assert_soft_perror(expr) ((expr) \
	? __ASSERT_VOID_CAST (0) \
	: __dll_assert_fail(#expr, __FILE__, __LINE__, \
		__ASSERT_FUNCTION, __dll_assert_tperror_soft_normal))

# endif

#endif /* LIBDLL_ASSERT_H */
# ifndef __clang__
/**
 * GCC doesn't allow to fall through switch-cases
 * This attribute required to ignore compiler errors about this.
 */
			__attribute__((fallthrough));
# endif /* __clang__ */
