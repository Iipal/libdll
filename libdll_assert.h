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

static inline void __THROWNL __attribute__((noreturn))
__dll_assert_fail(const char *assertion, const char *file,
		unsigned int line, const char *function) {
	unsigned int dllerr_line = *__dll_get_errno_line();
	char *dllerr_func = *__dll_get_errno_func();
	char *dllerr_file = *__dll_get_errno_file();
	dll_errno_t dllerr_num = *__dll_get_errno_num();
	char *dll_err = dll_strerr(dllerr_num);
	char *str;

	if (0 <= asprintf(&str,
			"%s%s%s:%u: %s%sAssertion `%s` failed.\n"
			" -- libdll: %s:%u: %s%s%s.\n",
			__progname, __progname[0] ? ": " : "",
			file, line,
			function ? function : "", function ? ": " : "",
			assertion, // end of default assertion error message
			dllerr_file ? dllerr_file : "", dllerr_line,
			dllerr_func ? dllerr_func : "",
			dllerr_func ? ": " : "", dll_err)) {
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
	: __dll_assert_fail(#expr, __FILE__, __LINE__, __ASSERT_FUNCTION))
# endif

#endif /* LIBDLL_ASSERT_H */
