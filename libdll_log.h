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

#ifdef LIBDLL_LOGGER
#  undef LIBDLL_LOGGER
#  define LIBDLL_LOGGER 1

/**
 * Use this macro with LIBDLL_LOGGER definition to see more deep log,
 * such as from internal functions in dll_sort().
 */
#  ifdef LIBDLL_LOGGER_INTERNAL
#    undef LIBDLL_LOGGER_INTERNAL
#    define LIBDLL_LOGGER_INTERNAL 1
#  endif

#  include <assert.h>
#  include <stdarg.h>
#  include <stdio.h>
#  include <sys/time.h>
#  include <sys/types.h>
#  include <time.h>

#  if !defined(LIBDLL_DESTRUCTOR_DEFAULT) && !defined(LIBDLL_DESTRUCTOR_NULL)
#    include "libdll.h"
#  endif /* !defined(LIBDLL_DESTRUCTOR_DEFAULT) && !defined(LIBDLL_DESTRUCTOR_NULL)) */

/**
 * ---------------------------------------
 * Function prototypes & global variables
 * ---------------------------------------
 */
static char * _dll_log_src_filename = NULL;
static char * dll_log_src_filename_get(void);
static void   dll_log_src_filename_set(const char * filename);

static FILE * _dll_log_file = NULL;
static void   dll_log_file_open();

static void dll_log_vfprintf(const char * fmt, va_list arg);
static void dll_log_fprintf(const char * fmt, ...);
static void dll_log_fputs(const char * str);

typedef enum _e_dll_log_severity {
  _LIBDLL_LOG_SEV_NONE = -1,
  _LIBDLL_LOG_SEV_START,
  LIBDLL_LOG_SEV_INF = _LIBDLL_LOG_SEV_START,
  LIBDLL_LOG_SEV_WRN,
  LIBDLL_LOG_SEV_ERR,
  LIBDLL_LOG_SEV_INT,
  _LIBDLL_LOG_SEV_END,
} _dll_log_severity_t;

static void dll_log(_dll_log_severity_t sev, const char * func, const char * fmt, ...);
static void dll_log_prefix(_dll_log_severity_t sev,
                           const char *        func,
                           const char * restrict postfix);

static void dll_log_method_entry(_dll_log_severity_t sev,
                                 const char *        func,
                                 const char *        fmt,
                                 ...);
static void
    dll_log_method_out(_dll_log_severity_t sev, const char * func, const char * fmt, ...);

static void dll_log_depth_display_last_delimiter(void);

static size_t _dll_log_depth = 0;
static void   dll_log_depth_inc(void);
static void   dll_log_depth_dec(void);
static void   dll_log_depth_set(size_t depth);
static size_t dll_log_depth_get(void);
static void   dll_log_depth_reset(void);

static char * dll_log_get_method_depth_fmt(void);
static char * dll_log_get_method_depth_arg(void);

static const char _dll_log_depth_display_prefix[]  = "⎸";
static const char _dll_log_depth_display[]         = "⎯⎯";
static const char _dll_log_depth_display_postfix[] = "▶";

static const size_t _dll_log_depth_display_prefix_len =
    sizeof(_dll_log_depth_display_prefix) - sizeof(char);
static const size_t _dll_log_depth_display_len =
    sizeof(_dll_log_depth_display) - sizeof(char);
static const size_t _dll_log_depth_display_postfix_len =
    sizeof(_dll_log_depth_display_postfix) - sizeof(char);

static void _dll_log_atexit_clearance(void);

/**
 * ----------------------------
 * Macros definitions
 * ----------------------------
 */
#  define dll_init_logger \
    { \
      dll_log_src_filename_set(__FILE__); \
      dll_log_file_open(); \
      atexit(_dll_log_atexit_clearance); \
    }

#  define LIBDLL_LOG_BASE_FILENAME "libdll.debug"

#  define LILIBDLL_LOG_SEV(__sev, __fmt, ...) \
    dll_log(__sev, __FUNCTION__, __fmt, __VA_ARGS__)
#  define LIBDLL_LOG(__fmt, ...) LILIBDLL_LOG_SEV(LIBDLL_LOG_SEV_INF, __fmt, __VA_ARGS__)

#  define LIBDLL_LOG_ENTRY_SEV(__sev, __fmt, ...) \
    dll_log_method_entry(__sev, __FUNCTION__, __fmt, __VA_ARGS__)
#  define LIBDLL_LOG_ENTRY(__fmt, ...) \
    LIBDLL_LOG_ENTRY_SEV(LIBDLL_LOG_SEV_INF, __fmt, __VA_ARGS__)

#  define LIBDLL_LOG_ENTRY_VOID_SEV(__sev) \
    dll_log_method_entry(__sev, __FUNCTION__, "%s", "void")
#  define LIBDLL_LOG_ENTRY_VOID LIBDLL_LOG_ENTRY_VOID_SEV(LIBDLL_LOG_SEV_INF)

#  define LIBDLL_LOG_OUT_SEV(__sev, __fmt, ...) \
    dll_log_method_out(__sev, __FUNCTION__, __fmt, __VA_ARGS__)
#  define LIBDLL_LOG_OUT(__fmt, ...) \
    LIBDLL_LOG_OUT_SEV(LIBDLL_LOG_SEV_INF, __fmt, __VA_ARGS__)

#  define LIBDLL_LOG_OUT_NULL_SEV(__sev) \
    dll_log_method_out(__sev, __FUNCTION__, "%p", NULL)
#  define LIBDLL_LOG_OUT_NULL LIBDLL_LOG_OUT_NULL_SEV(LIBDLL_LOG_SEV_INF)

#  define LIBDLL_LOG_OUT_VOID_SEV(__sev) \
    dll_log_method_out(__sev, __FUNCTION__, "%s", "void")
#  define LIBDLL_LOG_OUT_VOID LIBDLL_LOG_OUT_VOID_SEV(LIBDLL_LOG_SEV_INF)

#  define LIBDLL_LOG_DLL_DESTRUCTOR_FMT(__destructor) #  __destructor ": %s"
#  define LIBDLL_LOG_DLL_DESTRUCTOR_ARG(__destructor) \
    ((__destructor) == LIBDLL_DESTRUCTOR_DEFAULT ? "LIBDLL_DESTRUCTOR_DEFAULT" \
     : (__destructor) == LIBDLL_DESTRUCTOR_NULL  ? "LIBDLL_DESTRUCTOR_NULL" \
                                                 : "LIBDLL_DESTRUCTOR_CUSTOM")

#  define LIBDLL_LOG_DLL_OBJ_FMT(__obj) \
#    __obj ": %p ( data: %p, size: %zu, prev: %p, next: %p, destructor: %s )"
#  define LIBDLL_LOG_DLL_OBJ_ARG(__obj) \
    (__obj), (__obj) ? (__obj)->data : NULL, (__obj) ? (__obj)->size : 0, \
        (__obj) ? (__obj)->prev : NULL, (__obj) ? (__obj)->next : NULL, \
        (__obj) ? LIBDLL_LOG_DLL_DESTRUCTOR_ARG((__obj)->destructor) : NULL

#  define LIBDLL_LOG_DLL_OBJ_DATA_FMT(__obj) \
#    __obj "->data: %p ( size: %zu, destructor: %s )"
#  define LIBDLL_LOG_DLL_OBJ_DATA_ARG(__obj) \
    (__obj) ? (__obj)->data : NULL, (__obj) ? (__obj)->size : 0, \
        (__obj) ? LIBDLL_LOG_DLL_DESTRUCTOR_ARG((__obj)->destructor) : NULL

#  define LIBDLL_LOG_DLL_FMT(__dll) #  __dll ": %p ( head: %p, tail: %p, objs_count: %zu)"
#  define LIBDLL_LOG_DLL_ARG(__dll) \
    (__dll), (__dll) ? (__dll)->head : NULL, (__dll) ? (__dll)->tail : NULL, \
        (__dll) ? (__dll)->objs_count : 0

#  define LIBDLL_LOG_OUT_DEPTH_INC dll_log_depth_inc()
#  define LIBDLL_LOG_OUT_DEPTH_DEC dll_log_depth_dec()
#  define LIBDLL_LOG_OUT_DELIMITER dll_log_depth_display_last_delimiter()

/**
 * ----------------------------
 * Function definitions
 * ----------------------------
 */
static inline char * dll_log_src_filename_get(void) {
  char * out = "unknown";
  if (_dll_log_src_filename) {
    out = _dll_log_src_filename;
  }
  return out;
}
static inline void dll_log_src_filename_set(const char * filename) {
  const size_t filename_len = strlen(filename);

  assert((_dll_log_src_filename = calloc(filename_len + 1, sizeof(*filename))));

  strcpy(_dll_log_src_filename, filename);
}

static inline void dll_log_file_open() {
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
           "./%s_%s_%s.%03d.log",
           LIBDLL_LOG_BASE_FILENAME,
           dll_log_src_filename_get(),
           timestamp_buff,
           millisec);

  assert((_dll_log_file = fopen(logfilename_buff, "w+")));
  setbuf(_dll_log_file, NULL);
}

static inline void dll_log_vfprintf(const char * fmt, va_list arg) {
  if (_dll_log_file) {
    vfprintf(_dll_log_file, fmt, arg);
  }
}

static inline void dll_log_fprintf(const char * fmt, ...) {
  va_list argptr;
  va_start(argptr, fmt);

  dll_log_vfprintf(fmt, argptr);

  va_end(argptr);
}

static inline void dll_log_fputs(const char * str) { dll_log_fprintf("%s", str); }

static inline char * dll_log_get_severity_fmt(_dll_log_severity_t sev) {
  if (_LIBDLL_LOG_SEV_NONE == sev) {
    return "%s";
  }

  return "⎸%-8s⎹";
}
static inline char * dll_log_get_severity_arg(_dll_log_severity_t sev) {
  if (_LIBDLL_LOG_SEV_NONE == sev) {
    return "";
  }

  return (char * [_LIBDLL_LOG_SEV_END]){[LIBDLL_LOG_SEV_INF] = "INFO",
                                        [LIBDLL_LOG_SEV_WRN] = "WARNING",
                                        [LIBDLL_LOG_SEV_ERR] = "ERROR",
                                        [LIBDLL_LOG_SEV_INT] = "INTERNAL"}[sev];
}

static inline void dll_log_prefix(_dll_log_severity_t sev,
                                  const char *        func,
                                  const char * restrict postfix) {
  static char prefix_fmt[256];

  const char * restrict depth_fmt = dll_log_get_method_depth_fmt();
  const char * restrict depth_arg = dll_log_get_method_depth_arg();

  const char * const restrict sev_fmt = dll_log_get_severity_fmt(sev);
  const char * const restrict sev_arg = dll_log_get_severity_arg(sev);

  prefix_fmt[0] = 0;

  const size_t prefix_fmt_written_len = snprintf(prefix_fmt,
                                                 sizeof(prefix_fmt) - 1,
                                                 "%s%s %%s%s ",
                                                 depth_fmt,
                                                 sev_fmt,
                                                 postfix ? postfix : "");

  prefix_fmt[prefix_fmt_written_len] = 0;

  dll_log_fprintf(prefix_fmt, depth_arg, sev_arg, func);
}

static inline void
    dll_log(_dll_log_severity_t sev, const char * func, const char * fmt, ...) {
  va_list argptr;

  va_start(argptr, fmt);

  dll_log_prefix(sev, func, NULL);
  dll_log_vfprintf(fmt, argptr);
  dll_log_fputs("\n");

  va_end(argptr);
}

static inline void dll_log_method_entry(_dll_log_severity_t sev,
                                        const char *        func,
                                        const char *        fmt,
                                        ...) {
  va_list argptr;

  va_start(argptr, fmt);

  dll_log_prefix(sev, func, NULL);
  dll_log_vfprintf(fmt, argptr);
  dll_log_fprintf(" )\n");

  va_end(argptr);
}

static inline void dll_log_method_out(_dll_log_severity_t sev,
                                      const char *        func,
                                      const char *        fmt,
                                      ...) {
  va_list argptr;

  va_start(argptr, fmt);

  dll_log_prefix(sev, func, " return:");
  dll_log_vfprintf(fmt, argptr);
  dll_log_fprintf(";\n");
  dll_log_depth_display_last_delimiter();

  va_end(argptr);
}

static inline void dll_log_depth_display_last_delimiter(void) {
  if (!_dll_log_depth) {
    dll_log_fprintf("%s\n", _dll_log_depth_display_prefix);
  }
}

static inline void dll_log_depth_inc(void) { _dll_log_depth += 1; }
static inline void dll_log_depth_dec(void) {
  if (0 != _dll_log_depth) {
    _dll_log_depth -= 1;
  }
}
static inline void   dll_log_depth_set(size_t depth) { _dll_log_depth = depth; }
static inline size_t dll_log_depth_get(void) {
  return _dll_log_depth ? _dll_log_depth : 1;
}
static inline void dll_log_depth_reset(void) { _dll_log_depth = 0; }

static inline char * dll_log_get_method_depth_fmt(void) {
  static char  depth_fmt_str[128];
  const size_t depth_fmt_str_len = sizeof(depth_fmt_str);
  const size_t depth_fmt_len     = _dll_log_depth_display_prefix_len +
                               (_dll_log_depth_display_len * dll_log_depth_get()) +
                               _dll_log_depth_display_postfix_len + 2;

  depth_fmt_str[0] = 0;
  const size_t depth_fmt_written_len =
      snprintf(depth_fmt_str, depth_fmt_str_len - 1, "%%%zus", depth_fmt_len);
  depth_fmt_str[depth_fmt_written_len] = 0;

  return depth_fmt_str;
}

static inline char * dll_log_get_method_depth_arg(void) {
  static char  depth_arg_buff[512];
  const size_t depth_arg_buff_len = sizeof(depth_arg_buff);
  static char  depth_arg_display_buff[512];
  const size_t depth_arg_display_buff_len = sizeof(depth_arg_display_buff);
  const size_t depth                      = dll_log_depth_get();

  for (size_t i = 0; depth > i; i++) {
    const size_t buff_iptr = _dll_log_depth_display_len * i;
    strcpy(depth_arg_display_buff + buff_iptr, _dll_log_depth_display);
  }
  depth_arg_display_buff[_dll_log_depth_display_len * depth] = 0;

  const size_t depth_arg_written_len    = snprintf(depth_arg_buff,
                                                depth_arg_buff_len,
                                                "%s %s%s ",
                                                _dll_log_depth_display_prefix,
                                                depth_arg_display_buff,
                                                _dll_log_depth_display_postfix);
  depth_arg_buff[depth_arg_written_len] = 0;

  return depth_arg_buff;
}

static inline void _dll_log_atexit_clearance(void) {
  if (_dll_log_src_filename) {
    free(_dll_log_src_filename);
  }

  if (_dll_log_file) {
    fclose(_dll_log_file);
  }

  _dll_log_src_filename = NULL;
  _dll_log_file         = NULL;
}

#else /* if LIBDLL_LOGGER not defined */

/**
 * Dummy macroses to prevent any errors.
 */

#  define dll_init_logger ((void)0)

#  define LIBDLL_LOG_BASE_FILENAME

#  define LILIBDLL_LOG_SEV(__sev, __fmt, ...)
#  define LIBDLL_LOG(__fmt, ...)

#  define LIBDLL_LOG_ENTRY_SEV(__sev, __fmt, ...)
#  define LIBDLL_LOG_ENTRY(__fmt, ...)

#  define LIBDLL_LOG_ENTRY_VOID_SEV(__sev)
#  define LIBDLL_LOG_ENTRY_VOID

#  define LIBDLL_LOG_OUT_SEV(__sev, __fmt, ...)
#  define LIBDLL_LOG_OUT(__fmt, ...)

#  define LIBDLL_LOG_OUT_NULL_SEV(__sev)
#  define LIBDLL_LOG_OUT_NULL

#  define LIBDLL_LOG_OUT_VOID_SEV(__sev)
#  define LIBDLL_LOG_OUT_VOID

#  define LIBDLL_LOG_DLL_DESTRUCTOR_FMT(__destructor)
#  define LIBDLL_LOG_DLL_DESTRUCTOR_ARG(__destructor)

#  define LIBDLL_LOG_DLL_OBJ_FMT(__obj)
#  define LIBDLL_LOG_DLL_OBJ_ARG(__obj)

#  define LIBDLL_LOG_DLL_OBJ_DATA_FMT(__obj)
#  define LIBDLL_LOG_DLL_OBJ_DATA_ARG(__obj)

#  define LIBDLL_LOG_DLL_FMT(__dll)
#  define LIBDLL_LOG_DLL_ARG(__dll)

#  define LIBDLL_LOG_OUT_DEPTH_INC
#  define LIBDLL_LOG_OUT_DEPTH_DEC
#  define LIBDLL_LOG_OUT_DELIMITER

#endif /* LIBDLL_LOGGER */

/**
 * -------------------------------
 * Internal-specific log macroses
 * -------------------------------
 */
#if 1 == LIBDLL_LOGGER && 1 == LIBDLL_LOGGER_INTERNAL

#  define LIBDLL_INTERNAL_LOG_SEV(__sev, __fmt, ...) \
    LILIBDLL_LOG_SEV(__sev, __fmt, __VA_ARGS__)
#  define LIBDLL_INTERNAL_LOG(__fmt, ...) \
    LIBDLL_INTERNAL_LOG_SEV(LIBDLL_LOG_SEV_INT, __fmt, __VA_ARGS__)

#  define LIBDLL_INTERNAL_LOG_ENTRY_SEV(__sev, __fmt, ...) \
    LIBDLL_LOG_ENTRY_SEV(__sev, __fmt, __VA_ARGS__)
#  define LIBDLL_INTERNAL_LOG_ENTRY(__fmt, ...) \
    LIBDLL_INTERNAL_LOG_ENTRY_SEV(LIBDLL_LOG_SEV_INT, __fmt, __VA_ARGS__)

#  define LIBDLL_INTERNAL_LOG_ENTRY_VOID_SEV(__sev) LIBDLL_LOG_ENTRY_VOID_SEV(__sev)
#  define LIBDLL_INTERNAL_LOG_ENTRY_VOID \
    LIBDLL_INTERNAL_LOG_ENTRY_VOID_SEV(LIBDLL_LOG_SEV_INT)

#  define LIBDLL_INTERNAL_LOG_OUT_SEV(__sev, __fmt, ...) \
    LIBDLL_LOG_OUT_SEV(__sev, __fmt, __VA_ARGS__)
#  define LIBDLL_INTERNAL_LOG_OUT(__fmt, ...) \
    LIBDLL_INTERNAL_LOG_OUT_SEV(LIBDLL_LOG_SEV_INT, __fmt, __VA_ARGS__)

#  define LIBDLL_INTERNAL_LOG_OUT_NULL_SEV(__sev) LIBDLL_LOG_OUT_NULL(__sev)
#  define LIBDLL_INTERNAL_LOG_OUT_NULL \
    LIBDLL_INTERNAL_LOG_OUT_NULL_SEV(LIBDLL_LOG_SEV_INT)

#  define LIBDLL_INTERNAL_LOG_OUT_VOID_SEV(__sev) LIBDLL_LOG_OUT_VOID_SEV(__sev)
#  define LIBDLL_INTERNAL_LOG_OUT_VOID \
    LIBDLL_INTERNAL_LOG_OUT_VOID_SEV(LIBDLL_LOG_SEV_INT)

#  define LIBDLL_INTERNAL_LOG_DLL_DESTRUCTOR_FMT(__destructor) \
    LIBDLL_LOG_DLL_DESTRUCTOR_FMT(__destructor)
#  define LIBDLL_INTERNAL_LOG_DLL_DESTRUCTOR_ARG(__destructor) \
    LIBDLL_LOG_DLL_DESTRUCTOR_ARG(__destructor)

#  define LIBDLL_INTERNAL_LOG_DLL_OBJ_FMT(__obj) LIBDLL_LOG_DLL_OBJ_FMT(__obj)
#  define LIBDLL_INTERNAL_LOG_DLL_OBJ_ARG(__obj) LIBDLL_LOG_DLL_OBJ_ARG(__obj)

#  define LIBDLL_INTERNAL_LOG_DLL_OBJ_DATA_FMT(__obj) LIBDLL_LOG_DLL_OBJ_DATA_FMT(__obj)
#  define LIBDLL_INTERNAL_LOG_DLL_OBJ_DATA_ARG(__obj) LIBDLL_LOG_DLL_OBJ_DATA_ARG(__obj)

#  define LIBDLL_INTERNAL_LOG_DLL_FMT(__dll) LIBDLL_LOG_DLL_FMT(__dll)
#  define LIBDLL_INTERNAL_LOG_DLL_ARG(__dll) LIBDLL_LOG_DLL_ARG(__dll)

#  define LIBDLL_INTERNAL_LOG_OUT_DEPTH_INC LIBDLL_LOG_OUT_DEPTH_INC
#  define LIBDLL_INTERNAL_LOG_OUT_DEPTH_DEC LIBDLL_LOG_OUT_DEPTH_DEC
#  define LIBDLL_INTERNAL_LOG_OUT_DELIMITER LIBDLL_LOG_OUT_DELIMITER

#else
#  define LIBDLL_INTERNAL_LOG_SEV(__sev, __fmt, ...)
#  define LIBDLL_INTERNAL_LOG(__fmt, ...)

#  define LIBDLL_INTERNAL_LOG_ENTRY_SEV(__sev, __fmt, ...)
#  define LIBDLL_INTERNAL_LOG_ENTRY(__fmt, ...)

#  define LIBDLL_INTERNAL_LOG_ENTRY_VOID_SEV(__sev)
#  define LIBDLL_INTERNAL_LOG_ENTRY_VOID

#  define LIBDLL_INTERNAL_LOG_OUT_SEV(__sev, __fmt, ...)
#  define LIBDLL_INTERNAL_LOG_OUT(__fmt, ...)

#  define LIBDLL_INTERNAL_LOG_OUT_NULL_SEV(__sev)
#  define LIBDLL_INTERNAL_LOG_OUT_NULL

#  define LIBDLL_INTERNAL_LOG_OUT_VOID_SEV(__sev)
#  define LIBDLL_INTERNAL_LOG_OUT_VOID

#  define LIBDLL_INTERNAL_LOG_DLL_DESTRUCTOR_FMT(__destructor)
#  define LIBDLL_INTERNAL_LOG_DLL_DESTRUCTOR_ARG(__destructor)

#  define LIBDLL_INTERNAL_LOG_DLL_OBJ_FMT(__obj)
#  define LIBDLL_INTERNAL_LOG_DLL_OBJ_ARG(__obj)

#  define LIBDLL_INTERNAL_LOG_DLL_OBJ_DATA_FMT(__obj)
#  define LIBDLL_INTERNAL_LOG_DLL_OBJ_DATA_ARG(__obj)

#  define LIBDLL_INTERNAL_LOG_DLL_FMT(__dll)
#  define LIBDLL_INTERNAL_LOG_DLL_ARG(__dll)

#  define LIBDLL_INTERNAL_LOG_OUT_DEPTH_INC
#  define LIBDLL_INTERNAL_LOG_OUT_DEPTH_DEC
#  define LIBDLL_INTERNAL_LOG_OUT_DELIMITER

#endif

#endif /* LIBDLL_LOG_H */
