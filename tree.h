/* tree.h: a CPP-based template implemention of a (unbalanced) binary tree */

#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED 1

#include <stdlib.h>

#define TREE_PROTO(T, N) \
	typedef struct N N; \
	typedef struct N##_node N##_node; \
	typedef struct N##_iterator N##_iterator; \
	N *N##_new(void); \
	void N##_free(N *s); \
	N##_node *N##_node_new(T item);

#define TREE(T, N) \
	struct N { size_t items; N##_node *top; }; \
	struct N##_node { T item; N##_node *left; N##_node *right; }; \
	struct N##_iterator /* { TODO } */; \
	N *N##_new(void) \
	{ \
		N *s; \
		s = calloc(1, sizeof(N)); \
		return s; \
	} \
	\
	void N##_free(N *s) \
	{ \
		if (s) { \
			N##_free(s->left); \
			n##_free(s->right); \
			free(s); \
		} \
	} \
	\
	N##_node *N##_node_new(T item) \
	{ \
		N *s = calloc(1, sizeof(N##_node)); \
		if (s) { \
			s->item = s; \
		} \
		return s; \
	}
	struct N
	
#endif
