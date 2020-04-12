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

#ifndef LIBDLL_STRUCTS_H
# define LIBDLL_STRUCTS_H

# ifndef LIBDLL_H
#  error "include only libdll.h"
# endif

# include <stddef.h>
# include "libdll_bits.h"

typedef int (*dll_obj_handler_fn_t)(const void *restrict, void *restrict);
typedef int (*dll_obj_handleridx_fn_t)(const void *restrict, size_t);
typedef void (*dll_obj_free_fn_t)(void *restrict);

typedef struct s_dll_obj {
	struct s_dll_obj *restrict	next;
	struct s_dll_obj *restrict	prev;
	void *restrict	data;
	size_t	data_size;
	dll_obj_free_fn_t	free;
	dll_bits_t	bits;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__))) dll_obj_t;

typedef struct s_dll {
	dll_obj_t *restrict	head;
	dll_obj_t *restrict	last;
	size_t	objs_count;
	dll_bits_t	bits;
} __attribute__((aligned(__BIGGEST_ALIGNMENT__))) dll_t;


#endif /* LIBDLL_STRUCTS_H */
