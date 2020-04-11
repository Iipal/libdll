#ifndef LIBDLL_ERRNO_H
# define LIBDLL_ERRNO_H

# ifndef LIBDLL_H
#  error "include only libdll.h"
# endif

# include <assert.h>

typedef enum __e_dll_errno {
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
	__DLL_EOVERFLOW,
# define __DLL_EOVERFLOW __DLL_EOVERFLOW
	__DLL_EOUTOFRANGE,
# define __DLL_EOUTOFRANGE __DLL_EOUTOFRANGE
	__DLL_EMASK,
# define __DLL_EMASK __DLL_EMASK
} __attribute__((packed)) dll_errno_t;

extern char	*__progname;

static dll_errno_t	*__dll_get_errno_num(void) {
	static dll_errno_t errno_num;
	return &errno_num;
}
static unsigned int	*__dll_get_errno_line(void) {
	static unsigned int	errno_line;
	return &errno_line;
}
static char **__dll_get_errno_func(void) {
	static char	*errno_func;
	return &errno_func;
}
static char **__dll_get_errno_file(void) {
	static char	*errno_file;
	return &errno_file;
}

# define __dll_seterrno(_errcode) __extension__({ \
	*__dll_get_errno_num() = (_errcode); \
	*__dll_get_errno_line() = __LINE__; \
	*__dll_get_errno_file() = __FILE__; \
	*__dll_get_errno_func() = (char*)__ASSERT_FUNCTION; \
	__ASSERT_VOID_CAST(0); \
})

/**
 * Get a error message corresponding to \param err_num error code
 */
static inline char	*dll_strerr(dll_errno_t err_num) {
	static char	*__err_strs[] = {
		"Success", "Memory calloc-ation error", "Memory duplication error",
		"Operations with NULL-pointer", "Operations with empty list",
		"Operations with empty object", "No handler provided",
		"Handler returned a negative value", "List indexing overflow",
		"List indexing out of range", "Invalid bits mask"
	};

	return __err_strs[err_num];
}

/**
 * Permanently print error message corresponding
 * to last setted-up errno code in libdll
 */
static inline void	dll_perror(const char *restrict str) {
	char *errstr = dll_strerr(*__dll_get_errno_num());
	if (str && *str) {
		fwrite(str, strlen(str), 1, stderr);
		fputc(':', stderr);
		fputc(' ', stderr);
	}
	fwrite(errstr, strlen(errstr), 1, stderr);
	fputc('\n', stderr);
}

#endif /* LIBDLL_ERRNO_H */
