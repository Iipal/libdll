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

#ifndef LIBDLL_LOG_H
#define LIBDLL_LOG_H

#ifdef LIBDLL_DEBUG_INFO
#  undef LIBDLL_DEBUG_INFO
#  define LIBDLL_DEBUG_INFO 1

#  include <assert.h>
#  include <stdarg.h>
#  include <stdio.h>
#  include <sys/time.h>
#  include <sys/types.h>
#  include <time.h>

#  if !defined(LIBDLL_FN_FREE_CLEARANCE) && !defined(LIBDLL_FN_FREE_NULL)
#    include "libdll.h"
#  endif /* !defined(LIBDLL_FN_FREE_CLEARANCE) && !defined(LIBDLL_FN_FREE_NULL) */

/**
 * ---------------------------------------
 * Function prototypes & global variables
 * ---------------------------------------
 */
static char * _dlldbg_parent_filename = NULL;
static char * dlldbg_parent_filename_get(void);
static void   dlldbg_parent_filename_set(const char * filename);

static FILE * _dlldbg_logfile = NULL;
static void   dlldbg_logfile_open();

static void dlldbg_vfprintf(const char * fmt, va_list arg);
static void dlldbg_fprintf(const char * fmt, ...);
static void dlldbg_fputs(const char * str);

static char * dlldbg_log_get_method_depth_fmt(void);
static char * dlldbg_log_get_method_depth_arg(void);

static void dlldbg_log_method_prefix(void);
static void
    dlldbg_log_method_entry(const char * file, const char * func, const char * fmt, ...);
static void
    dlldbg_log_method_out(const char * file, const char * func, const char * fmt, ...);

static void dlldbg_depth_display_last_delimiter(void);

static size_t _dlldbg_nested_depth = 0;
static void   dlldbg_depth_inc(void);
static void   dlldbg_depth_dec(void);
static void   dlldbg_depth_set(size_t depth);
static void   dlldbg_depth_reset(void);

static const char _dlldbg_nested_depth_display_prefix[]  = "⎸";
static const char _dlldbg_nested_depth_display[]         = "⎯⎯";
static const char _dlldbg_nested_depth_display_postfix[] = "▶";

static const size_t _dlldbg_nested_depth_display_prefix_len =
    sizeof(_dlldbg_nested_depth_display_prefix) - sizeof(char);
static const size_t _dlldbg_nested_depth_display_len =
    sizeof(_dlldbg_nested_depth_display) - sizeof(char);
static const size_t _dlldbg_nested_depth_display_postfix_len =
    sizeof(_dlldbg_nested_depth_display_postfix) - sizeof(char);

static void _dlldbg_atexit_clearance(void);

/**
 * ----------------------------
 * Macros definitions
 * ----------------------------
 */
#  define dll_init_logger \
    { \
      dlldbg_parent_filename_set(__FILE__); \
      dlldbg_logfile_open(); \
      atexit(_dlldbg_atexit_clearance); \
    }

#  define DLLDBG_LOG_BASE_FILENAME "libdll.debug"

#  define DLLDBG_LOG(__fmt, ...) \
    dlldbg_log_method_entry(__FILE__, __FUNCTION__, __fmt, __VA_ARGS__)
#  define DLLDBG_LOG_VOID dlldbg_log_method_entry(__FILE__, __FUNCTION__, "%s", "void")

#  define DLLDBG_LOG_OUT(__fmt, ...) \
    dlldbg_log_method_out(__FILE__, __FUNCTION__, __fmt, __VA_ARGS__)
#  define DLLDBG_LOG_OUT_NULL dlldbg_log_method_out(__FILE__, __FUNCTION__, "%p", NULL)
#  define DLLDBG_LOG_OUT_VOID dlldbg_log_method_out(__FILE__, __FUNCTION__, "%s", "void")

#  define DLLDBG_LOG_DLL_FN_FREE_FMT(__fn_free) #  __fn_free ": %s"
#  define DLLDBG_LOG_DLL_FN_FREE_ARG(__fn_free) \
    (__fn_free) == LIBDLL_FN_FREE_CLEARANCE ? "LIBDLL_FN_FREE_CLEARANCE" \
    : (__fn_free) == LIBDLL_FN_FREE_NULL    ? "LIBDLL_FN_FREE_NULL" \
                                            : "LIBDLL_FN_FREE_SET"

#  define DLLDBG_LOG_DLL_OBJ_FMT(__obj) \
#    __obj ": %p ( data: %p, data_size: %zu, prev: %p, next: %p, fn_free: %s )"
#  define DLLDBG_LOG_DLL_OBJ_ARG(__obj) \
    (__obj), (__obj) ? (__obj)->data : NULL, (__obj) ? (__obj)->data_size : 0, \
        (__obj) ? (__obj)->prev : NULL, (__obj) ? (__obj)->next : NULL, \
        (__obj) ? DLLDBG_LOG_DLL_FN_FREE_ARG((__obj)->fn_free) : NULL

#  define DLLDBG_LOG_DLL_FMT(__dll) #  __dll ": %p ( head: %p, tail: %p, objs_count: %zu)"
#  define DLLDBG_LOG_DLL_ARG(__dll) \
    (__dll), (__dll) ? (__dll)->head : NULL, (__dll) ? (__dll)->tail : NULL, \
        (__dll) ? (__dll)->objs_count : 0

#  define DLLDBG_LOG_OUT_DEPTH_INC dlldbg_depth_inc()
#  define DLLDBG_LOG_OUT_DEPTH_DEC dlldbg_depth_dec()
#  define DLLDBG_LOG_OUT_DELIMITER dlldbg_depth_display_last_delimiter()

/**
 * ----------------------------
 * Function definitions
 * ----------------------------
 */
static inline char * dlldbg_parent_filename_get(void) {
  char * out = "unknown";
  if (_dlldbg_parent_filename) {
    out = _dlldbg_parent_filename;
  }
  return out;
}
static inline void dlldbg_parent_filename_set(const char * filename) {
  const size_t filename_len = strlen(filename);

  assert((_dlldbg_parent_filename = calloc(filename_len + 1, sizeof(*filename))));
  strcpy(_dlldbg_parent_filename, filename);
}

static inline void dlldbg_logfile_open() {
  char timestamp_buff[128];
  char logfilename_buff[128];

  struct timeval tv;
  gettimeofday(&tv, NULL);
  int millisec = tv.tv_usec / 1000.0;
  if (millisec >= 1000) {
    millisec -= 1000;
    tv.tv_sec++;
  }

  strftime(
      timestamp_buff, sizeof(timestamp_buff) - 1, "%y-%m-%d-%T", localtime(&tv.tv_sec));
  snprintf(logfilename_buff,
           sizeof(logfilename_buff) - 1,
           "./%s_%s.%03d_%s.log",
           DLLDBG_LOG_BASE_FILENAME,
           timestamp_buff,
           millisec,
           dlldbg_parent_filename_get());

  assert((_dlldbg_logfile = fopen(logfilename_buff, "w+")));
  setbuf(_dlldbg_logfile, NULL);
}

static inline void dlldbg_vfprintf(const char * fmt, va_list arg) {
  if (_dlldbg_logfile) {
    vfprintf(_dlldbg_logfile, fmt, arg);
  }
}

static inline void dlldbg_fprintf(const char * fmt, ...) {
  va_list argptr;
  va_start(argptr, fmt);

  dlldbg_vfprintf(fmt, argptr);

  va_end(argptr);
}

static inline void dlldbg_fputs(const char * str) { dlldbg_fprintf("%s", str); }

static inline char * dlldbg_log_get_method_depth_fmt(void) {
  static char  depth_fmt_str[128];
  const size_t depth_fmt_str_len = sizeof(depth_fmt_str);
  const size_t depth_fmt_len = _dlldbg_nested_depth * _dlldbg_nested_depth_display_len;

  depth_fmt_str[0] = 0;
  const size_t depth_fmt_written_len =
      snprintf(depth_fmt_str, 15, "%%%zus", depth_fmt_len);
  depth_fmt_str[depth_fmt_written_len] = 0;

  return depth_fmt_str;
}

static inline char * dlldbg_log_get_method_depth_arg(void) {
  static char  depth_arg_buff[512];
  const size_t depth_arg_buff_len = sizeof(depth_arg_buff);
  static char  depth_arg_display_buff[512];
  const size_t depth_arg_display_buff_len = sizeof(depth_arg_display_buff);

  for (size_t i = 0; _dlldbg_nested_depth > i; i++) {
    const size_t buff_iptr = _dlldbg_nested_depth_display_len * i;
    strcpy(depth_arg_display_buff + buff_iptr, _dlldbg_nested_depth_display);
  }
  depth_arg_display_buff[_dlldbg_nested_depth * _dlldbg_nested_depth_display_len] = 0;

  const size_t depth_arg_written_len    = snprintf(depth_arg_buff,
                                                depth_arg_buff_len,
                                                "%s %s%s ",
                                                _dlldbg_nested_depth_display_prefix,
                                                depth_arg_display_buff,
                                                _dlldbg_nested_depth_display_postfix);
  depth_arg_buff[depth_arg_written_len] = 0;

  return depth_arg_buff;
}

static inline void dlldbg_log_method_prefix(void) {
  const char * restrict depth_fmt = dlldbg_log_get_method_depth_fmt();
  const char * restrict depth_arg = dlldbg_log_get_method_depth_arg();

  dlldbg_fprintf(depth_fmt, depth_arg);
  dlldbg_fputs("\n");
}

static inline void
    dlldbg_log_method_entry(const char * file, const char * func, const char * fmt, ...) {
  static char prefix_fmt[64];
  va_list     argptr;

  va_start(argptr, fmt);

  char * restrict depth_fmt = dlldbg_log_get_method_depth_fmt();
  char * restrict depth_arg = dlldbg_log_get_method_depth_arg();

  prefix_fmt[0] = 0;

  const size_t prefix_fmt_written_len =
      snprintf(prefix_fmt, sizeof(prefix_fmt) - 1, "%s%%s:%%s ( ", depth_fmt);

  prefix_fmt[prefix_fmt_written_len] = 0;

  dlldbg_fprintf(prefix_fmt, depth_arg, file, func);
  dlldbg_vfprintf(fmt, argptr);
  dlldbg_fprintf(" )\n");

  va_end(argptr);
}

static inline void
    dlldbg_log_method_out(const char * file, const char * func, const char * fmt, ...) {
  static char prefix_fmt[64];
  va_list     argptr;

  va_start(argptr, fmt);

  char * restrict depth_fmt = dlldbg_log_get_method_depth_fmt();
  char * restrict depth_arg = dlldbg_log_get_method_depth_arg();

  prefix_fmt[0] = 0;

  const size_t prefix_fmt_written_len =
      snprintf(prefix_fmt, sizeof(prefix_fmt) - 1, "%s%%s %%s return: ", depth_fmt);

  prefix_fmt[prefix_fmt_written_len] = 0;

  dlldbg_fprintf(prefix_fmt, depth_arg, file, func);
  dlldbg_vfprintf(fmt, argptr);
  dlldbg_fprintf(";\n");
  dlldbg_depth_display_last_delimiter();

  va_end(argptr);
}

static inline void dlldbg_depth_display_last_delimiter(void) {
  if (!_dlldbg_nested_depth) {
    dlldbg_fprintf("%s\n", _dlldbg_nested_depth_display_prefix);
  }
}

static inline void dlldbg_depth_inc(void) { _dlldbg_nested_depth += 1; }
static inline void dlldbg_depth_dec(void) {
  if (0 != _dlldbg_nested_depth) {
    _dlldbg_nested_depth -= 1;
  }
}
static inline void dlldbg_depth_set(size_t depth) { _dlldbg_nested_depth = depth; }
static inline void dlldbg_depth_reset(void) { _dlldbg_nested_depth = 0; }

static inline void _dlldbg_atexit_clearance(void) {
  if (_dlldbg_parent_filename) {
    free(_dlldbg_parent_filename);
  }

  if (_dlldbg_logfile) {
    fclose(_dlldbg_logfile);
  }

  _dlldbg_parent_filename = NULL;
  _dlldbg_logfile         = NULL;
}

#else /* if LIBDLL_DEBUG_INFO not defined */

/**
 * Dummy macroses to prevent any errors.
 */

#  define dll_init_logger ((void)0)

#  define DLLDBG_LOG_BASE_FILENAME

#  define DLLDBG_LOG(__fmt, ...)
#  define DLLDBG_LOG_VOID

#  define DLLDBG_LOG_OUT(__fmt, ...)
#  define DLLDBG_LOG_OUT_NULL
#  define DLLDBG_LOG_OUT_VOID
#  define DLLDBG_LOG_DLL_FN_FREE_FMT(__fn_free)
#  define DLLDBG_LOG_DLL_FN_FREE_ARG(__fn_free)

#  define DLLDBG_LOG_DLL_OBJ_FMT(__obj)
#  define DLLDBG_LOG_DLL_OBJ_ARG(__obj)

#  define DLLDBG_LOG_DLL_FMT(__dll)
#  define DLLDBG_LOG_DLL_ARG(__dll)

#  define DLLDBG_LOG_OUT_DEPTH_INC
#  define DLLDBG_LOG_OUT_DEPTH_DEC
#  define DLLDBG_LOG_OUT_DELIMITER

#endif /* LIBDLL_DEBUG_INFO */

#endif /* LIBDLL_LOG_H */
