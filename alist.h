/*
 * alist.h: a CPP-based template implementation of array lists (resizable array)
 *
 * Initialization of the arraylist for a type:
 * ALIST_PROTO(TYPE, NAME): structs, typedefs and function prototypes for an
 *     arraylist of elements of type TYPE and name NAME
 * ALIST(TYPE, NAME): functions for an arraylist of TYPE elements and name NAME
 *
 * Types defined:
 * struct NAME: The arraylist.
 * NAME: typedef of struct NAME.
 * NAME_iterator: typedef of int, the iterator for the arraylist.
 *
 * Usage:
 * NAME_new(void): malloc a new arraylist with initial capacity 8.
 * NAME_new_cap(int S): malloc a new arraylist with initial capacity S.
 * NAME_free(struct NAME *A): Frees the arraylist A and its underlying array.
 * NAME_insert(struct NAME *A, TYPE I, int P): Inserts the element I into
 *     arraylist A at the position P. If P is equal to A->len or -1,
 *     it is directly inserted to the end of the arraylist. Otherwise, all
 *     elements from P to A->len-1 are shifted before I is inserted. If
 *     necessary (A->cap == A->len), arraylist's underlying array is also
 *     resized, its capacity doubled.
 * NAME_pop(struct NAME *A, int P): Removes the element at position P from
 *     A and returns it. If P equalt to -1 or A->len, the last
 *     element is immediately removed. Otherwise, all elements from P to
 *     A->len are shifted one place backwards.
 * NAME_get(struct NAME *A, int P): Returns the element at position P from
 *     A, value -1 means A->len.
 * NAME_set(struct NAME *A, TYPE I, int P): Assigns I to the element at
 *     position P (A->len if P is -1) of A.
 * NAME_resize(struct NAME *A, int S): Resizes the arraylist to hold at most S
 *     elements, removes elements with indexes larger than S. Can be used
 *     as NAME_resize(A, A->len) to trim the arraylist to hold exactly the
 *     number of elements it currently has.
 * NAME_iterate(struct NAME *A): Returns an iterator for the arraylist pointed
 *     right before the first element.
 * NAME_next(struct NAME *A, NAME_iterator *I): Points the iterator to the
 *     next position in the list. Returns 0 if iterator is already on the end.
 * NAME_get_at(struct NAME *L, NAME_iterator P): Returns the value at the
 *     current position of the iterator.
 * NAME_set_at(struct NAME *L, TYPE I, NAME_iterator P): Assigns I to the
 *     element at the current position of the iterator.
 * NAME_insert_after(struct NAME *A, TYPE I, NAME_iterator P): Inserts the
 *     element I as a newly malloc'd node into list L at the next position
 *     after the one P points to. Equivalent to NAME_insert(L, *P+1).
 * NAME_pop_after(struct NAME *L, NAME_iterator P): Removes the element at the
 *     next position after the one P points to and returns it. Equivalent to
 *     NAME_pop(L, *P+1). Undefined when P points to the last item in the list.
 *
 * The following fields of the struct are also accessible (A is an arraylist):
 * A->cap: The current capacity of the underlying array. Do not change.
 * A->len: The number of elements currently in the arraylist. Decrement (but
 *     not below 0) to remove an element from the tail of the arraylist.
 * A->arr: The underlying array. Direct access is okay as long as you do
 *     bounds checking - the length of the array is A->cap, while only the
 *     elements up to A->len are in use, so you should threat it as an array
 *     of A->cap with the first A->len elements defined.
 *
 * To iterate the list manually, do something like this (A is a list):
 * int i;
 * for (i=0; i<A->len; ++i) {
 *     do_stuff(A->arr[i]);
 * }
 */

#ifndef ALIST_H_INCLUDED
#define ALIST_H_INCLUDED

#include <stdlib.h>

/* prototypes for an arraylist with elements of type T named N */
#define ALIST_PROTO(T, N) \
	struct N { int cap; int len; T *arr; }; \
	typedef struct N N; \
	typedef int N##_iterator; \
	struct N *N##_new(void); \
	struct N *N##_new_cap(int size); \
	void N##_free(struct N *s); \
	int N##_insert(struct N *s, T item, int pos); \
	T N##_pop(struct N *s, int pos); \
	T N##_get(struct N *s, int pos); \
	void N##_set(struct N *s, T item, int pos); \
	int N##_resize(struct N *s, int size); \
	N##_iterator N##_iterate(struct N *s); \
	int N##_next(struct N *s, N##_iterator *iter); \
	T N##_get_at(struct N *s, N##_iterator iter); \
	void N##_set_at(struct N *s, T item, N##_iterator iter); \
	int N##_insert_after(struct N *s, T item, N##_iterator iter); \
	T N##_pop_after(struct N *s, N##_iterator iter)

/* defines functions for an arraylist with elements of type T named N */
#define ALIST(T, N) \
	struct N *N##_new(void) \
	{ \
		return N##_new_cap(8); \
	} \
	struct N *N##_new_cap(int size) \
	{ \
		struct N *s; \
		s = malloc(sizeof(struct N)); \
		if (!s) return NULL; \
		s->cap = size; \
		s->len = 0; \
		s->arr = malloc(size * sizeof(T)); \
		if (!s->arr) { free(s); return NULL; } \
		return s; \
	} \
	void N##_free(struct N *s) \
	{ \
		free(s->arr); \
		free(s); \
	} \
	int N##_insert(struct N *s, T item, int pos) \
	{ \
		T *temp; \
		int i; \
		if (s->len >= s->cap) { \
			temp = realloc(s->arr, s->cap*2*sizeof(T)); \
			if (!temp) return 0; \
			s->arr = temp; \
			s->cap *= 2; \
		} \
		if (pos >= 0 && pos != s->len) { \
			for (i=s->len; i>pos; --i) { \
				s->arr[i] = s->arr[i-1]; \
			} \
		} \
		s->arr[pos<0?s->len:pos] = item; \
		s->len++; \
		return 1; \
	} \
	T N##_pop(struct N *s, int pos) \
	{ \
		T temp; \
		int i; \
		if (pos < 0) return s->arr[--s->len]; \
		temp = s->arr[pos]; \
		for (i=pos; i<s->len-1; ++i) { \
			s->arr[i] = s->arr[i+1]; \
		} \
		--s->len; \
		return temp; \
	} \
	T N##_get(struct N *s, int pos) \
	{ \
		return s->arr[pos<0||pos>=s->len?s->len-1:pos]; \
	} \
	void N##_set(struct N *s, T item, int pos) \
	{ \
		s->arr[pos<0||pos>=s->len?s->len-1:pos] = item; \
	} \
	int N##_resize(struct N *s, int size) \
	{ \
		T *temp; \
		temp = realloc(s->arr, size*sizeof(T)); \
		if (!temp) return 0; \
		s->arr = temp; \
		if (size < s->len) s->len = size; \
		s->cap = size; \
		return 1; \
	} \
	N##_iterator N##_iterate(struct N *s) \
	{ \
		return -1; \
	} \
	int N##_next(struct N *s, N##_iterator *iter) \
	{ \
		if (*iter < 0) { \
			++*iter; \
			return s->len > 0; \
		} \
		if (*iter >= s->len-1) { \
			return 0; \
		} \
		++*iter; \
		return 1; \
	} \
	T N##_get_at(struct N *s, N##_iterator iter) \
	{ \
		return s->arr[iter]; \
	} \
	void N##_set_at(struct N *s, T item, N##_iterator iter) \
	{ \
		s->arr[iter] = item; \
	} \
	int N##_insert_after(struct N *s, T item, N##_iterator iter) \
	{ \
		return N##_insert(s, item, iter+1); \
	} \
	T N##_pop_after(struct N *s, N##_iterator iter) \
	{ \
		return N##_pop(s, iter+1); \
	} \
	struct N /* to avoid extra semicolon outside of a function */

#endif /* ifndef ALIST_H_INCLUDED */
