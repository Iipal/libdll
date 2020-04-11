#ifndef LIBDLL_STRUCTS_H
# define LIBDLL_STRUCTS_H

# ifndef LIBDLL_H
#  error "include only libdll.h"
# endif

# include <stddef.h>
# include "libdll_bits.h"

typedef int (*f_dll_obj_handler)(const void *restrict);
typedef int (*f_dll_obj_handler_index)(const void *restrict, size_t);
typedef void (*f_dll_obj_data_del)(void *restrict);

typedef struct s_dll_obj {
	struct s_dll_obj *restrict	next;
	struct s_dll_obj *restrict	prev;
	void *restrict	data;
	size_t	data_size;
	f_dll_obj_data_del	del;
	dll_bits_t	bits;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__))) dll_obj_t;

typedef struct s_dll {
	dll_obj_t *restrict	head;
	dll_obj_t *restrict	last;
	size_t	objs_count;
	dll_bits_t	bits;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__))) dll_t;


#endif /* LIBDLL_STRUCTS_H */
