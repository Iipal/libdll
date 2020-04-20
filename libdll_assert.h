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

extern char	*__progname;

static inline void	__dll_assert_fail(const char *assertion,
		const char *file,
		unsigned int line,
		const char *function,
		bool is_soft) {
	fprintf(stderr, "%s%s%s:%u: %s%sAssertion `%s` failed.\n"
					" -- libdll: %s:%u: %s: %s%s%s%s.\n",
		__progname, __progname[0] ? ": " : "", file, line,
		function ? function : "", function ? ": " : "", assertion,
		__dll_internal_errdata.__errfile,
		__dll_internal_errdata.__errln,
		__dll_internal_errdata.__errfn,
		__dll_internal_errdata.__errassertion ? "Assertion `" : "",
		__dll_internal_errdata.__errassertion ? __dll_internal_errdata.__errassertion : "",
		__dll_internal_errdata.__errassertion ? "` failed with: ": "",
		dll_strerr(__dll_internal_errdata.__errcode));
	fflush(stderr);
	if (!is_soft)
		abort();
}

#  define dll_assert(expr) ((expr) \
	? __ASSERT_VOID_CAST (0) \
	: __dll_assert_fail(#expr, __FILE__, __LINE__, \
		__ASSERT_FUNCTION, false))

#  define dll_assert_soft(expr) ((expr) \
	? __ASSERT_VOID_CAST (0) \
	: __dll_assert_fail(#expr, __FILE__, __LINE__, \
		__ASSERT_FUNCTION, true))

# endif

#endif /* LIBDLL_ASSERT_H */
# ifndef __clang__
/**
 * GCC doesn't allow to fall through switch-cases
 * This attribute required to ignore compiler errors about this.
 */
			__attribute__((fallthrough));
# endif /* __clang__ */
