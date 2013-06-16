# container-macros
### Generic template implementations of common types using C preprocessor

## lists
Included are array list (vector) and linked list templates.

### common list methods (NAME is the name of the list, TYPE is the type of its elements):
- `NAME *NAME_new(void)` - allocate a new list
- `void NAME_free(NAME *list)` - free the list
- `int NAME_insert(NAME *list, TYPE value, int pos)` - inserts an item to position `pos`, returns `0` on failure
- `TYPE NAME_pop(NAME *list, int pos)` - removes the item at position `pos` from `list` and returns it
- `TYPE NAME_get(NAME *list, int pos)` - retrieves the item at position `pos`
- `void NAME_set(NAME *list, TYPE value, int pos)` - sets the item on position `pos` to `value`
- `NAME_iterator NAME_iterate(NAME *list)` - creates a new list iterator, `NAME_next` must be called before accessing the value at its position
- `int NAME_next(NAME *list, NAME_iterator *iter)` - moves `iter` to the next position, returns `0` if there are no more elements
- `TYPE NAME_get_at(NAME *list, NAME_iterator iter)` - returns the value at the current position of `iter`
- `void NAME_set_at(NAME *list, TYPE value, NAME_iterator iter)` - sets the item at the current position of `iter` to `value`
- `int NAME_insert_after(NAME *list, TYPE value, NAME_iterator iter)` - inserts `value` to `list` right after the position `iter` points to
- `TYPE NAME_pop_after(NAME *list, NAME_iterator iter)` - removes the item at one position after `iter` from `list` and returns it; undefined when `iter` points to the last item in the list

`NAME_list *` also has a field, `int len`, accessible like `list->len`. It represents the current length of the list and generally shouldn't be modified.

All `int pos` parameters have a special value `-1`, which is equivalent to `list->len` and insert/pop/get/set on that position are O(1) in both linked and array lists.

A type `NAME_iterator` is also defined. It is used to iterate throught the list.

### alist.h
alist.h implements an array list (vector), which is basically an array that is reallocated to double its length once more space is required.

#### alist-specific functionality
- `ALIST_PROTO(TYPE, NAME)` - macro for prototypes for alist types and methods, put in a header
- `ALIST(TYPE, NAME)` - macro for functions for alist, put in a .c file
- `NAME *NAME_new_cap(int cap)` - allocates a new alist with initial capacity `cap` (`NAME_new` uses 8)
- `int NAME_resize(NAME *list, int size)` - reallocs the list's capacity to `size`, truncates elements if `size<list->len`

The list's length field can be decremented to remove elements from the end of the list (but it must not become negative). To remove one element from the tail of a non-empty alist, do `list->len--`.

In addition to `list->len`, there's also a `list->cap`, which is an `int` and holds the list's array's current capacity. Don't modify it.
The list stores its element in `list->arr`, which is a `TYPE *` of length `list->cap`, but only the first `list->len` elements are defined. This array can be reallocated and moved on any insert.

List inserts or pops to anywhere but the tail are O(n), as the elements must be shifted. Get and set are O(1).

`NAME_iterator` is actually `int`. It is an index to the list. The `NAME_iterate` function returns -1. Inserting or removing an element prior to or on the iterator's position invalidates the iterator.

To manually iterate an alist, do `int i; for (i=0; i<list->len; ++i) { do_something(list->arr[i]); }`

### llist.h
llist.h implements a singly-linked list.
#### llist-specific functionality
In addition to `len`, a llist struct has the fields:
- `NAME_pair *first` - the first element in the list, `NULL` in an empty list
- `NAME_pair *last` - the last element in the list, `NULL` in an empty list and `list->first` if there's only one element

A struct `NAME_pair` is also defined along with the list. It holds these fields:
- `TYPE *car` - the value of the list element
- `NAME_pair *cdr` - the next element on the list, NULL if this is the last element

Associated function:
- `NAME_pair *NAME_pair_new(TYPE value)` - allocates a new list element with the value `value`

List inserts, pops, gets and sets on head and tail are O(1), O(n) elsewhere.

`NAME_iterator` is actually `NAME_pair *`. The iterator is the element it points to. `NAME_iterate` returns `NULL` and `NAME_next` returns `list->head` when given `NULL` as the iterator. Popping the item at the current position of the iterator from the list invalidates the iterator.

`NAME_get_at`, `NAME_set_at`, `NAME_insert_after` and `NAME_pop_after` are O(1) regardless of the position the iterator is at.

To manually iterate a llist, do `NAME_pair *p; for (p=list->first; p; p=p->cdr) { do_something(p->car); }`

---

All code compiles with GCC with the following CFLAGS: `-Wall -Werror -ansi -pedantic -pedantic-errors`

See [list-example.c](list-example.c) for examples and more documentation.
