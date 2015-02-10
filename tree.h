/* tree.h: a CPP-based template implemention of a (unbalanced) binary tree */

#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED 1

#include <stdlib.h>

#define TREE_PROTO(T, N) \
	typedef struct N N; \
	/* typedef struct N##_iterator N##_iterator; TODO, maybe */ \
	void N##_free_all(N *s); \
	N *N##_new(T item); \
	N *N##_construct(T item, N *left, N *right); \
	size_t N##_size(const N *tree);

#define TREE(T, N) \
	struct N { T item; N *left; N *right; }; \
	N *N##_new(T item) \
	{ \
		N *s = calloc(1, sizeof(N)); \
		if (s) { \
			s->item = item; \
		} \
		return s; \
	} \
	\
	void N##_free_all(N *s) \
	{ \
		if (s) { \
			N##_free_all(s->left); \
			N##_free_all(s->right); \
			free(s); \
		} \
	} \
	\
	N *N##_construct(T item, N *left, N *right) \
	{ \
		N *s = calloc(1, sizeof(N)); \
		if (s) { \
			s->item = item; \
			s->left = left; \
			s->right = right; \
		} \
		return s; \
	} \
	\
	size_t N##_size(const N *tree) \
	{ \
		if (tree) { \
			return \
				N##_size(tree->left) + \
				N##_size(tree->right) + \
				1; \
		} else { \
			return 0; \
		} \
	} \
	struct N
	
#endif
