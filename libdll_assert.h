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

extern char	*__progname;

static inline void __THROW __attribute__((noreturn)) __dll_assert_fail(void) {
	unsigned int	errno_line = *__dll_get_errno_line();
	char *errno_func = *__dll_get_errno_func();
	char *errno_file = *__dll_get_errno_file();
	dll_errno_t errno_num = *__dll_get_errno_num();
	char *dll_err = dll_strerr(errno_num);
	char *str;

	if (0 <= asprintf(&str, "%s%s%s:%u: %s%s%s.\n",
			__progname, __progname[0] ? ": " : "",
			errno_file ? errno_file : "", errno_line,
			errno_func ? errno_func : "",
			errno_func ? ": " : "", dll_err)) {
		fprintf(stderr, "%s", str);
		fflush(stderr);
		free(str);
	} else {
		static const char	errstr[] = "Unexpected error\n";
		write(STDERR_FILENO, errstr, sizeof(errstr) - 1);
	}
	abort();
}

#  define dll_assert(expr) ((expr) \
	? __ASSERT_VOID_CAST (0) \
	: __dll_assert_fail())
# endif

#endif /* LIBDLL_ASSERT_H */
