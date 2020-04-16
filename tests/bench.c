#define _GNU_SOURCE

#define ANV_TRACE_IMPLEMENTATION 1
#include "anv/src/anv_trace.h"
#include "anv/src/anv_bench.h"

#define LIBDLL_PTHREAD_IMPLEMENTATITON 1
#include "../libdll.h"

#include <err.h>
#include <stdio.h>

int	iter_obj(void *restrict data, void *restrict ptr, size_t idx) {
	(void)idx;
	(void)ptr;
	char *restrict	str = data;
	*str = 'b';
	return 0;
}

static void	libdll_tests(void);

int main(void) {
	FILE *bout = fopen("./bout.log", "wb");
	if (!bout)
		err(EXIT_FAILURE, "fopen(./out.log)");
	FILE *berr = fopen("./berr.log", "wb");
	if (!berr)
		err(EXIT_FAILURE, "fopen(./err.log)");

	int	bout_fd = fileno(bout);
	int	berr_fd = fileno(berr);
	int	save_stdout_fd = dup(fileno(stdout));
	int	save_stderr_fd = dup(fileno(stdout));

	if (-1 == dup2(bout_fd, fileno(stdout)))
		err(EXIT_FAILURE, "can't redirect STDOUT to bout.log");
	if (-1 == dup2(berr_fd, fileno(stderr)))
		err(EXIT_FAILURE, "can't redirect STDERR to berr.log");

	libdll_tests();

	fflush(stdout);
	fflush(stderr);
	fclose(bout);
	fclose(berr);
	dup2(save_stdout_fd, fileno(stdout));
	dup2(save_stderr_fd, fileno(stderr));
	close(save_stderr_fd);
	close(save_stdout_fd);
};


static void	libdll_tests(void) {
	dll_t *restrict	dll;
	anv_trace_init(stdout);
	anv_trace_enter();

	anv_tracei("initialization list: %s", "dll_init");
	dll_assert_soft_perror(dll = dll_init(DLL_BIT_DFLT));
	anv_tracei("10 runs with 1000 times adding objects and run foreach%c", '.');
    volatile int _i;
	ANV_BENCH_BEGIN(stdout, 10, 1000);
		ANV_BENCH_ADD(dll_assert_soft_perror, dll_pushback(dll, strdup("test"), 4, DLL_BIT_FREE, NULL));
		ANV_BENCH_ADD(dll_assert_soft_perror, dll_foreach(dll, iter_obj));
	ANV_BENCH_END();
	anv_tracei("Currently %zu objects in list\n", dll_getsize(dll));
	anv_tracei("2 runs with 10 times trying to create object for NULL-pointed list%c", '.');
	ANV_BENCH_BEGIN(stdout, 2, 1);
		ANV_BENCH_ADD(dll_assert_soft_perror, dll_pushback(NULL, NULL, 0, DLL_BIT_DFLT, NULL));
	ANV_BENCH_END();
	anv_tracei("Freeing list", '.');
	ANV_BENCH(stdout, 1, dll_free, &dll);
	anv_tracei("initialization new list%c", '.');
	dll_assert_soft_perror(dll = dll_init(DLL_BIT_DFLT));
	anv_tracei("2 runs with 10 times trying operations with empty list%c", '.');
	ANV_BENCH_BEGIN(stdout, 2, 1);
		ANV_BENCH_ADD(dll_assert_soft_perror, dll_popback(dll));
		ANV_BENCH_ADD(dll_assert_soft_perror, dll_popfront(dll));
		ANV_BENCH_ADD(dll_assert_soft_perror, dll_getsize(dll));
		ANV_BENCH_ADD(dll_assert_soft_perror, dll_gethead(dll));
		ANV_BENCH_ADD(dll_assert_soft_perror, dll_getlast(dll));
	ANV_BENCH_END();
	anv_tracei("Freeing empty list%c", '.');
	ANV_BENCH(stdout, 1, dll_assert_soft_perror, dll_free(&dll));

	anv_trace_leave();
	anv_trace_quit();
}
