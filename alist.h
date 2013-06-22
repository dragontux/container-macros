/* alist.h: a CPP-based template implementation of array lists (resizable array) */

#ifndef ALIST_H_INCLUDED
#define ALIST_H_INCLUDED

#include <stdlib.h>

#define ALIST_PROTO(T, N) \
	typedef struct N { int cap; int len; T *arr; } N; \
	typedef int N##_iterator; \
	N *N##_new(void); \
	N *N##_new_cap(int size); \
	void N##_free(N *s); \
	int N##_insert(N *s, T item, int pos); \
	T N##_pop(N *s, int pos); \
	T N##_get(const N *s, int pos); \
	void N##_set(N *s, T item, int pos); \
	int N##_resize(N *s, int size); \
	N##_iterator N##_iterate(const N *s); \
	int N##_next(const N *s, N##_iterator *iter); \
	T N##_get_at(const N *s, N##_iterator iter); \
	void N##_set_at(N *s, T item, N##_iterator iter); \
	int N##_insert_after(N *s, T item, N##_iterator iter); \
	T N##_pop_after(N *s, N##_iterator iter)

/* defines functions for an arraylist with elements of type T named N */
#define ALIST(T, N) \
	N *N##_new(void) \
	{ \
		return N##_new_cap(8); \
	} \
	N *N##_new_cap(int _alist_size) \
	{ \
		N *_alist_s; \
		_alist_s = malloc(sizeof(struct N)); \
		if (!_alist_s) return NULL; \
		_alist_s->cap = _alist_size; \
		_alist_s->len = 0; \
		_alist_s->arr = malloc(_alist_size * sizeof(T)); \
		if (!_alist_s->arr) { free(_alist_s); return NULL; } \
		return _alist_s; \
	} \
	void N##_free(N *s) \
	{ \
		free(s->arr); \
		free(s); \
	} \
	int N##_insert(N *_alist_s, T _alist_item, int _alist_pos) \
	{ \
		T *_alist_temp; \
		int _alist_i; \
		if (_alist_s->len >= _alist_s->cap) { \
			_alist_temp = realloc(_alist_s->arr, _alist_s->cap*2*sizeof(T)); \
			if (!_alist_temp) return 0; \
			_alist_s->arr = _alist_temp; \
			_alist_s->cap *= 2; \
		} \
		if (_alist_pos >= 0 && _alist_pos != _alist_s->len) { \
			for (_alist_i=_alist_s->len; _alist_i>_alist_pos; --_alist_i) { \
				_alist_s->arr[_alist_i] = _alist_s->arr[_alist_i-1]; \
			} \
		} \
		_alist_s->arr[_alist_pos<0?_alist_s->len:_alist_pos] = _alist_item; \
		_alist_s->len++; \
		return 1; \
	} \
	T N##_pop(N *s, int pos) \
	{ \
		T temp; \
		int i; \
		if (pos < 0) return s->arr[s->len-1]; \
		temp = s->arr[pos]; \
		for (i=pos; i<s->len-1; ++i) { \
			s->arr[i] = s->arr[i+1]; \
		} \
		--s->len; \
		return temp; \
	} \
	T N##_get(const N *s, int pos) \
	{ \
		return s->arr[pos<0||pos>=s->len?s->len-1:pos]; \
	} \
	void N##_set(N *s, T item, int pos) \
	{ \
		s->arr[pos<0||pos>=s->len?s->len-1:pos] = item; \
	} \
	int N##_resize(N *_alist_s, int _alist_size) \
	{ \
		T *_alist_temp; \
		_alist_temp = realloc(_alist_s->arr, _alist_size*sizeof(T)); \
		if (!_alist_temp) return 0; \
		_alist_s->arr = _alist_temp; \
		if (_alist_size < _alist_s->len) _alist_s->len = _alist_size; \
		_alist_s->cap = _alist_size; \
		return 1; \
	} \
	N##_iterator N##_iterate(const N *s) \
	{ \
		return -1; \
	} \
	int N##_next(const N *s, N##_iterator *iter) \
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
	T N##_get_at(const N *s, N##_iterator iter) \
	{ \
		return s->arr[iter]; \
	} \
	void N##_set_at(N *s, T item, N##_iterator iter) \
	{ \
		s->arr[iter] = item; \
	} \
	int N##_insert_after(N *s, T item, N##_iterator iter) \
	{ \
		return N##_insert(s, item, iter+1); \
	} \
	T N##_pop_after(N *s, N##_iterator iter) \
	{ \
		return N##_pop(s, iter+1); \
	} \
	struct N /* to avoid extra semicolon outside of a function */

#endif /* ifndef ALIST_H_INCLUDED */
