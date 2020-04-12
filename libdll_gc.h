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

#ifndef LIBDLL_GC_H
# define LIBDLL_GC_H

# include "libdll.h"

static dll_t	*__dll_getgcl(void) {
	static dll_t	__garbage_collector_list;
	return &__garbage_collector_list;
}

static void	__dll_gclfn_delobj(void *restrict data) {
	dll_free((dll_t *restrict *restrict)&data);
}

static int	__dll_gclfn_searchptr(const void *restrict data, void *restrict any_data) {
	return !(data == any_data);
}

static inline void	__dll_addgcptr(void *restrict ptr) {
	dll_assert(dll_pushback(__dll_getgcl(), ptr, sizeof(ptr), DLL_BIT_EIGN, __dll_gclfn_delobj));
}

static void	__dll_unlinkgcptr(void *restrict ptr) {
	dll_t *restrict	gcl = __dll_getgcl();
	dll_obj_t *restrict	unlink_obj = dll_findkey(gcl, __dll_gclfn_searchptr, ptr);
	if (unlink_obj)
		dll_unlink(gcl, unlink_obj);
}

static void	__dll_freegc_atexit(void) {
	dll_t *restrict	gc = __dll_getgcl();
	if (dll_getsize(gc)) {
		fprintf(stderr, " -- libdll: %zu lists aren't freed. freeing it now:\n", dll_getsize(gc));
		while (dll_popfront(gc))
			;
		fprintf(stderr, "\tOK. all garbage lists freed.\n");
	}
}

#endif /* LIBDLL_GC_H */
