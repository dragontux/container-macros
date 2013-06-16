/*
 * llist.h: a CPP-based template implementation of singly-linked list
 *
 * Initialization of the list for a type:
 * LLIST_PROTO(TYPE, NAME): structs, typedefs and function prototypes for a
 *     list of elements of type TYPE and name NAME
 * LLIST(TYPE, NAME): functions for a list of TYPE elements and name NAME
 *
 * Types defined:
 * struct NAME: The list.
 * NAME: typedef of struct NAME.
 * struct NAME_pair: A pair of (value, next_pair), which are used to build
 *     the list.
 * NAME_pair: typedef of struct NAME_pair.
 * NAME_iterator: typedef of struct NAME_pair *, the iterator for the list.
 *
 * Usage:
 * NAME_new(): malloc a new list.
 * NAME_free(struct NAME *L): Frees the list L and all of its nodes.
 * NAME_insert(struct NAME *L, TYPE I, int P): Inserts the element I as a newly
 *     malloc'd node into list L at the position P. If P is equal to 0 or
 *     L->len (-1 means L->len), it is directly inserted to the
 *     front/back of the list. Otherwise, The list has to be traversed to the
 *     node at P.
 * NAME_pop(struct NAME *L, int P): Removes the element at position P from
 *     L and returns it (P equal to -1 means L->len), freeing its pair. If it's
 *     not the first/last element, the list has to be traversed to pair P.
 * NAME_get(struct NAME *L, int P): Returns the element at position P from
 *     L, value -1 means L->len. First/last elements are returned
 *     immediately, otherwise the list needs to be traversed.
 * NAME_set(struct NAME *L, TYPE I, int P): Assigns I to the element at
 *     position P (L->len if P is -1) of L. The list needs to be
 *     traversed to P if P is not 0 or L->len.
 * NAME_iterate(struct NAME *L): Returns an iterator for the list pointed to
 *     right before the first element.
 * NAME_next(struct NAME *L, NAME_iterator *I): Makes the iterator point to the
 *     next position in the list. Returns 0 if iterator is already on the end.
 * NAME_value(struct NAME *L, NAME_iterator *I): Returns the value at the
 *     current position of the iterator.
 *
 * The following fields of the list struct are also accessible (L is a list):
 * L->len: The number of elements in the list. Do not change.
 * L->first: The first pair in the list.
 * L->last: The last pair in the list.
 *
 * The following fields of the pair struct are also accessible (P is a pair):
 * P->car: The value of the current node.
 * P->cdr: The next node. Is NULL on the last element.
 *
 * To iterate the list manually, do something like this (L is a list):
 * struct NAME_pair *p;
 * for (p=L->first; p; p=p->cdr) {
 *     do_stuff(p->car);
 * }
 */

#ifndef LLIST_H_INCLUDED
#define LLIST_H_INCLUDED

#include <stdlib.h>

/* prototypes and structs for a list with elements of type T named N */
#define LLIST_PROTO(T, N) \
	struct N##_pair { T car; struct N##_pair *cdr; }; \
	typedef struct N##_pair N##_pair; \
	struct N { int len; struct N##_pair *first; struct N##_pair *last; }; \
	typedef struct N N; \
	typedef struct N##_pair *N##_iterator; \
	struct N *N##_new(void); \
	void N##_free(struct N *s); \
	struct N##_pair *N##_pair_new(T item); \
	int N##_insert(struct N *s, T item, int pos); \
	T N##_pop(struct N *s, int pos); \
	T N##_get(struct N *s, int pos); \
	void N##_set(struct N *s, T item, int pos); \
	N##_iterator N##_iterate(struct N *s); \
	int N##_next(struct N *s, N##_iterator *iter); \
	T N##_value(struct N *s, N##_iterator *iter)

/* defines functions for a list with elements of type T named N */
#define LLIST(T, N) \
	struct N *N##_new(void) \
	{ \
		struct N *s; \
		s = malloc(sizeof(struct N)); \
		if (!s) return NULL; \
		s->len = 0; \
		s->first = NULL; \
		s->last = NULL; \
		return s; \
	} \
	void N##_free(struct N *s) \
	{ \
		struct N##_pair *p, *temp; \
		temp = NULL; \
		for (p=s->first; p; ) { \
			temp = p; \
			p = p->cdr; \
			free(temp); \
		} \
		free(s); \
	} \
	struct N##_pair *N##_pair_new(T item) \
	{ \
		struct N##_pair *p; \
		p = malloc(sizeof(struct N##_pair)); \
		if (!p) return NULL; \
		p->car = item; \
		p->cdr = NULL; \
		return p; \
	} \
	int N##_insert(struct N *s, T item, int pos) \
	{ \
		int i; \
		struct N##_pair *newp, *p; \
		newp = N##_pair_new(item); \
		if (!newp) return 0; \
		if (!s->first) { \
			s->first = s->last = newp; \
			++s->len; \
			return 1; \
		} \
		if (pos < 0) { \
			s->last->cdr = newp; \
			s->last = newp; \
			++s->len; \
			return 1; \
		} \
		if (pos == 0) { \
			newp->cdr = s->first; \
			s->first = newp; \
			++s->len; \
			return 1; \
		} \
		for (p=s->first, i=0; i<pos-1 && p; ++i) p=p->cdr; \
		if (!p) { \
			free(newp); \
			return 0; \
		} \
		newp->cdr = p->cdr; \
		p->cdr = newp; \
		++s->len; \
		if (!newp->cdr || p == s->last) { \
			s->last = newp; \
		} \
		return 1; \
	} \
	T N##_pop(struct N *s, int pos) \
	{ \
		struct N##_pair *p, *p2; \
		T temp; \
		int i; \
		if (pos == 0) { \
			p = s->first; \
			s->first = p->cdr; \
			temp = p->car; \
			if (!p->cdr) s->last = NULL; \
			free(p); \
			--s->len; \
			return temp; \
		} \
		if (pos < 0) { \
			pos = s->len - 1; \
		} \
		for (p=s->first, i=0; i<pos-1 && p; ++i) p=p->cdr; \
		temp = p->cdr->car; \
		p2 = p->cdr; \
		p->cdr = p2->cdr; \
		if (!p->cdr) { \
			s->last = p; \
		} \
		free(p2); \
		--s->len; \
		return temp; \
	} \
	T N##_get(struct N *s, int pos) \
	{ \
		int i; \
		struct N##_pair *p; \
		if (pos == 0) { \
			return s->first->car; \
		} \
		if (pos < 0) { \
			return s->last->car; \
		} \
		for (p=s->first, i=0; i<pos && p; ++i) p=p->cdr; \
		return p->car; \
	} \
	void N##_set(struct N *s, T item, int pos) \
	{ \
		int i; \
		struct N##_pair *p; \
		if (pos == 0) { \
			s->first->car = item; \
		} else if (pos < 0) { \
			s->last->car = item; \
		} else { \
			for (p=s->first, i=0; i<pos && p; ++i) p=p->cdr; \
			p->car = item; \
		} \
	} \
	N##_iterator N##_iterate(struct N *s) \
	{ \
		return NULL; \
	} \
	int N##_next(struct N *s, N##_iterator *iter) \
	{ \
		if (!*iter) { \
			*iter = s->first; \
			return 1; \
		} \
		if (!(*iter)->cdr) { \
			return 0; \
		} \
		*iter = (*iter)->cdr; \
		return 1; \
	} \
	T N##_value(struct N *s, N##_iterator *iter) \
	{ \
		return (*iter)->car; \
	} \
	struct N /* to avoid extra semicolon outside of a function */

#endif /* ifndef LLIST_H_INCLUDED */
