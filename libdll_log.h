#ifndef LIBDLL_LOG_H
# define LIBDLL_LOG_H

# include <assert.h>
# include <stdarg.h>
# include <stdio.h>
# include <time.h>
# include <sys/types.h>
# include <sys/time.h>

# if !defined(LIBDLL_FN_FREE_CLEARANCE) && !defined(LIBDLL_FN_FREE_NULL)
#  include "libdll.h"
# endif /* !defined(LIBDLL_FN_FREE_CLEARANCE) && !defined(LIBDLL_FN_FREE_NULL) */

/**
 * ----------------------------
 * Function prototypes
 * ----------------------------
 */
static char             *dlldbg_parent_filename = NULL;
static void             dlldbg_parent_filename_set(const char *filename);
static char             *dlldbg_parent_filename_get(void);

static FILE             *dlldbg_logfile = NULL;
static void             dlldbg_logfile_open();

static void             dlldbg_vfprintf(const char *fmt, va_list arg);
static void             dlldbg_fprintf(const char *fmt, ...);
static void             dlldbg_fputs(const char *str);

static char             *dlldbg_log_get_method_depth_fmt(void);
static char             *dlldbg_log_get_method_depth_arg(void);

static void             dlldbg_log_method_entry(const char *file, const char *func, const char *fmt, ...);
static void             dlldbg_log_method_out(const char *file, const char *func, const char *fmt, ...);

static void             dlldbg_depth_display_last_delimiter(void);

static size_t           dlldbg_nested_depth = 0;
static void             dlldbg_depth_inc(void);
static void             dlldbg_depth_dec(void);
static void             dlldbg_depth_set(size_t depth);
static void             dlldbg_depth_reset(void);


static const char       dlldbg_nested_depth_display_prefix[] = "| ";
static const size_t     dlldbg_nested_depth_display_prefix_len = sizeof(dlldbg_nested_depth_display_prefix) - sizeof(char);
static const char       dlldbg_nested_depth_display[] = "--";
static const size_t     dlldbg_nested_depth_display_len = sizeof(dlldbg_nested_depth_display) - sizeof(char);
static const char       dlldbg_nested_depth_display_postfix[] = "> ";
static const size_t     dlldbg_nested_depth_display_postfix_len = sizeof(dlldbg_nested_depth_display_postfix) - sizeof(char);


static void             dlldbg_atexit_clearance(void);

/**
 * ----------------------------
 * Macros definitions
 * ----------------------------
 */
# define dll_init_logger { \
    dlldbg_parent_filename_set(__FILE__); \
    dlldbg_logfile_open(); \
    atexit(dlldbg_atexit_clearance); }

# define DLLDBG_LOG_BASE_FILENAME "dlldbg"

# define DLLDBG_LOG(__fmt, ...) dlldbg_log_method_entry(__FILE__, __FUNCTION__, __fmt, __VA_ARGS__)
# define DLLDBG_LOG_VOID dlldbg_log_method_entry(__FILE__, __FUNCTION__, "%s", "void")

# define DLLDBG_LOG_OUT(__fmt, ...) dlldbg_log_method_out(__FILE__, __FUNCTION__, __fmt, __VA_ARGS__)
# define DLLDBG_LOG_OUT_NULL dlldbg_log_method_out(__FILE__, __FUNCTION__, "%p", NULL)
# define DLLDBG_LOG_OUT_VOID dlldbg_log_method_out(__FILE__, __FUNCTION__, "%s", "void")

# define DLLDBG_LOG_DLL_FN_FREE_FMT(__fn_free) #__fn_free ": %s"
# define DLLDBG_LOG_DLL_FN_FREE_ARG(__fn_free) \
    (__fn_free) == LIBDLL_FN_FREE_CLEARANCE \
        ? "LIBDLL_FN_FREE_CLEARANCE" \
        : (__fn_free) == LIBDLL_FN_FREE_NULL \
            ? "LIBDLL_FN_FREE_NULL" \
            : "LIBDLL_FN_FREE_SET"

# define DLLDBG_LOG_DLL_OBJ_FMT(__obj) #__obj ": %p ( data: %p, data_size: %zu, prev: %p, next: %p, fn_free: %s )"
# define DLLDBG_LOG_DLL_OBJ_ARG(__obj) (__obj), \
    (__obj) ? (__obj)->data : NULL, \
    (__obj) ? (__obj)->data_size : 0, \
    (__obj) ? (__obj)->prev : NULL, \
    (__obj) ? (__obj)->next : NULL, \
    (__obj) ? DLLDBG_LOG_DLL_FN_FREE_ARG((__obj)->fn_free) : NULL

# define DLLDBG_LOG_DLL_FMT(__dll) #__dll ": %p ( head: %p, tail: %p, objs_count: %zu)"
# define DLLDBG_LOG_DLL_ARG(__dll) (__dll), \
    (__dll) ? (__dll)->head : NULL, \
    (__dll) ? (__dll)->tail : NULL, \
    (__dll) ? (__dll)->objs_count : 0

# define DLLDBG_LOG_OUT_DELIMITER dlldbg_depth_display_last_delimiter()

/**
 * ----------------------------
 * Function definitions
 * ----------------------------
 */
static char             *dlldbg_parent_filename_get(void) {
    char    *out = "unknown";
    if (dlldbg_parent_filename) {
        out = dlldbg_parent_filename;
    }
    return out;
}
static void             dlldbg_parent_filename_set(const char *filename) {
    const size_t    filename_len = strlen(filename);

    assert((dlldbg_parent_filename = calloc(filename_len + 1, sizeof(*filename))));
    strcpy(dlldbg_parent_filename, filename);
}

static void     dlldbg_logfile_open() {
    char            timestamp_buff[128];
    char            logfilename_buff[128];

    struct timeval  tv;
        gettimeofday(&tv, NULL);
    int millisec = tv.tv_usec / 1000.0;
    if (millisec >= 1000) {
        millisec -= 1000;
        tv.tv_sec++;
    }

    strftime(timestamp_buff, sizeof(timestamp_buff) - 1, "%y-%m-%d-%T", localtime(&tv.tv_sec));
    snprintf(logfilename_buff, sizeof(logfilename_buff) - 1, "./%s_%s.%03d_%s.log",
        DLLDBG_LOG_BASE_FILENAME, timestamp_buff, millisec, dlldbg_parent_filename_get());

    assert((dlldbg_logfile = fopen(logfilename_buff, "w+")));
    setbuf(dlldbg_logfile, NULL);
}

static void     dlldbg_vfprintf(const char *fmt, va_list arg) {
    if (dlldbg_logfile) {
        vfprintf(dlldbg_logfile, fmt, arg);
    }
}

static void     dlldbg_fprintf(const char *fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);

    dlldbg_vfprintf(fmt, argptr);

    va_end(argptr);
}

static void dlldbg_fputs(const char *str) {
    dlldbg_fprintf("%s", str);
}

static char *dlldbg_log_get_method_depth_fmt(void) {
    static char     depth_fmt_str[16];
    const size_t    depth_fmt_str_len = sizeof(depth_fmt_str);
    const size_t    depth_fmt_len = dlldbg_nested_depth * dlldbg_nested_depth_display_len;

    depth_fmt_str[0] = 0;
    const size_t depth_fmt_written_len = snprintf(depth_fmt_str, 15, "%%%zus", depth_fmt_len);
    depth_fmt_str[depth_fmt_written_len] = 0;

    return depth_fmt_str;
}

static char *dlldbg_log_get_method_depth_arg(void) {
    static char         depth_arg_buff[512];
    const size_t        depth_arg_buff_len = sizeof(depth_arg_buff);
    static char         depth_arg_display_buff[512];
    const size_t        depth_arg_display_buff_len = sizeof(depth_arg_display_buff);

    for (size_t i = 0; dlldbg_nested_depth > i; i++) {
        const size_t    buff_iptr = dlldbg_nested_depth_display_len * i;
        strcpy(depth_arg_display_buff + buff_iptr, dlldbg_nested_depth_display);
    }
    depth_arg_display_buff[dlldbg_nested_depth * dlldbg_nested_depth_display_len] = 0;

    const size_t depth_arg_written_len = snprintf(depth_arg_buff, depth_arg_buff_len,
        "%s%s%s",
        dlldbg_nested_depth_display_prefix, depth_arg_display_buff, dlldbg_nested_depth_display_postfix);
    depth_arg_buff[depth_arg_written_len] = 0 ;

    return depth_arg_buff;
}

static void dlldbg_log_method_entry(const char *file, const char *func, const char *fmt, ...) {
    static char     prefix_fmt[64];
    va_list         argptr;

    va_start(argptr, fmt);

    char *restrict  depth_fmt = dlldbg_log_get_method_depth_fmt();
    char *restrict  depth_arg = dlldbg_log_get_method_depth_arg();

    prefix_fmt[0] = 0;
    const size_t prefix_fmt_written_len = snprintf(prefix_fmt, sizeof(prefix_fmt) - 1, "%s%%s:%%s ( ", depth_fmt);
    prefix_fmt[prefix_fmt_written_len] = 0;

    dlldbg_fprintf(prefix_fmt, depth_arg, file, func);
    dlldbg_vfprintf(fmt, argptr);
    dlldbg_fprintf(" )\n");

    va_end(argptr);
}

static void dlldbg_log_method_out(const char *file, const char *func, const char *fmt, ...) {
    static char     prefix_fmt[32];
    va_list         argptr;

    va_start(argptr, fmt);

    char *restrict  depth_fmt = dlldbg_log_get_method_depth_fmt();
    char *restrict  depth_arg = dlldbg_log_get_method_depth_arg();

    prefix_fmt[0] = 0;
    const size_t prefix_fmt_written_len = snprintf(prefix_fmt, sizeof(prefix_fmt) - 1, "%s%%s %%s return ( ", depth_fmt);
    prefix_fmt[prefix_fmt_written_len] = 0;

    dlldbg_fprintf(prefix_fmt, depth_arg, file, func);
    dlldbg_vfprintf(fmt, argptr);
    dlldbg_fprintf(" )\n");
    dlldbg_depth_display_last_delimiter();

    va_end(argptr);
}

static void dlldbg_depth_inc(void) {
    dlldbg_nested_depth += 1;
}
static void dlldbg_depth_dec(void) {
    if (0 != dlldbg_nested_depth) {
        dlldbg_nested_depth -= 1;
    }
}
static void dlldbg_depth_set(size_t depth) {
    dlldbg_nested_depth = depth;
}
static void dlldbg_depth_reset(void) {
    dlldbg_nested_depth = 0;
}

static void dlldbg_depth_display_last_delimiter(void) {
    if (!dlldbg_nested_depth) {
        dlldbg_fputs("\n");
    }
}

static void dlldbg_atexit_clearance(void) {
    if (dlldbg_parent_filename) {
        free(dlldbg_parent_filename);
    }

    if (dlldbg_logfile) {
        fclose(dlldbg_logfile);
    }

    dlldbg_parent_filename = NULL;
    dlldbg_logfile = NULL;
}

#endif /* LIBDLL_LOG_H */
