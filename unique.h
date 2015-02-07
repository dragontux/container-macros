#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define UNIQUE(T, N) \
	/* The main data container for unique pointers */ \
	typedef struct N##_unique_data {\
		T data;\
		bool released;\
		void (*dtor)(T ptr);\
	} N##_unique_data_t;\
	\
	/* The structure for released ("free") pointers,
	 * in order to have a seperate T for them */ \
	typedef struct N##_unique_data_free {\
		T data;\
		bool released;\
		void (*dtor)(T ptr);\
	} N##_unique_ret_t;\
	\
	/* The cleanup function. Calls the given destructor (if any) and frees the data
	 * if the contained pointer is not released. */ \
	static inline void N##_unique_cleanup( N##_unique_data_t *u ){\
		if ( u->data ){\
			if ( u->released ){\
				u->released = false;\
				\
			} else {\
				if ( u->dtor ){\
					u->dtor( u->data );\
				} else {\
					free( u->data );\
				}\
				u->data = NULL;\
			}\
		}\
	}\
	\
	/* Releases ownership from a unique pointer and returns a unique_data_free struct */ \
	static inline N##_unique_ret_t N##_release( N##_unique_data_t *u ){\
		struct N##_unique_data_free ret;\
		\
		u->released = ret.released = true;\
		ret.data = u->data;\
		ret.dtor = u->dtor;\
		u->data = NULL;\
		\
		return ret;\
	}\
	\
	/* Claims ownership of a previously released pointer */ \
	static inline N##_unique_data_t N##_obtain( N##_unique_ret_t u ){\
		struct N##_unique_data ret;\
		assert( u.released == true );\
		\
		ret.released = false;\
		ret.data = u.data;\
		ret.dtor = u.dtor;\
		\
		return ret;\
	}\
	\
	/* Moves ownership of a pointer from one container to another */ \
	static inline N##_unique_data_t N##_move( N##_unique_data_t *u ){\
		return N##_obtain( N##_release( u ));\
	}\
	\
	/* Returns a unique pointer container for the given pointer */ \
	static inline N##_unique_data_t N##_make_unique( T ptr ){\
		struct N##_unique_data ret = { ptr, false, NULL };\
		\
		return ret;\
	}

#define _unique_(OUTTYPE)     __attribute__((cleanup(OUTTYPE##_unique_cleanup)))
#define unique_t(OUTTYPE)     _unique_(OUTTYPE) OUTTYPE##_unique_data_t
#define unique_ret_t(OUTTYPE) OUTTYPE##_unique_ret_t
#define unique(OUTTYPE, NAME, DTOR ) _unique_(OUTTYPE) OUTTYPE##_unique_data_t NAME = \
                                    { .released = false, .data = NULL, .dtor = DTOR }; NAME.data
