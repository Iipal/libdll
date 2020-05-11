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

typedef ssize_t (*dll_obj_handler_fn_t)(void *restrict, void *restrict, size_t);
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

/**
 * Specify only \param objptr or \param index and specify the insert type correct
 */
typedef struct s_dll_insert_data {
	union {
		dll_obj_t *restrict	at_objptr;
		size_t	at_index;
	};

	enum
# ifdef __clang__
	__attribute__((__enum_extensibility__(closed), __flag_enum__))
# endif
	{
		dll_insert_after,
		dll_insert_before,
	} __attribute__((packed)) method;

	enum
# ifdef __clang__
	__attribute__((__enum_extensibility__(closed), __flag_enum__))
# endif
	{
		dll_insert_at_object,
		dll_insert_at_index,
	} __attribute__((packed)) type;
} dll_insert_data_t;

/**
 * Fast create insert data for dll_insert functions
 * ||| dll_insert(dll, obj, dll_insert_data(other_obj, dll_insert_after, dll_insert_at_object))
 * or
 * ||| dll_insert_data_t id_info = dll_insert_data(5, dll_insert_before, dll_insert_at_index);
 * ||| dll_insert(dll, obj, id_info);
 */
# define dll_insert_data(data, method, type) (dll_insert_data_t) { \
	{ (data) }, (method), (type) \
}

#endif /* LIBDLL_STRUCTS_H */
