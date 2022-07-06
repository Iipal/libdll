/**
 * \file libdll.h
 *
 * \brief Single-header realization of Double Linked List API based on C++11 std::list on
 * C
 *
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

//
// ----------------------------
// libdll specifications and macroses
// ----------------------------
//

#ifdef LIBDLL_UNSAFE_USAGE
#  undef LIBDLL_UNSAFE_USAGE

/**
 * Removing all nullability checks in all functions.
 *
 * \warning May cause better performance but undefined behavior as well.
 */
#  define LIBDLL_UNSAFE_USAGE 1

#endif /* LIBDLL_UNSAFE_USAGE */

/**
 * Use this macros as \c destructor argument for #dll_new_obj if you do not
 * allocate anything inside the \c data , but the \c data itself was allocated before you
 * put in #dll_new_obj .
 */
#define LIBDLL_DESTRUCTOR_DEFAULT ((dll_callback_destructor_fn_t)-0x1UL)

/**
 * Use this macros as \c destructor argument for #dll_new_obj if you don't need
 * to free any memory in list-objects \c data or even pointer to \c data itself.
 */
#define LIBDLL_DESTRUCTOR_NULL ((dll_callback_destructor_fn_t)NULL)

//
// ----------------------------
// Macros definitions
// ----------------------------
//

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

#define __dll_inline static inline

//
// ----------------------------
// Data structure definitions
// ----------------------------
//

/**
 * A callback typedef for comparing \c data from 2 list-objects, or list-object
 * with any other given data by user, or example: #dll_remove , #dll_sort or #dll_map .
 *
 * \param obj_data list-object data.
 * \param other other list-object or user data.
 * \param index an index of current obj_data.
 *
 * \return comparing result value.
 */
typedef ssize_t (*dll_callback_fn_t)(void * restrict obj_data,
                                     void * restrict other,
                                     size_t index);

/**
 * An extended version of #dll_callback_fn_t
 *
 * \param obj_a list-object data.
 * \param obj_b other list-object or user data.
 * \param any an any data pointer.
 * \param index an index of current obj_data.
 *
 * \return comparing result value.
 */
typedef ssize_t (*dll_callback_ext_fn_t)(void * restrict obj_a,
                                         void * restrict obj_b,
                                         void * restrict any,
                                         size_t index);

/**
 * A destructor callback typedef for list-object desctructor function.
 *
 * \param obj_data list-object data.
 */
typedef void (*dll_callback_destructor_fn_t)(void * restrict obj_data);

/**
 * A callback function for #dll_map.
 *
 * \param obj_data list-object data.
 * \param any an any additional data.
 * \param index index of current list-object passed to this callback, starts from 1.
 */
typedef void * (*dll_callback_mapper_fn_t)(void * restrict obj_data,
                                           void * restrict any,
                                           size_t index);

/**
 * A list-object structure.
 *
 * \typedef dll_obj_t
 */
typedef struct __s_dll_obj {
  /** a pointer to next list-object. */
  struct __s_dll_obj * restrict next;
  /** a pointer to previous list-object. */
  struct __s_dll_obj * restrict prev;
  /** an any user defined data. */
  void * restrict data;

  /**
   * \destructor_description
   */
  dll_callback_destructor_fn_t destructor;

  /** a \c data size. */
  size_t size;
} dll_obj_t;

/**
 * A doubly linked list structure.
 *
 * \typedef dll_t
 */
typedef struct {
  /** a head of list. */
  dll_obj_t * restrict head;
  /** a tail of list. */
  dll_obj_t * restrict tail;
  /** a counter of list-objects in list. */
  size_t objs_count;
} dll_t;

/**
 * A list-object iterator structure.
 *
 * \typedef dll_iterator_t
 */
typedef struct {
  /** a current iterator list-object */
  dll_obj_t * restrict __obj;
  /** an original list, for which iterator was created */
  dll_t * restrict __dll;
  /** current list-object index. */
  size_t __index;
} dll_iterator_t;

//
// ----------------------------
// Function prototypes
// ----------------------------
//

/**
 * \b Creates a new and empty list with this function.
 *
 * \return allocated memory for new list, \c NULL otherwise
 */
__dll_inline dll_t * dll_new(void);

/**
 * \b Creates a new from given parameters.
 *
 * \attention Strongly recommended to set-up \p destructor if you have allocated memory
 * inside \p data .
 *
 * \param data any data you want to put inside of list.
 * \param size a size of \p data .
 * \param destructor \destructor_description
 *
 * \return a new list-object, \c NULL otherwise
 */
__dll_inline dll_obj_t * dll_new_obj(void * restrict data,
                                     size_t                       size,
                                     dll_callback_destructor_fn_t destructor);

/**
 * \b Pushes a provided list-object to front of given \p dll list.
 *
 * \param dll destination list.
 * \param obj list-object.
 *
 * \return \p obj on success, \c NULL otherwise
 */
__dll_inline dll_obj_t * dll_push_front(dll_t * restrict dll, dll_obj_t * restrict obj);

/**
 * \b Pushes a provided obj list-object to end of given \p dll list.
 *
 * \param dll destination list.
 * \param obj list-object.
 *
 * \return \p obj on success, \c NULL otherwise
 */
__dll_inline dll_obj_t * dll_push_back(dll_t * restrict dll, dll_obj_t * restrict obj);

/**
 * \b Creates a new list-object via #dll_new_obj with given parameters and
 * pushing it in front of \p dll list.
 *
 * \param dll destination list.
 * \param data any data.
 * \param size size of \p data.
 * \param destructor \destructor_description
 *
 * \return a new list-object, \c NULL otherwise
 */
__dll_inline dll_obj_t * dll_emplace_front(dll_t * restrict dll,
                                           void * restrict data,
                                           size_t                       size,
                                           dll_callback_destructor_fn_t destructor);

/**
 * \b Creates a new list-object via #dll_new_obj with given parameters and
 * pushing it at the end of \p dll list.
 *
 * \param dll destination list.
 * \param data any data.
 * \param size size of \p data.
 * \param destructor \destructor_description
 *
 * \return a new created list-object.
 */
__dll_inline dll_obj_t * dll_emplace_back(dll_t * restrict dll,
                                          void * restrict data,
                                          size_t                       size,
                                          dll_callback_destructor_fn_t destructor);

/**
 * \b Unlinks the first(head) list-object from the list.
 *
 * \param dll list.
 *
 * \return unlinked list-object
 */
__dll_inline dll_obj_t * dll_pop_front(dll_t * restrict dll);

/**
 * \b Unlinks the last(tail) list-object from the list.
 *
 * \param dll list.
 *
 * \return unlinked list-object
 */
__dll_inline dll_obj_t * dll_pop_back(dll_t * restrict dll);

/**
 * \b Erases all elements from the \p dll list.
 *
 * \attention After this call, #dll_size returns zero. Any pointers referring to contained
 * elements can be invalid if you specified a \c destructor callback-function for each
 * individual list-object, alsot lost memory leaks errors can occur as well if you don't
 * specified a \c destructor function for each individual list-object but memory inside of
 * this list-object was allocated.
 *
 * \param dll list.
 *
 * \return \c true on success, \c false otherwise
 */
__dll_inline bool dll_clear(dll_t * restrict dll);

/**
 * \b Inserts a provided list-object \p obj on the specified location \p pos in
 * the list \p dll .
 *
 * \param dll list to where new object will be injected.
 * \param obj list-object to be inserted.
 * \param pos injection position where list-object will be inserted
 *
 * \note \p pos must starts from 0 to #dll_size .
 *
 * \return inserted list-object or NULL if something is wrong.
 */
__dll_inline dll_obj_t *
    dll_insert(dll_t * restrict dll, dll_obj_t * restrict obj, size_t pos);

/**
 * \b Inserts a new list-object created  via #dll_new_obj on the specified
 * location \p pos in the list \p dll.
 *
 * \param dll list.
 * \param data any data.
 * \param size size of \p data.
 * \param destructor \destructor_description
 *
 * \return inserted list-object or NULL if something is wrong.
 */
__dll_inline dll_obj_t * dll_emplace(dll_t * restrict dll,
                                     void * restrict data,
                                     size_t                       size,
                                     dll_callback_destructor_fn_t destructor,
                                     size_t                       pos);

/**
 * \b Erases the specified range of list-objects from the list \p dll .
 *
 * \param dll list .
 * \param start start of the range.
 * \param end end of the range.
 *
 * \note If \p end is zero or bigger than \p start then only 1
 * list-object at \c start position will be erased.
 *
 * \note \p start and \p end must starts from 1.
 *
 * \attention \c NULL returns in cases if: \p dll list has no objects or \p dll is
 * \c NULL ; \c start bigger than #dll_size;
 *
 * \return a last list-object after deleting range or NULL.
 */
__dll_inline dll_obj_t * dll_erase(dll_t * restrict dll, size_t start, size_t end);

/**
 * \b Going throught all the list-objects in \p dll list and calls a
 * provided \p fn callback-function for each list-object.
 *
 * \note Function works only if \p dll and \p fn is not a \c NULL .
 *
 * \param dll list.
 * \param fn callback with only 1 argument - `void*` to any data inside list-object.
 * \param any any data to be passed to the \p fn second argument.
 *
 * \return \c true on success, \c false otherwise.
 */
__dll_inline bool
    dll_foreach(const dll_t * restrict dll, dll_callback_fn_t fn, void * restrict any);

/**
 * \b Creates a new #dll_iterator_t iterator for provided \p dll list.
 *
 * \param dll list.
 *
 * \return a new iterator
 */
__dll_inline dll_iterator_t dll_iterator(dll_t * restrict const dll);

/**
 * \b Access the data inside provided #dll_iterator_t \p it .
 *
 * \note returns \c NULL if iterator at the end or at the start of list.
 *
 * \param it an iterator.
 *
 * \return list-object
 */
__dll_inline dll_obj_t * dll_iterator_get_obj(const dll_iterator_t * restrict const it);

/**
 * \b Access any data from list-object inside provided iterator #dll_iterator_t \p it .
 *
 * \note returns \c NULL if iterator at the end or at the start of list.
 *
 * \param it an iterator.
 *
 * \return any data
 */
__dll_inline void * dll_iterator_get_data(const dll_iterator_t * restrict const it);

/**
 * \b Access current index of iterator #dll_iterator \p it .
 *
 * \note Returns \c ~0UL if iterator is \c NULL .
 *
 * \param it an iterator.
 *
 * \return unsigned integer represents current iterator index
 */
__dll_inline size_t dll_iterator_get_index(const dll_iterator_t * restrict const it);

/**
 * \b Replaces \c data inside the iterator #dll_iterator_t \p it .
 *
 * \note \b Replacing data inside the iterator means replacing it at the original list for
 * which iterator was created as well.
 *
 * \param it an iterator.
 * \param destructor \destructor_description
 *
 * \return any previous data
 */
__dll_inline void * dll_iterator_set_data(const dll_iterator_t * restrict const it,
                                          void * restrict data,
                                          dll_callback_destructor_fn_t destructor);

/**
 * \b Moves the #dll_iterator_t to next list-object.
 *
 * \return true while not meets the end of list.
 */
__dll_inline bool dll_next(dll_iterator_t * restrict const it);

/**
 * \b Moves the #dll_iterator_t to previous list-object.
 *
 * \return true while not meets a start of list.
 */
__dll_inline bool dll_prev(dll_iterator_t * restrict const it);

/**
 * **Access the first**(head) list-object in \p dll list as an
 * #dll_iterator_t iterator.
 *
 * \note \b Equlas to #dll_iterator function.
 *
 * \param dll list.
 *
 * \return iterator at the begining of list.
 */
__dll_inline dll_iterator_t dll_front(dll_t * restrict dll);

/**
 * **Access the last**(tail) list-object in \p dll list as an
 * #dll_iterator_t iterator.
 *
 * \param dll list.
 *
 * \return iterator at the end of list.
 */
__dll_inline dll_iterator_t dll_back(dll_t * restrict dll);

/**
 * \b Checks whether the list \p dll is empty or not.
 *
 * \param dll list.
 *
 * \return \c true if the list is empty, \c false otherwise.
 */
__dll_inline bool dll_empty(const dll_t * restrict dll);

/**
 * \b Get the count of elements in the provided list \p dll.
 *
 * \param dll list.
 *
 * \return count of elements in the list.
 */
__dll_inline size_t dll_size(const dll_t * restrict dll);

/**
 * \b Get the data inside provided list-object \p obj.
 *
 * \param obj a list-object.
 *
 * \return any data.
 */
__dll_inline void * dll_obj_get_data(const dll_obj_t * restrict const obj);

/**
 * \b Merges two lists into a \p dst list.
 *
 * \note After merging \p dst list will be sorted if you specifed a \p fn_sort function.
 *
 * \attentnion The \p src list will be clear after merging it to the \p dst list.
 *
 * \param dst destination list.
 * \param src source list.
 * \param fn_sort after merging sorter callback.
 *
 * \return \c true if lists succesfly merged and src cleared, as well as sorted(optional),
 * otherwise \c false.
 */
__dll_inline bool dll_merge(dll_t * restrict dst,
                            dll_t * restrict src,
                            dll_callback_ext_fn_t fn_sort,
                            void *                fn_sort_any);

/**
 * \b Moves list-objects from \p src to \p dst.
 *
 * \param dst destination list.
 * \param src second list. Full or part of this list will be clear after applying this
 * function.
 * \param dst_pos to what position in list \p dst - list \p src will be transfered.
 * \param src_start start position of list \p src from which list-objects will be
 * transefered to \p dst .
 * \param src_end end position of transfering from \p src list.
 *
 * \exception If \c dst_pos , \c src_start , and \c src_end are zero, OR if \p dst_pos and
 * \p src_start at the end of lists \p dst and \p src when \p src_end is 0 - then
 * \p src will be merged at the end of \p dst via #dll_merge .
 *
 * \attention \p dst_pos , \p src_start ,and \p src_end must starts from 1
 *
 * \warning \c false returns in case if: \p src_end less than or equal to \p src_start ;
 * \p src list is empty; \p dst or \p src is \c NULL .
 *
 * \return \c true on success, \c false otherwise
 */
__dll_inline bool dll_splice(dll_t * restrict const dst,
                             dll_t * restrict const src,
                             size_t dst_pos,
                             size_t src_start,
                             size_t src_end);

/**
 * \b Removes all list-objects satisfying specific criterias via \p fn_cmp
 * from \p dll. Applies for all list-objects for which \p fn_cmp return a zero value.
 *
 * \param dll list.
 * \param fn_cmp comparator for list-objects data and given \p any data.
 * \param any any additional data to be given to the second argument of \p fn_cmp .
 *
 * \return count of removed objects from list \p dll .
 */
__dll_inline size_t dll_remove(dll_t * restrict dll,
                               dll_callback_fn_t fn_cmp,
                               void * restrict any);

/**
 * \b Reverses the order of the list-objects in the list.
 *
 * \param dll list to be reversed.
 *
 * \return true on succes, false otherwise
 */
__dll_inline bool dll_reverse(const dll_t * restrict const dll);

/**
 * \b Searches for specific \p data via \p fn_search when it returns zero value.
 *
 * \param dll list.
 * \param fn_search function which will deside is given \c data is valid
 * \param any an any additional data that will be passed to the second argument of
 * \p fn_search
 *
 * \return first occurrence of searched data in list \p dll , \c NULL otherwise.
 */
__dll_inline void *
    dll_find(dll_t * restrict dll, dll_callback_fn_t fn_search, void * restrict any);

/**
 * \b Creates a new array populated with the results of calling a provided function
 * \p mapper on every list-object for the calling list \p dll .
 *
 * \param dll mapping list
 * \param mapper callback function to produce results for each list-object
 * \param any an any additional data to be passed to the \p mapper callback function
 *
 * \return A new array of pointers with each element being the result of the callback
 * function.
 */
__dll_inline void ** dll_map(const dll_t * restrict const dll,
                             dll_callback_mapper_fn_t mapper,
                             void * restrict any);

/**
 * \b Removes all consecutive duplicate list-objects from the list.
 *
 * \note Only the first list-object in each group of equal list-objects is left.
 * List-objects compares via \p fn_cmp and only if \p fn_cmp returns zero means the
 * list-objects are equals.
 *
 * \param dll list to be checked for duplicates.
 * \param fn_cmp comparator for list-objects.
 *
 * \return count of removed objects from a list.
 */
__dll_inline size_t dll_unique(dll_t * restrict dll,
                               dll_callback_ext_fn_t fn_cmp,
                               void *                any);

/**
 * \b Sorts all the list-objects via \p fn_sort in \p dll list using a recursive merge
 * sort.
 *
 * \note if list empty or has only 1 list-object then it's consider as sorted and returns
 * \c true .
 *
 * \param dll list to be sorted.
 * \param fn_sort callback-function to compare list-objects.
 * \param any any data to be passed to fn_sort callback.
 *
 * \returns \c true on success, \c false otherwise.
 */
__dll_inline bool
    dll_sort(dll_t * restrict dll, dll_callback_ext_fn_t fn_sort, void * any);

/**
 * \b Compares two lists if they are not equals.
 *
 * \note if \p fn_cmp is not provided then list-objects will be compared by it's \c size
 * and \c data .
 *
 * \param dll_a first list to compare
 * \param dll_b second list to compare
 * \param fn_cmp list-objects comparator
 *
 * \return true if lists are equals
 */
__dll_inline bool dll_is_equal(const dll_t * restrict const dll_a,
                               const dll_t * restrict const dll_b,
                               dll_callback_ext_fn_t fn_cmp,
                               void *                any);

/**
 * **Unlink and free** a list-object \p obj from list \p dll.
 *
 * \param dll list.
 * \param obj list-object to be deleted.
 *
 * \return \c true on success, \c false otherwise
 */
__dll_inline bool dll_delete(dll_t * restrict dll, dll_obj_t * restrict obj);

/**
 * \b Unlink (only removes list-object from list but not deleting it) a #dll_obj_t
 * list-object \p obj from list \p dll .
 *
 * \param dll list.
 * \param obj list-object to be unlinked.
 *
 * \return unlinked \o obj on success, \c NULL otherwise.
 */
__dll_inline dll_obj_t * dll_unlink(dll_t * restrict dll, dll_obj_t * restrict obj);

/**
 * \b Free a list-object \p obj and data in it.
 *
 * \attention BE CERAFUL - It's doesn't works with links to next and previous
 * list-objects, so calling this function without #dll_unlink may cause \c SIGSEGV in
 * future work with list. Better soultion may be for you to use a #dll_delete instead.
 *
 * \param obj a list-object.
 *
 * \return true on success, false otherwise
 */
__dll_inline bool dll_free_obj(dll_obj_t * restrict * restrict obj);

/**
 * \b Free the whole list with all list-objects linked to this list and its data.
 *
 * \param dll list.
 *
 * \return true on success, false otherwise
 */
__dll_inline bool dll_free(dll_t * restrict * restrict dll);

/*
 * ----------------------------
 * Function definitions
 * ----------------------------
 */

__dll_inline dll_t * dll_new(void) {
  dll_t * restrict out = calloc(1, sizeof(*out));

  return out;
}

__dll_inline dll_obj_t * dll_new_obj(void * restrict data,
                                     size_t                       size,
                                     dll_callback_destructor_fn_t destructor) {
  dll_obj_t * restrict out = NULL;

#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == (out = calloc(1, sizeof(*out))))) {
    return NULL;
  }
#else
  out = calloc(1, sizeof(*out));
#endif /* LIBDLL_UNSAFE_USAGE */

  out->data       = data;
  out->size       = size;
  out->destructor = destructor;
  return out;
}

__dll_inline dll_obj_t * dll_push_front(dll_t * restrict dll, dll_obj_t * restrict obj) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == obj)) {
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
  return obj;
}

/**
 * \b Pushes a provided \p obj list-object to the end of given \p dll list.
 *
 * \param dll destination list.
 * \param obj list-object.
 *
 * \return \p obj on success, \c NULL otherwise
 */
__dll_inline dll_obj_t * dll_push_back(dll_t * restrict dll, dll_obj_t * restrict obj) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == obj)) {
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
  return obj;
}

__dll_inline dll_obj_t * dll_emplace_front(dll_t * restrict dll,
                                           void * restrict data,
                                           size_t                       size,
                                           dll_callback_destructor_fn_t destructor) {
  dll_obj_t * restrict new_obj = dll_new_obj(data, size, destructor);
  dll_obj_t * restrict __ret   = dll_push_front(dll, new_obj);

  return __ret;
}

__dll_inline dll_obj_t * dll_emplace_back(dll_t * restrict dll,
                                          void * restrict data,
                                          size_t                       size,
                                          dll_callback_destructor_fn_t destructor) {
  dll_obj_t * restrict new_obj = dll_new_obj(data, size, destructor);
  dll_obj_t * restrict __ret   = dll_push_back(dll, new_obj);

  return __ret;
}

__dll_inline dll_obj_t * dll_pop_front(dll_t * restrict dll) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == dll->head)) {
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  dll_obj_t * restrict head = dll->head;

  dll->head = head->next;

  dll_obj_t * restrict __ret = dll_unlink(dll, head);

  return __ret;
}

__dll_inline dll_obj_t * dll_pop_back(dll_t * restrict dll) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == dll->tail)) {
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  dll_obj_t * restrict tail = dll->tail;

  dll->tail = tail->prev;

  dll_obj_t * restrict __ret = dll_unlink(dll, tail);

  return __ret;
}

__dll_inline bool dll_clear(dll_t * restrict dll) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll)) {
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  while (dll->objs_count) {
    dll_obj_t * restrict obj = dll_pop_front(dll);

#ifndef LIBDLL_UNSAFE_USAGE
    if (obj) {
      const register bool is_obj_freed =
#endif /* LIBDLL_UNSAFE_USAGE */
          dll_free_obj(&obj);

#ifndef LIBDLL_UNSAFE_USAGE
      if (!is_obj_freed) {
        return false;
      }
    } else {
      return false;
    }
#endif /* LIBDLL_UNSAFE_USAGE */
  }

  return true;
}

__dll_inline dll_obj_t *
    dll_insert(dll_t * restrict dll, dll_obj_t * restrict obj, size_t pos) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == obj)) {
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  if (dll->objs_count < pos) {
    return NULL;
  }

  if (NULL == dll->head || 0 == pos) {
    dll_obj_t * restrict __ret =
        dll_emplace_front(dll, obj->data, obj->size, obj->destructor);

    return __ret;
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

    return obj;
  }
}

__dll_inline dll_obj_t * dll_emplace(dll_t * restrict dll,
                                     void * restrict data,
                                     size_t                       size,
                                     dll_callback_destructor_fn_t destructor,
                                     size_t                       pos) {
  dll_obj_t * restrict new_obj = dll_new_obj(data, size, destructor);
  dll_obj_t * restrict __ret   = dll_insert(dll, new_obj, pos);

  return __ret;
}

__dll_inline dll_obj_t * __dlli_get_obj_at_index(dll_t * restrict dll, size_t pos) {
  const size_t dll_size    = dll->objs_count;
  dll_obj_t * restrict obj = NULL;

  if ((dll_size / 2) >= pos) {
    obj = dll->head;
    for (size_t i = 0; obj && pos > i; ++i) {
      obj = obj->next;
    }
  } else {
    obj = dll->tail;
    for (size_t i = dll_size ? dll_size - 1 : dll_size; obj && pos < i; --i) {
      obj = obj->prev;
    }
  }

  return obj;
}

__dll_inline dll_obj_t * dll_erase(dll_t * restrict dll, size_t start, size_t end) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll)) {
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  if (0 == dll->objs_count || dll->objs_count < start) {
    return NULL;
  }

  dll_obj_t * restrict erasing = __dlli_get_obj_at_index(dll, start);
  dll_obj_t * restrict save    = NULL;
  size_t i                     = start ? start - 1 : start;

  do {
    save = erasing->next;

#ifndef LIBDLL_UNSAFE_USAGE
    if (false == dll_delete(dll, erasing)) {
      return NULL;
    }
#else
    dll_delete(dll, erasing);
#endif

    erasing = save;
  } while (end > i++ && erasing);

  return erasing ? erasing : dll->tail;
}

__dll_inline bool
    dll_foreach(const dll_t * restrict dll, dll_callback_fn_t fn, void * restrict any) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == fn)) {
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  size_t i = 0;

  for (dll_obj_t * restrict iobj = dll->head; iobj; iobj = iobj->next) {
    fn(iobj->data, any, i++);
  }

  return true;
}

__dll_inline dll_iterator_t dll_iterator(dll_t * restrict const dll) {
  dll_iterator_t it = {
#ifndef LIBDLL_UNSAFE_USAGE
      dll ? dll->head : NULL,
#else
      dll->head,
#endif
      dll,
      0};

  return it;
}

__dll_inline bool dll_next(dll_iterator_t * restrict const it) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == it || (it && NULL == it->__dll))) {
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  it->__obj = it->__obj ? it->__obj->next : NULL;
  ++it->__index;

  return !!it->__obj;
}

__dll_inline bool dll_prev(dll_iterator_t * restrict const it) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == it || (it && NULL == it->__dll))) {
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  it->__obj = it->__obj ? it->__obj->prev : NULL;
  --it->__index;

  return !!it->__obj;
}

__dll_inline dll_obj_t * dll_iterator_get_obj(const dll_iterator_t * restrict const it) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == it)) {
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  return it->__obj;
}

__dll_inline void * dll_iterator_get_data(const dll_iterator_t * restrict const it) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == it)) {
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  return it->__obj->data;
}

__dll_inline void * dll_iterator_set_data(const dll_iterator_t * restrict const it,
                                          void * restrict data,
                                          dll_callback_destructor_fn_t destructor) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == it)) {
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  void * restrict old_data = it->__obj->data;

  it->__obj->data       = data;
  it->__obj->destructor = destructor;

  return old_data;
}

__dll_inline size_t dll_iterator_get_index(const dll_iterator_t * restrict const it) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == it)) {
    return ~0UL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  return it->__index;
}

__dll_inline dll_iterator_t dll_front(dll_t * restrict dll) {
  dll_iterator_t it = dll_iterator(dll);

  return it;
}

__dll_inline dll_iterator_t dll_back(dll_t * restrict dll) {
  dll_iterator_t it = {
#ifndef LIBDLL_UNSAFE_USAGE
      dll ? dll->tail : NULL,
      dll,
      0,
#else
      dll->tail,
      dll,
      dll->objs_count ? dll->objs_count - 1 : 0,
#endif
  };

  return it;
}

__dll_inline bool dll_empty(const dll_t * restrict dll) {
#ifndef LIBDLL_UNSAFE_USAGE
  return !(dll && !!dll->objs_count);
#else
  return !!dll->objs_count;
#endif /* LIBDLL_UNSAFE_USAGE */
}

__dll_inline size_t dll_size(const dll_t * restrict dll) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll)) {
    return 0;
  } else {
    return dll->objs_count;
  }
#else
  return dll->objs_count;
#endif /* LIBDLL_UNSAFE_USAGE */
}

__dll_inline void * dll_obj_get_data(const dll_obj_t * restrict const obj) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == obj)) {
    return NULL;
  } else {
    return obj->data;
  }
#else
  return obj->data;
#endif /* LIBDLL_UNSAFE_USAGE */
}

__dll_inline bool dll_merge(dll_t * restrict dst,
                            dll_t * restrict src,
                            dll_callback_ext_fn_t fn_sort,
                            void *                fn_sort_any) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dst || NULL == src)) {
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  dll_obj_t * restrict iobj = src->head;
  dll_obj_t * restrict save = NULL;

  while (iobj) {
    save = iobj->next;

#ifndef LIBDLL_UNSAFE_USAGE
    const dll_obj_t * const restrict __new_obj =
        dll_emplace_back(dst, iobj->data, iobj->size, iobj->destructor);
    if (__new_obj) {
      const bool is_deleted_src = dll_delete(src, iobj);
      if (false == is_deleted_src) {
        return false;
      }
    } else {
      return false;
    }
#else
    dll_emplace_back(dst, iobj->data, iobj->size, iobj->destructor);
    dll_delete(src, iobj);
#endif /* LIBDLL_UNSAFE_USAGE */

    iobj = save;
  }

  bool __ret = true;
  if (NULL != fn_sort) {
    __ret = dll_sort(dst, fn_sort, fn_sort_any);
  }

  return __ret;
}

__dll_inline bool dll_splice(dll_t * restrict const dst,
                             dll_t * restrict const src,
                             size_t dst_pos,
                             size_t src_start,
                             size_t src_end) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dst || NULL == src)) {
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  if ((src_end && src_start > src_end) || 0 == src->objs_count) {
    return false;
  }

  if ((0 == dst_pos && 0 == src_start && 0 == src_end) ||
      ((dst_pos >= dst->objs_count) && (src_start >= src->objs_count) &&
       (0 == src_end))) {
    const bool __ret = dll_merge(dst, src, NULL, NULL);

    return __ret;
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

  return true;
}

__dll_inline size_t dll_remove(dll_t * restrict dll,
                               dll_callback_fn_t fn_cmp,
                               void * restrict any) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == fn_cmp)) {
    return 0;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  size_t removed_objs = 0;
  size_t i            = 0;

  for (dll_obj_t * restrict iobj = dll->head; iobj;) {
    dll_obj_t * restrict save = iobj->next;

    const size_t fn_cmp_ret = fn_cmp(iobj->data, any, i++);
    if (0 == fn_cmp_ret) {
#ifndef LIBDLL_UNSAFE_USAGE
      if (false == dll_delete(dll, iobj)) {
        return 0;
      }
#else
      dll_delete(dll, iobj);
#endif
      ++removed_objs;
    }

    iobj = save;
  }

  return removed_objs;
}

/**
 * \b Copying all the data of list-object \p src to \p dst.
 *
 * \attention This variation of memcpy(3) copies only internal data without
 * previous\next list pointers.
 *
 * \param dst destination
 * \param src source
 */
__dll_inline void __dlli_memcpy(dll_obj_t * restrict dst, dll_obj_t * restrict src) {
  unsigned char * restrict dstptr = (unsigned char * restrict)dst;
  unsigned char * restrict srcptr = (unsigned char * restrict)src;
  const size_t offsetnp           = offsetof(dll_obj_t, data);
  const size_t copysize           = sizeof(*dst) - offsetnp;

  memcpy(dstptr + offsetnp, srcptr + offsetnp, copysize);
}

__dll_inline bool dll_reverse(const dll_t * restrict const dll) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(!dll)) {
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  for (dll_obj_t * restrict ifirst = dll->head, * restrict ilast = dll->tail;
       ifirst && ilast && ifirst != ilast;
       ifirst = ifirst->next, ilast = ilast->prev) {
    dll_obj_t temp = *ifirst;
    __dlli_memcpy(ifirst, ilast);
    __dlli_memcpy(ilast, &temp);
  }

  return true;
}

__dll_inline void *
    dll_find(dll_t * restrict dll, dll_callback_fn_t fn_search, void * restrict any) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == fn_search)) {
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  void * restrict out = NULL;
  size_t i            = 0;

  for (dll_obj_t * restrict iobj = dll->head; iobj; iobj = iobj->next) {
    const ssize_t fn_search_ret = fn_search(iobj->data, any, i++);
    if (0 == fn_search_ret) {
      out = iobj->data;
      break;
    }
  }

  return out;
}

__dll_inline void ** dll_map(const dll_t * restrict const dll,
                             dll_callback_mapper_fn_t mapper,
                             void * restrict any) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == mapper || 0 == dll->objs_count)) {
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  void ** mapped_array = calloc(dll->objs_count, sizeof(*mapped_array));

#ifndef LIBDLL_UNSAFE_USAGE
  if (!mapped_array) {
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  size_t i = 0;
  for (dll_obj_t * restrict iobj = dll->head; iobj && dll->objs_count > i;
       iobj                      = iobj->next, ++i) {
    void * restrict new_data = mapper(iobj->data, any, i + 1);
    mapped_array[i]          = new_data;
  }

  return mapped_array;
}

__dll_inline size_t dll_unique(dll_t * restrict dll,
                               dll_callback_ext_fn_t fn_cmp,
                               void *                any) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == fn_cmp)) {
    return 0;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  dll_obj_t * restrict jobj = NULL;
  dll_obj_t * restrict save = NULL;
  size_t removed_objs       = 0;
  size_t i                  = 0;

  for (dll_obj_t * restrict iobj = dll->head; iobj; iobj = iobj->next) {
    jobj = iobj->next;

    while (jobj) {
      save = jobj->next;

      if (0 == fn_cmp(iobj->data, jobj->data, any, i)) {
#ifndef LIBDLL_UNSAFE_USAGE
        if (false == dll_delete(dll, jobj)) {
          return false;
        }
#else
        dll_delete(dll, jobj);
#endif /* LIBDLL_UNSAFE_USAGE */

        ++removed_objs;
      }

      jobj = save;
    }

    ++i;
  }

  return removed_objs;
}

__dll_inline dll_obj_t * __dlli_msort(dll_obj_t * restrict first,
                                      dll_obj_t * restrict second,
                                      dll_callback_ext_fn_t fn_sort,
                                      void *                any) {
  if (NULL == first) {
    return second;
  }
  if (NULL == second) {
    return first;
  }

  const ssize_t fn_sort_ret = fn_sort(first->data, second->data, any, ~0UL);
  if (0 > fn_sort_ret) {
    first->next       = __dlli_msort(first->next, second, fn_sort, any);
    first->next->prev = first;
    first->prev       = NULL;
    return first;
  } else {
    second->next       = __dlli_msort(first, second->next, fn_sort, any);
    second->next->prev = second;
    second->prev       = NULL;
    return second;
  }
}

__dll_inline dll_obj_t * __dlli_msort_parts(dll_obj_t * restrict head,
                                            dll_callback_ext_fn_t fn_sort,
                                            void *                any) {
  if (NULL == head || NULL == head->next) {
    return head;
  }

  dll_obj_t * restrict ifast = head;
  dll_obj_t * restrict islow = head;

  while (ifast->next && ifast->next->next) {
    ifast = ifast->next->next;
    islow = islow->next;
  }

  dll_obj_t * restrict half = islow->next;
  islow->next               = NULL;

  head = __dlli_msort_parts(head, fn_sort, any);
  half = __dlli_msort_parts(half, fn_sort, any);

  return __dlli_msort(head, half, fn_sort, any);
}

__dll_inline bool
    dll_sort(dll_t * restrict dll, dll_callback_ext_fn_t fn_sort, void * any) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == fn_sort)) {
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  if (1 >= dll->objs_count) {
    return true; // list already "sorted"
  }

  dll->head = __dlli_msort_parts(dll->head, fn_sort, any);

  // okay, maybe this code isn't good, but i can't find any other solution to update
  // dll->tail pointer
  dll_obj_t * restrict iobj = dll->head;
  while (iobj) {
    if (!iobj->next) {
      break;
    }
    iobj = iobj->next;
  }
  dll->tail = iobj;

  return true;
}

__dll_inline bool dll_is_equal(const dll_t * restrict const dll_a,
                               const dll_t * restrict const dll_b,
                               dll_callback_ext_fn_t fn_cmp,
                               void * restrict any) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll_a || NULL == dll_b)) {
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  if (dll_a->objs_count != dll_b->objs_count) {
    return false;
  }

  dll_obj_t * restrict iobj_a = dll_a->head;
  dll_obj_t * restrict iobj_b = dll_b->head;
  size_t i                    = 0;

  for (; iobj_a && iobj_b; iobj_a = iobj_a->next, iobj_b = iobj_b->next) {
    if (fn_cmp) {
      if (0 != fn_cmp(iobj_a->data, iobj_b->data, any, i++)) {
        return false;
      }
    } else {
      if (iobj_a->size != iobj_b->size || iobj_a->data != iobj_b->data) {
        return false;
      }
    }
  }

  return !(iobj_a || iobj_b);
}

__dll_inline bool dll_delete(dll_t * restrict dll, dll_obj_t * restrict obj) {
  dll_obj_t * restrict del_obj = dll_unlink(dll, obj);

#ifndef LIBDLL_UNSAFE_USAGE
  if (NULL == del_obj) {
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  const bool __ret = dll_free_obj(&del_obj);

  return __ret;
}

__dll_inline dll_obj_t * dll_unlink(dll_t * restrict dll, dll_obj_t * restrict obj) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == obj)) {
    return NULL;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

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

  obj->prev = NULL;
  obj->next = NULL;
  --dll->objs_count;

  return obj;
}

__dll_inline bool dll_free_obj(dll_obj_t * restrict * restrict obj) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == obj || NULL == *obj)) {
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  if ((*obj)->destructor) {
    if (LIBDLL_DESTRUCTOR_DEFAULT == (*obj)->destructor) {
      free((*obj)->data);
    } else {
      (*obj)->destructor((*obj)->data);
    }
  }

  (*obj)->data = NULL;
  free(*obj);
  *obj = NULL;
  return true;
}

__dll_inline bool dll_free(dll_t * restrict * restrict dll) {
#ifndef LIBDLL_UNSAFE_USAGE
  if (__dll_unlikely(NULL == dll || NULL == *dll)) {
    return false;
  }
#endif /* LIBDLL_UNSAFE_USAGE */

  dll_obj_t * restrict iobj = (*dll)->head;
  dll_obj_t * restrict save = NULL;

  while (iobj) {
    save = iobj->next;

#ifndef LIBDLL_UNSAFE_USAGE
    if (false == dll_free_obj(&iobj)) {
      return false;
    }
#else
    dll_free_obj(&iobj)
#endif /* LIBDLL_UNSAFE_USAGE */

    iobj = save;
  }

  free(*dll);
  *dll = NULL;

  return true;
}

#endif /* LIBDLL_H */
