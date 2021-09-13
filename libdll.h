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

#ifndef LIBDLL_H
#define LIBDLL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/**
 * ----------------------------
 * libdll specifications and macroses
 * ----------------------------
 */

/*
 * Define LIBDLL_DEBUG_INFO before libdll.h include to make logger macroses work.
 */
#include "libdll_log.h"

#ifdef LIBDLL_UNSAFE_USAGE
#  undef LIBDLL_UNSAFE_USAGE

/**
 * \brief Removing all nullability checks in all functions.
 *  May cause better performance but undefined behavior as well.
 */
#  define LIBDLL_UNSAFE_USAGE 1

#endif /* LIBDLL_UNSAFE_USAGE */

/**
 * \brief Use this macros as \c fn_free argument for \c dll_new_obj if you don't allocate
 * anything inside a \c data, but the \c data itself was allocated.
 */
#define LIBDLL_FN_FREE_CLEARANCE ((dll_callback_fn_t)-0x1UL)

/**
 * \brief Use this macros as \c fn_free argument for \c dll_new_obj if you don't need to
 * free any memory in lists.
 */
#define LIBDLL_FN_FREE_NULL ((dll_callback_fn_t)NULL)

/**
 * ----------------------------
 * Data structure definitions
 * ----------------------------
 */

/**
 * \brief A callback funcion typedef for comparing \c data from 2 list-objects.
 *  Or list-object with any other given data by user.
 *  For example: \a dll_remove or \a dll_sort.
 *
 * \param obj_data list-object data.
 * \param other other list-object data.
 *
 * \return comparing result value.
 */
typedef ssize_t (*dll_callback_cmp_fn_t)(void * restrict obj_data, void * restrict other);

/**
 * \brief A callback function typedef for interact with list-object data.
 *
 * \param obj_data list-object data.
 */
typedef void (*dll_callback_fn_t)(void * restrict obj_data);

/**
 * \brief A list-object structure.
 *
 * \param next a pointer to next list-object.
 * \param prev a pointer to previous list-object.
 * \param data an any user defined data.
 * \param fn_free callback-function to manually free anything inside \c data on deleting
 * the list-object. Notice: \c data itself will be freed automatically with \c free if a
 * \c fn_free function was specified. Use \c LIBDLL_FN_FREE_CLEARANCE if you don't
 * allocate anything inside a \c data, but the \c data itself was allocated. \param
 * data_size a \c data size.
 */
typedef struct s_dll_obj {
  struct s_dll_obj * restrict next; /** a pointer to next list-object. */
  struct s_dll_obj * restrict prev; /** a pointer to previous list-object. */
  void * restrict data;             /** an any user defined data. */
  dll_callback_fn_t
      fn_free; /** callback-function to manually free anything inside \c data on deleting
                  the list-object. Notice: \c data itself will be freed automatically with
                  \c free if a \c fn_free function was specified. Use \c
                  LIBDLL_FN_FREE_CLEARANCE if you don't allocate anything inside a \c
                  data, but the \c data itself was allocated.*/
  size_t data_size; /** a \c data size. */
} __attribute__((aligned(__BIGGEST_ALIGNMENT__))) dll_obj_t;

/**
 * \brief A doubly linked list structure.
 *
 * \param head a head of list.
 * \param tail a tail of list.
 * \param objs_count a counter of list-objects in list.
 */
typedef struct s_dll {
  dll_obj_t * restrict head; /** a head of list. */
  dll_obj_t * restrict tail; /** a tail of list. */
  size_t objs_count;         /** a counter of list-objects in list. */
} __attribute__((aligned(__BIGGEST_ALIGNMENT__))) dll_t;

/**
 * ----------------------------
 * Function prototypes
 * ----------------------------
 */

/**
 * \brief Initialize your list first with this function.
 *
 * \return allocated memory for new list.
 */
static inline dll_t * dll_init(void);

/**
 * \brief Creating a new list-object from given parameters.
 * Strongly recomended to setup \c fn_free if you allocate memory inside \c data.
 *
 * \param data any data you want to put inside of list.
 * \param size size of \c data.
 * \param fn_free callback-function to manually free anything inside \c data on deleting
 * the list-object. Notice: \c data itself will be freed automatically with \c free if a
 * \c fn_free function was specified. Use \c LIBDLL_FN_FREE_CLEARANCE if you don't
 * allocate anything inside a \c data, but the \c data itself was allocated.
 *
 * \return a new created object.
 */
static inline dll_obj_t *
    dll_new_obj(void * restrict data, size_t size, dll_callback_fn_t fn_free);

/**
 * \brief Push a \c list-object to front of given \c list.
 *
 * \param dll destination list.
 * \param obj list-object.
 *
 * \return \c obj
 */
static inline dll_obj_t * dll_push_front(dll_t * restrict dll, dll_obj_t * restrict obj);

/**
 * \brief Push a \c obj list-object to end of given \c dll list.
 *
 * \param dll destination list.
 * \param obj list-object.
 *
 * \return \c obj
 */
static inline dll_obj_t * dll_push_back(dll_t * restrict dll, dll_obj_t * restrict obj);

/**
 * \brief Creating a new list-object via \c dll_new_obj and pushing it in front of list \c
 * dll.
 *
 * \param dll destination list.
 * \param data any data you want to put inside of list.
 * \param size size of \c data.
 * \param fn_free callback-function to manually free anything inside \c data on deleting
 * the list-object. Notice: \c data itself will be freed automatically with \c free if a
 * \c fn_free function was specified. Use \c LIBDLL_FN_FREE_CLEARANCE if you don't
 * allocate anything inside a \c data, but the \c data itself was allocated.
 *
 * \return a new created list-object.
 */
static inline dll_obj_t * dll_emplace_front(dll_t * restrict dll,
                                            void * restrict data,
                                            size_t            size,
                                            dll_callback_fn_t fn_free);

/**
 * \brief Creating a new list-object via \c dll_new_obj and pushing it in end of list \c
 * dll.
 *
 * \param dll destination list.
 * \param data any data you want to put inside of list.
 * \param size size of \c data.
 * \param fn_free callback-function to manually free anything inside \c data on deleting
 * the list-object. Notice: \c data itself will be freed automatically with \c free if a
 * \c fn_free function was specified. Use \c LIBDLL_FN_FREE_CLEARANCE if you don't
 * allocate anything inside a \c data, but the \c data itself was allocated.
 *
 * \return a new created list-object.
 */
static inline dll_obj_t * dll_emplace_back(dll_t * restrict dll,
                                           void * restrict data,
                                           size_t            size,
                                           dll_callback_fn_t fn_free);

/**
 * \brief Removes the first list-object of the list.
 *
 * \param dll list.
 */
static inline void dll_pop_front(dll_t * restrict dll);

/**
 * \brief Removes the last(tail) list-object of the list.
 *
 * \param dll list.
 */
static inline void dll_pop_back(dll_t * restrict dll);

/**
 * \brief Erases all elements from the \c dll list. After this call, \c dll_size returns
 * zero. Any pointers referring to contained elements can be invalid if you specified a \c
 * fn_free callback-function for each individual list-object. Also lost memory leaks
 * errors can occur as well if you don't specified a \c fn_free function for each
 * individual list-object but memory inside of this list-object was allocated.
 *
 * \param dll list to be cleared.
 */
static inline void dll_clear(dll_t * restrict dll);

/**
 * \brief Inserts list-object \c obj on the specified location \c pos in the list \c dll.
 *
 * \param dll list in which list-object will be inserted.
 * \param obj list-object to be inserted.
 * \param pos position in list on which list-object will be inserted(starts from 0 to \c
 * dll_size).
 *
 * \return inserted list-object or NULL if something is wrong.
 */
static inline dll_obj_t *
    dll_insert(dll_t * restrict dll, dll_obj_t * restrict obj, size_t pos);

/**
 * \brief Inserts a new created list-object via \c dll_new_obj on the specified location
 * \c pos in the list \c dll.
 *
 * \param dll list in which list-object will be inserted.
 * \param data any data you want to put inside of list.
 * \param size size of \c data.
 * \param fn_free callback-function to manually free anything inside \c data on deleting
 * the list-object. Notice: \c data itself will be freed automatically with \c free if a
 * \c fn_free function was specified. Use \c LIBDLL_FN_FREE_CLEARANCE if you don't
 * allocate anything inside a \c data, but the \c data itself was allocated. \param pos
 * position in list on which list-object will be inserted(starts from 0 to \c dll_size).
 *
 * \return inserted list-object or NULL if something is wrong.
 */
static inline dll_obj_t * dll_emplace(dll_t * restrict dll,
                                      void * restrict data,
                                      size_t            size,
                                      dll_callback_fn_t fn_free,
                                      size_t            pos);

/**
 * \brief Erases the specified list-objects from the list.
 *
 * \param dll list from which specified list-objects will be deleted.
 * \param pos_start Removes the element at \c pos. Or \c start of the range.
 * \param end End of the range. If \c end is zero or bigger than \c dll_size then only 1
 * list-object at \c pos_start will be erased.
 *
 * \return a last(tail) object after deleting range or NULL if: \c dll list has no
 * objects; \c pos_start bigger than list-objects count in \c dll list; \c end is not zero
 * and bigger than \c pos_start.
 */
static inline dll_obj_t * dll_erase(dll_t * restrict dll, size_t pos_start, size_t end);

/**
 * \brief call a \c fn callback-function for each list-object in list \c dll.
 * Function works only if \c dll and \c fn is not a NULL.
 *
 * \param dll list.
 * \param fn function for each list-object.
 */
static inline void dll_foreach(const dll_t * restrict dll, dll_callback_fn_t fn);

/**
 * \brief Access the first(head) list-object in \c dll list.
 *
 * \param dll list.
 *
 * \return Pointer to the first(head) list-object.
 */
static inline dll_obj_t * dll_front(const dll_t * restrict dll);

/**
 * \brief Access the last(tail) list-object in \c dll list.
 *
 * \param dll list.
 *
 * \return Pointer to the last(tail) list-object.
 */
static inline dll_obj_t * dll_back(const dll_t * restrict dll);

/**
 * \brief Checks whether the list \c dll is empty.
 *
 * \param dll list to check.
 *
 * \return true if the list is empty, false otherwise.
 */
static inline bool dll_empty(const dll_t * restrict dll);

/**
 * \brief Returns the number of elements in the list \c dll.
 *
 * \param dll list.
 *
 * \return The number of elements in the list.
 */
static inline size_t dll_size(const dll_t * restrict dll);

/**
 * \brief Merges two lists into a \c dst. After merging \c dst list will be sorted if you
 * specify a \c fn_sort function.
 *
 * \param dst destination list.
 * \param src second list. This list will be clear after merging to \c dst.
 * \param fn_sort optional argument. if \c fn_sort specified then \c dst after merging
 * will be sorted.
 */
static inline void
    dll_merge(dll_t * restrict dst, dll_t * restrict src, dll_callback_cmp_fn_t fn_sort);

/**
 * \brief Transfers list-objects from \c src to \c dst.
 *
 * \param dst destination list.
 * \param src second list. Full or part of this list will be clear after applying this
 * function. \param dst_pos to what position in list \c dst - list \src will be
 * transfered. (starts from 1) \param src_start start position of list \c src from which
 * list-objects will be transefered to \c dst. (starts from 1) \param src_end \c src end
 * position of transfering from a list \c src. (starts from 1)
 *
 * \exception If \c dst_pos, \c src_start, and \c src_end are zero, OR \a dst_pos, \a
 * src_start are non zero but \a src_end are zero - then just a \c dll_merge will be
 * called. \exception If \c dst or \c src is NULL then function will do nothing.
 * \exception If \c src_end less than or equal to \c src_start then function will do
 * nothing.
 */
static inline void dll_splice(dll_t * restrict dst,
                              dll_t * restrict src,
                              size_t dst_pos,
                              size_t src_start,
                              size_t src_end);

/**
 * \brief Removes all list-objects satisfying specific criteria \a value via \a fn_cmp
 * from \a dll. Applies for all list-objects for which \a fn_cmp return a positive value.
 *
 * \param dll list from which list-objects will be removed.
 * \param value any data to be compared with each list-object.
 * \param fn_cmp comparator for list-objects data and given \a value.
 *
 * \return count of removed objects from list \a dll.
 */
static inline size_t
    dll_remove(dll_t * restrict dll, void * restrict value, dll_callback_cmp_fn_t fn_cmp);

/**
 * \brief Reverses the order of the list-objects in the list.
 *
 * \param dll list to be reversed.
 */
static inline void dll_reverse(dll_t * restrict dll);

/**
 * \brief Removes all consecutive duplicate list-objects from the list.
 *  Only the first list-object in each group of equal list-objects is left.
 *  list-objects compares via \a fn_cmp and only if \a fn_cmp returns zero means the
 * list-objects are equal.
 *
 * \param dll list to be checked for duplicates.
 * \param fn_cmp comparator for list-objects.
 *
 * \return count of removed objects from list \a dll.
 */
static inline size_t dll_unique(dll_t * restrict dll, dll_callback_cmp_fn_t fn_cmp);

/**
 * \brief Sorts all the list-objects via \c fn_sort in \c dll list using a recursive merge
 * sort.
 *
 * \param dll list to be sorted.
 * \param fn_sort callback-function to compare list-objects.
 */
static inline void dll_sort(dll_t * restrict dll, dll_callback_cmp_fn_t fn_sort);

/**
 * \brief Compares two lists is they are equals
 *
 * \param dll_a first list to compare
 * \param dll_b second list to compare
 * \param fn_cmp if set then list-objects compare by it's data
 *
 * \return true if lists are equals
 */
static inline bool dll_is_equal(const dll_t * restrict dll_a,
                                const dll_t * restrict dll_b,
                                dll_callback_cmp_fn_t fn_cmp);

/**
 * \brief Compares two lists is they are not equals
 * *using dll_is_equals
 *
 * \param dll_a first list to compare
 * \param dll_b second list to compare
 * \param fn_cmp if set then list-objects compare by it's data
 *
 * \return true if lists are not equals
 */
static inline bool dll_is_not_equal(const dll_t * restrict dll_a,
                                    const dll_t * restrict dll_b,
                                    dll_callback_cmp_fn_t fn_cmp);

/**
 * \brief Unlink and delete a list-object \c obj from list \c dll.
 *
 * \param dll list from which \c obj will be deleted.
 * \param obj list-object to be deleted.
 */
static inline void dll_del(dll_t * restrict dll, dll_obj_t * restrict obj);

/**
 * \brief Unlink(only remove from list but not delete) a list-object \c obj from list \c
 * dll.
 *
 * \param dll list from which \c obj will be unlinked.
 * \param obj list-object to be unlinked.
 *
 * \return unlinked \c obj.
 */
static inline dll_obj_t * dll_unlink(dll_t * restrict dll, dll_obj_t * restrict obj);

/**
 * \brief Deleting a list-object and data on it.
 *  BE CERAFUL - It's doesn't works with links to next and previous list-objects,
 *   so calling this function without \c dll_unlink may cause SIGSEGV in future work with
 * list. Better soultion may be for you to use a \c dll_del.
 *
 * \param obj a pointer to list-object to be deleted.
 */
static inline void dll_free_obj(dll_obj_t * restrict * restrict obj);

/**
 * \brief Delete the whole list with all list-objects linked to this list at its data.
 *
 * \param dll a pointer to list to be deleted.
 */
static inline void dll_free(dll_t * restrict * restrict dll);

/**
 * ----------------------------
 * Macros definitions
 * ----------------------------
 */

#ifdef __glibc_likely
#  define __dll_likely(_cond) __glibc_likely(_cond)
#else
#  define __dll_likely(_cond) (_cond)
#endif

#ifdef __glibc_unlikely
#  define __dll_unlikely(_cond) __glibc_unlikely(_cond)
#else
#  define __dll_unlikely(_cond) (_cond)
#endif

/**
 * ----------------------------
 * Function definitions
 * ----------------------------
 */

static inline dll_t * dll_init(void) {
  DLLDBG_LOG_VOID;

  dll_t * restrict out = calloc(1, sizeof(*out));

  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_FMT(out), DLLDBG_LOG_DLL_ARG(out));
  return out;
}

static inline dll_obj_t *
    dll_new_obj(void * restrict data, size_t size, dll_callback_fn_t fn_free) {
  DLLDBG_LOG("data: %p, size: %zu, " DLLDBG_LOG_DLL_FN_FREE_FMT(fn_free),
             data,
             size,
             DLLDBG_LOG_DLL_FN_FREE_ARG(fn_free));

  dll_obj_t * restrict out = calloc(1, sizeof(*out));

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_likely(NULL != out)) {
#endif /* LIBDLL_UNSAFE_USAGE */

    out->data      = data;
    out->data_size = size;
    out->fn_free   = fn_free;

#ifndef LIBDLL_UNSAFE_USAGE
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(out), DLLDBG_LOG_DLL_OBJ_ARG(out));
  return out;
}

static inline dll_obj_t * dll_push_front(dll_t * restrict dll, dll_obj_t * restrict obj) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", " DLLDBG_LOG_DLL_OBJ_FMT(obj),
             DLLDBG_LOG_DLL_ARG(dll),
             DLLDBG_LOG_DLL_OBJ_ARG(obj));

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == obj)) {
    DLLDBG_LOG_OUT_NULL;
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  obj->next = NULL;
  obj->prev = NULL;

  ++dll->objs_count;
  if (NULL == dll->head) {
    dll->head = dll->tail = obj;
  } else {
    dll->head->prev = obj;
    obj->next       = dll->head;
    dll->head       = obj;
  }

  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(obj), DLLDBG_LOG_DLL_OBJ_ARG(obj));
  return obj;
}

static inline dll_obj_t * dll_push_back(dll_t * restrict dll, dll_obj_t * restrict obj) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", " DLLDBG_LOG_DLL_OBJ_FMT(obj),
             DLLDBG_LOG_DLL_ARG(dll),
             DLLDBG_LOG_DLL_OBJ_ARG(obj));

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == obj)) {
    DLLDBG_LOG_OUT_NULL;
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  obj->next = NULL;
  obj->prev = NULL;

  ++dll->objs_count;
  if (NULL == dll->head) {
    dll->head = dll->tail = obj;
  } else {
    dll->tail->next = obj;
    obj->prev       = dll->tail;
    dll->tail       = obj;
  }

  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(obj), DLLDBG_LOG_DLL_OBJ_ARG(obj));
  return obj;
}

static inline dll_obj_t * dll_emplace_front(dll_t * restrict dll,
                                            void * restrict data,
                                            size_t            size,
                                            dll_callback_fn_t fn_free) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", data: %p, size: %zu, " DLLDBG_LOG_DLL_FN_FREE_FMT(
                 fn_free),
             DLLDBG_LOG_DLL_ARG(dll),
             data,
             size,
             DLLDBG_LOG_DLL_FN_FREE_ARG(fn_free));
  DLLDBG_LOG_OUT_DEPTH_INC;

  dll_obj_t * restrict new_obj = dll_new_obj(data, size, fn_free);
  dll_obj_t * restrict out     = dll_push_front(dll, new_obj);

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(out), DLLDBG_LOG_DLL_OBJ_ARG(out));
  return out;
}

static inline dll_obj_t * dll_emplace_back(dll_t * restrict dll,
                                           void * restrict data,
                                           size_t            size,
                                           dll_callback_fn_t fn_free) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", data: %p, size: %zu, " DLLDBG_LOG_DLL_FN_FREE_FMT(
                 fn_free),
             DLLDBG_LOG_DLL_ARG(dll),
             data,
             size,
             DLLDBG_LOG_DLL_FN_FREE_ARG(fn_free));
  DLLDBG_LOG_OUT_DEPTH_INC;

  dll_obj_t * restrict new_obj = dll_new_obj(data, size, fn_free);
  dll_obj_t * restrict out     = dll_push_back(dll, new_obj);

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(out), DLLDBG_LOG_DLL_OBJ_ARG(out));
  return out;
}

static inline void dll_pop_front(dll_t * restrict dll) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll), DLLDBG_LOG_DLL_ARG(dll));

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == dll->head)) {
    DLLDBG_LOG_OUT_VOID;
    return;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT_DEPTH_INC;

  dll_obj_t * restrict save = dll->head->next;

  dll_del(dll, dll->head);
  dll->head = save;

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT_VOID;
}

static inline void dll_pop_back(dll_t * restrict dll) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll), DLLDBG_LOG_DLL_ARG(dll));

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == dll->tail)) {
    DLLDBG_LOG_OUT_VOID;
    return;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT_DEPTH_INC;
  DLLDBG_LOG_OUT_VOID;

  dll_obj_t * restrict save = dll->tail->prev;

  dll_del(dll, dll->tail);
  dll->tail = save;

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT_VOID;
}

static inline void dll_clear(dll_t * restrict dll) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll), DLLDBG_LOG_DLL_ARG(dll));

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll)) {
    DLLDBG_LOG_OUT_VOID;
    return;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT_DEPTH_INC;

  while (dll->objs_count) {
    dll_pop_front(dll);
  }

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT_VOID;
}

static inline dll_obj_t *
    dll_insert(dll_t * restrict dll, dll_obj_t * restrict obj, size_t pos) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", " DLLDBG_LOG_DLL_OBJ_FMT(obj) ", pos: %zu",
             DLLDBG_LOG_DLL_ARG(dll),
             DLLDBG_LOG_DLL_OBJ_ARG(obj),
             pos);

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == obj)) {
    DLLDBG_LOG_OUT_NULL;
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  if (dll->objs_count < pos) {
    DLLDBG_LOG_OUT_NULL;
    return NULL;
  }

  DLLDBG_LOG_OUT_DEPTH_INC;

  dll_obj_t * restrict out = NULL;

  if (NULL == dll->head || 0 == pos) {
    out = dll_emplace_front(dll, obj->data, obj->data_size, obj->fn_free);
  } else {
    dll_obj_t * restrict iter = dll->head;

    for (size_t i = 0; (pos - 1) > i && iter; ++i) {
      iter = iter->next;
    }

    obj->next = iter->next;
    obj->prev = iter;
    if (iter->next) {
      iter->next->prev = obj;
    }
    iter->next = obj;

    ++dll->objs_count;

    out = obj;
  }

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(out), DLLDBG_LOG_DLL_OBJ_ARG(out));
  return out;
}

static inline dll_obj_t * dll_emplace(dll_t * restrict dll,
                                      void * restrict data,
                                      size_t            size,
                                      dll_callback_fn_t fn_free,
                                      size_t            pos) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", data: %p, size: %zu, " DLLDBG_LOG_DLL_FN_FREE_FMT(
                 fn_free) ", pos: %zu",
             DLLDBG_LOG_DLL_ARG(dll),
             data,
             size,
             DLLDBG_LOG_DLL_FN_FREE_ARG(fn_free),
             pos);
  DLLDBG_LOG_OUT_DEPTH_INC;

  dll_obj_t * restrict new_obj = dll_new_obj(data, size, fn_free);
  dll_obj_t * out              = dll_insert(dll, new_obj, pos);

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(out), DLLDBG_LOG_DLL_OBJ_ARG(out));
  return out;
}

static inline dll_obj_t * __dlli_get_obj_at_index(dll_t * restrict dll, size_t pos) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", pos: %zu", DLLDBG_LOG_DLL_ARG(dll), pos);

  const size_t dll_size    = dll->objs_count;
  dll_obj_t * restrict out = NULL;

  if ((dll_size / 2) >= pos) {
    out = dll->head;
    for (size_t i = 0; out && pos > i; ++i) {
      out = out->next;
    }
  } else {
    out = dll->tail;
    for (size_t i = dll_size ? dll_size - 1 : dll_size; out && pos < i; --i) {
      out = out->prev;
    }
  }

  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(out), DLLDBG_LOG_DLL_OBJ_ARG(out));
  return out;
}

static inline dll_obj_t * dll_erase(dll_t * restrict dll, size_t pos_start, size_t end) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", pos_start: %zu, end: %zu",
             DLLDBG_LOG_DLL_ARG(dll),
             pos_start,
             end);

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll)) {
    DLLDBG_LOG_OUT_NULL;
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  if (0 == dll->objs_count || dll->objs_count < pos_start || (end && pos_start > end)) {
    DLLDBG_LOG_OUT_NULL;
    return NULL;
  }

  DLLDBG_LOG_OUT_DEPTH_INC;

  dll_obj_t * restrict erasing = __dlli_get_obj_at_index(dll, pos_start);
  dll_obj_t * restrict save    = NULL;
  size_t i                     = pos_start ? pos_start - 1 : pos_start;

  do {
    save = erasing->next;
    dll_del(dll, erasing);
    erasing = save;
  } while (end > i++ && erasing);

  dll_obj_t * out = erasing ? erasing : dll->tail;

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(out), DLLDBG_LOG_DLL_OBJ_ARG(out));
  return out;
}

static inline void dll_foreach(const dll_t * restrict dll, dll_callback_fn_t fn) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", fn: %p", DLLDBG_LOG_DLL_ARG(dll), fn);

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == fn)) {
    DLLDBG_LOG_OUT_VOID;
    return;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  for (dll_obj_t * restrict iobj = dll->head; iobj; iobj = iobj->next) {
    fn(iobj->data);
  }

  DLLDBG_LOG_OUT_VOID;
}

static inline dll_obj_t * dll_front(const dll_t * restrict dll) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll), DLLDBG_LOG_DLL_ARG(dll));

  dll_obj_t * head = NULL;

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll)) {
    head = NULL;
  } else {
    head = dll->head;
  }
#else
  head  = dll->head;
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(head), DLLDBG_LOG_DLL_OBJ_ARG(head));
  return head;
}

static inline dll_obj_t * dll_back(const dll_t * restrict dll) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll), DLLDBG_LOG_DLL_ARG(dll));

  dll_obj_t * tail = NULL;

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll)) {
    tail = NULL;
  } else {
    tail = dll->tail;
  }
#else
  tail  = dll->tail;
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(tail), DLLDBG_LOG_DLL_OBJ_ARG(tail));
  return tail;
}

static inline bool dll_empty(const dll_t * restrict dll) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll), DLLDBG_LOG_DLL_ARG(dll));

  bool empty = true;

#ifndef LIBDLL_UNSAFE_USAGE
  empty = !(dll && !!dll->objs_count);
#else
  empty = !!dll->objs_count;
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT("%s", empty ? "true" : "false");
  return empty;
}

static inline size_t dll_size(const dll_t * restrict dll) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll), DLLDBG_LOG_DLL_ARG(dll));

  size_t count = 0;

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll)) {
    count = 0;
  } else {
    count = dll->objs_count;
  }
#else
  count = dll->objs_count;
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT("%zu", count);
  return count;
}

static inline void
    dll_merge(dll_t * restrict dst, dll_t * restrict src, dll_callback_cmp_fn_t fn_sort) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dst) ", " DLLDBG_LOG_DLL_FMT(src) ", fn_sort: %p",
             DLLDBG_LOG_DLL_ARG(dst),
             DLLDBG_LOG_DLL_ARG(src),
             fn_sort);

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dst || NULL == src)) {
    DLLDBG_LOG_OUT_VOID;
    return;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT_DEPTH_INC;

  dll_obj_t * restrict iobj = src->head;
  dll_obj_t * restrict save = NULL;

  while (iobj) {
    dll_emplace_back(dst, iobj->data, iobj->data_size, iobj->fn_free);
    save = iobj->next;
    dll_del(src, iobj);
    iobj = save;
  }

  if (NULL != fn_sort) {
    dll_sort(dst, fn_sort);
  }

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT_VOID;
}

static inline void dll_splice(dll_t * restrict dst,
                              dll_t * restrict src,
                              size_t dst_pos,
                              size_t src_start,
                              size_t src_end) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dst) ", " DLLDBG_LOG_DLL_FMT(
                 src) ", dst_pos: %zu, src_start: %zu, src_end: %zu",
             DLLDBG_LOG_DLL_ARG(dst),
             DLLDBG_LOG_DLL_ARG(src),
             dst_pos,
             src_start,
             src_end);

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dst || NULL == src)) {
    DLLDBG_LOG_OUT_VOID;
    return;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  if ((src_end && src_start > src_end) || 0 == src->objs_count) {
    DLLDBG_LOG_OUT_VOID;
    return;
  }

  DLLDBG_LOG_OUT_DEPTH_INC;

  if ((0 == dst_pos && 0 == src_start && 0 == src_end) ||
      ((dst_pos >= dst->objs_count) && (src_start >= src->objs_count) &&
       (0 == src_end))) {
    dll_merge(dst, src, NULL);

    DLLDBG_LOG_OUT_DEPTH_DEC;
    DLLDBG_LOG_OUT_VOID;
    return;
  }

  dll_obj_t * restrict dst_pos_obj = __dlli_get_obj_at_index(dst, dst_pos);
  dll_obj_t * restrict src_pos_obj = __dlli_get_obj_at_index(src, src_start);
  dll_obj_t * restrict src_pos_end_obj =
      __dlli_get_obj_at_index(src, src_end ? src_end : src->objs_count);
  size_t spliced_size = 0;

  if (0 == src_start && 0 == src_end) {
    spliced_size = src->objs_count;
  } else if (src_start && 0 == src_end) {
    spliced_size = src->objs_count - (src->objs_count - src_start);
  } else {
    spliced_size =
        (src_end > src->objs_count ? src->objs_count : (src_end + 1)) - src_start;
  }

  dst->objs_count += spliced_size;
  src->objs_count -= spliced_size;

  if (NULL == src_pos_end_obj->next) {
    src->tail = src_pos_obj->prev;
  }
  if (NULL == dst_pos_obj || (dst_pos_obj && NULL == dst_pos_obj->next)) {
    dst->tail = src_pos_end_obj;
  }
  if (0 == src_start) {
    src->head = src_pos_end_obj->next;
  }

  if (src_pos_obj->prev) {
    src_pos_obj->prev->next = src_pos_end_obj->next;
  }

  if (dst_pos && dst_pos_obj->next) {
    dst_pos_obj->next->prev = src_pos_end_obj;
  }

  if (0 == dst_pos) {
    src_pos_end_obj->next = dst->head;
    src_pos_obj->prev     = NULL;
    dst->head             = src_pos_obj;
  } else {
    if (dst_pos_obj) {
      src_pos_end_obj->next = dst_pos_obj->next;
      src_pos_obj->prev     = dst_pos_obj;
      dst_pos_obj->next     = src_pos_obj;
    } else {
      src_pos_end_obj->next = NULL;
      src_pos_obj->prev     = NULL;
      dst->head             = src_pos_obj;
      dst->tail             = src_pos_end_obj;
    }
  }

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT_VOID;
}

static inline size_t dll_remove(dll_t * restrict dll,
                                void * restrict value,
                                dll_callback_cmp_fn_t fn_cmp) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", value: %p, fn_cmp: %p",
             DLLDBG_LOG_DLL_ARG(dll),
             value,
             fn_cmp);

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == fn_cmp)) {
    DLLDBG_LOG_OUT_NULL;
    return 0;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT_DEPTH_INC;

  dll_obj_t * restrict iobj = dll->head;
  dll_obj_t * restrict save = NULL;
  size_t removed_objs       = 0;

  while (iobj) {
    save = iobj->next;
    if (0 < fn_cmp(iobj->data, value)) {
      dll_del(dll, iobj);
      ++removed_objs;
    }
    iobj = save;
  }

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT("%zu", removed_objs);
  return removed_objs;
}

static inline void __dlli_memcpy(dll_obj_t * restrict dst, dll_obj_t * restrict src) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_OBJ_FMT(dst) ", " DLLDBG_LOG_DLL_OBJ_FMT(src),
             DLLDBG_LOG_DLL_OBJ_ARG(dst),
             DLLDBG_LOG_DLL_OBJ_ARG(src));

  __u_char * restrict dst_ptr = (__u_char * restrict)dst;
  __u_char * restrict src_ptr = (__u_char * restrict)src;
  const size_t offsetnp       = offsetof(dll_obj_t, data);
  const size_t sizecopy       = sizeof(*dst) - offsetnp;

  memcpy(dst_ptr + offsetnp, src_ptr + offsetnp, sizecopy);

  DLLDBG_LOG_OUT_VOID;
}

static inline void dll_reverse(dll_t * restrict dll) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll), DLLDBG_LOG_DLL_ARG(dll));

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(!dll)) {
    DLLDBG_LOG_OUT_VOID;
    return;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT_DEPTH_INC;

  for (dll_obj_t * restrict ifirst = dll->head, * restrict ilast = dll->tail;
       ifirst && ilast && ifirst != ilast;
       ifirst = ifirst->next, ilast = ilast->prev) {
    dll_obj_t temp = *ifirst;
    __dlli_memcpy(ifirst, ilast);
    __dlli_memcpy(ilast, &temp);
  }

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT_VOID;
}

static inline size_t dll_unique(dll_t * restrict dll, dll_callback_cmp_fn_t fn_cmp) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", fn_cmp: %p", DLLDBG_LOG_DLL_ARG(dll), fn_cmp);

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == fn_cmp)) {
    DLLDBG_LOG_OUT("%zu", 0);
    return 0;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT_DEPTH_INC;

  dll_obj_t * restrict jobj = NULL;
  dll_obj_t * restrict save = NULL;
  size_t removed_objs       = 0;

  for (dll_obj_t * restrict iobj = dll->head; iobj; iobj = iobj->next) {
    jobj = iobj->next;
    while (jobj) {
      save = jobj->next;
      if (0 == fn_cmp(iobj->data, jobj->data)) {
        dll_del(dll, jobj);
        ++removed_objs;
      }
      jobj = save;
    }
  }

  DLLDBG_LOG_OUT_DEPTH_DEC;
  return removed_objs;
}

static inline dll_obj_t * __dlli_msort(dll_obj_t * restrict first,
                                       dll_obj_t * restrict second,
                                       dll_callback_cmp_fn_t fn_sort) {
  DLLDBG_LOG(
      DLLDBG_LOG_DLL_OBJ_FMT(first) ", " DLLDBG_LOG_DLL_OBJ_FMT(second) ", fn_sort: %p",
      DLLDBG_LOG_DLL_OBJ_ARG(first),
      DLLDBG_LOG_DLL_OBJ_ARG(second),
      fn_sort);

  if (NULL == first) {
    DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(second), DLLDBG_LOG_DLL_OBJ_ARG(second));
    return second;
  }
  if (NULL == second) {
    DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(first), DLLDBG_LOG_DLL_OBJ_ARG(first));
    return first;
  }

  dll_obj_t * out = NULL;

  if (0 > fn_sort(first->data, second->data)) {
    first->next       = __dlli_msort(first->next, second, fn_sort);
    first->next->prev = first;
    first->prev       = NULL;
    out               = first;
  } else {
    second->next       = __dlli_msort(first, second->next, fn_sort);
    second->next->prev = second;
    second->prev       = NULL;
    out                = second;
  }

  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(out), DLLDBG_LOG_DLL_OBJ_ARG(out));
  return out;
}

static inline dll_obj_t * __dlli_msort_parts(dll_obj_t * restrict head,
                                             dll_callback_cmp_fn_t fn_sort) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_OBJ_FMT(head) ", fn_sort: %p",
             DLLDBG_LOG_DLL_OBJ_ARG(head),
             fn_sort);

  if (NULL == head || NULL == head->next) {
    DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(head), DLLDBG_LOG_DLL_OBJ_ARG(head));
    return head;
  }

  DLLDBG_LOG_OUT_DEPTH_INC;

  dll_obj_t * restrict ifast = head;
  dll_obj_t * restrict islow = head;

  while (ifast->next && ifast->next->next) {
    ifast = ifast->next->next;
    islow = islow->next;
  }

  dll_obj_t * restrict half = islow->next;
  islow->next               = NULL;

  head = __dlli_msort_parts(head, fn_sort);
  half = __dlli_msort_parts(half, fn_sort);

  dll_obj_t * __msort_out = __dlli_msort(head, half, fn_sort);

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(__msort_out),
                 DLLDBG_LOG_DLL_OBJ_ARG(__msort_out));
  return __msort_out;
}

static inline void dll_sort(dll_t * restrict dll, dll_callback_cmp_fn_t fn_sort) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", fn_sort: %p", DLLDBG_LOG_DLL_ARG(dll), fn_sort);

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == fn_sort)) {
    DLLDBG_LOG_OUT_VOID;
    return;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  if (NULL == dll->head || NULL == dll->head->next) {
    DLLDBG_LOG_OUT_VOID;
    return;
  }

  DLLDBG_LOG_OUT_DEPTH_INC;

  dll->head = __dlli_msort_parts(dll->head, fn_sort);

  /* okay, maybe this code isn't good, but i can't find any other solution to update
   * dll->tail pointer */
  for (dll_obj_t * restrict iobj = dll->head; iobj; iobj = iobj->next) {
    dll->tail = iobj;
  }

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT_VOID;
}

static inline bool dll_is_equal(const dll_t * restrict dll_a,
                                const dll_t * restrict dll_b,
                                dll_callback_cmp_fn_t fn_cmp) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll_a) ", " DLLDBG_LOG_DLL_FMT(dll_b) ", fn_cmp: %p",
             DLLDBG_LOG_DLL_ARG(dll_a),
             DLLDBG_LOG_DLL_ARG(dll_b),
             fn_cmp);

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll_a || NULL == dll_b)) {
    DLLDBG_LOG_OUT("%s", "false");
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  if (dll_a->objs_count != dll_b->objs_count) {
    DLLDBG_LOG_OUT("%s", "false");
    return false;
  }

  dll_obj_t * restrict iobj_a = dll_a->head;
  dll_obj_t * restrict iobj_b = dll_b->head;

  for (; iobj_a && iobj_b; iobj_a = iobj_a->next, iobj_b = iobj_b->next) {
    if (fn_cmp) {
      if (0 != fn_cmp(iobj_a->data, iobj_b->data)) {
        DLLDBG_LOG_OUT("%s", "false");
        return false;
      }
    } else {
      if (iobj_a->data_size != iobj_b->data_size || iobj_a->data != iobj_b->data) {
        DLLDBG_LOG_OUT("%s", "false");
        return false;
      }
    }
  }

  bool out = !(iobj_a || iobj_b);

  DLLDBG_LOG_OUT("%s", out ? "true" : "false");
  return out;
}

static inline bool dll_is_not_equal(const dll_t * restrict dll_a,
                                    const dll_t * restrict dll_b,
                                    dll_callback_cmp_fn_t fn_cmp) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll_a) ", " DLLDBG_LOG_DLL_FMT(dll_b) ", fn_cmp: %p",
             DLLDBG_LOG_DLL_ARG(dll_a),
             DLLDBG_LOG_DLL_ARG(dll_b),
             fn_cmp);
  DLLDBG_LOG_OUT_DEPTH_INC;

  const bool is_not_equals = !dll_is_equal(dll_a, dll_b, fn_cmp);

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT("%s", is_not_equals ? "true" : "false");
  return is_not_equals;
}

static inline void dll_del(dll_t * restrict dll, dll_obj_t * restrict obj) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", " DLLDBG_LOG_DLL_OBJ_FMT(obj),
             DLLDBG_LOG_DLL_ARG(dll),
             DLLDBG_LOG_DLL_OBJ_ARG(obj));
  DLLDBG_LOG_OUT_DEPTH_INC;

  dll_obj_t * restrict del_obj = dll_unlink(dll, obj);

  dll_free_obj(&del_obj);

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT_VOID;
}

static inline dll_obj_t * dll_unlink(dll_t * restrict dll, dll_obj_t * restrict obj) {
  DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(dll) ", " DLLDBG_LOG_DLL_OBJ_FMT(obj),
             DLLDBG_LOG_DLL_ARG(dll),
             DLLDBG_LOG_DLL_OBJ_ARG(obj));

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == obj)) {
    DLLDBG_LOG_OUT_NULL;
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  --dll->objs_count;
  if (obj->prev) {
    obj->prev->next = obj->next;
  } else {
    dll->head = obj->next;
  }
  if (obj->next) {
    obj->next->prev = obj->prev;
  } else {
    dll->tail = obj->prev;
  }

  DLLDBG_LOG_OUT(DLLDBG_LOG_DLL_OBJ_FMT(obj), DLLDBG_LOG_DLL_OBJ_ARG(obj));
  return obj;
}

static inline void dll_free_obj(dll_obj_t * restrict * restrict obj) {
  {
    const dll_obj_t * restrict __obj = *obj;
    DLLDBG_LOG(DLLDBG_LOG_DLL_OBJ_FMT(__obj), DLLDBG_LOG_DLL_OBJ_ARG(__obj));
  }

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == obj || NULL == obj || NULL == *obj)) {
    DLLDBG_LOG_OUT_VOID;
    return;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  if ((*obj)->fn_free) {
    if (LIBDLL_FN_FREE_CLEARANCE != (*obj)->fn_free) {
      (*obj)->fn_free((*obj)->data);
    }
    if (LIBDLL_FN_FREE_NULL != (*obj)->data) {
      free((*obj)->data);
    }
  }

  (*obj)->data = NULL;
  free(*obj);
  *obj = NULL;

  DLLDBG_LOG_OUT_VOID;
}

static inline void dll_free(dll_t * restrict * restrict dll) {
  {
    const dll_t * restrict __dll = *dll;
    DLLDBG_LOG(DLLDBG_LOG_DLL_FMT(__dll), DLLDBG_LOG_DLL_ARG(__dll));
  }

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == *dll)) {
    DLLDBG_LOG_OUT_VOID;
    return;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  DLLDBG_LOG_OUT_DEPTH_INC;

  dll_obj_t * restrict iobj = (*dll)->head;
  dll_obj_t * restrict save = NULL;

  while (iobj) {
    save = iobj->next;
    dll_free_obj(&iobj);
    iobj = save;
  }

  free(*dll);
  *dll = NULL;

  DLLDBG_LOG_OUT_DEPTH_DEC;
  DLLDBG_LOG_OUT_VOID;
}

#endif /* LIBDLL_H */
