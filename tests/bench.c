#define ANV_TRACE_IMPLEMENTATION 1
#include "anv/src/anv_trace.h"
#include "anv/src/anv_bench.h"
#include "../libdll.h"

int	iter_obj(void *restrict data) {
	char *restrict	str = data;
	*str = 'b';
	return 0;
}

int main(void) {
	dll_t *restrict	dll = dll_init(DLL_BIT_DFLT);

	ANV_BENCH_BEGIN(stdout, 10, 2100);
		ANV_BENCH_ADD(dll_pushback, dll, strdup("test"), 4, DLL_BIT_FREE, NULL);
		ANV_BENCH_ADD(dll_foreach, dll, iter_obj);
	ANV_BENCH_END();

	printf("\n\t!!! total added objects: %zu !!!\n\n", dll_getsize(dll));
	ANV_BENCH(stdout, 1, dll_free, &dll);
};
