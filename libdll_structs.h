#ifndef LIBDLL_STRUCTS_H
# define LIBDLL_STRUCTS_H

# ifndef LIBDLL_H
#  error "include only libdll.h"
# endif

# include <stddef.h>
# include "libdll_bits.h"

typedef struct s_dll_obj {
	struct s_dll_obj *restrict	next;
	struct s_dll_obj *restrict	prev;
	void *restrict	data;
	size_t	data_size;
	dll_bits_t	bits;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__))) dll_obj_t;

typedef struct s_dll {
	dll_obj_t *restrict	head;
	dll_obj_t *restrict	last;
	size_t	objs_count;
	dll_bits_t	bits;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__))) dll_t;

typedef int (*f_dll_obj_handler)(const void *restrict);

#endif /* LIBDLL_STRUCTS_H */
