# container-macros
### Generic template implementations of common types using C preprocessor

## lists
Included are array list (vector) and linked list templates.

### common list methods (NAME is the name of the list, TYPE is the type of its elements):
Types defined:
- `NAME` - the list, used as a pointer; public fields:
    - `int len` - the number of list elements
- `NAME_iterator` - a list iterator, used as a value

Functions defined:
- `NAME *NAME_new(void)` - allocate a new list
- `void NAME_free(NAME *list)` - free the list
- `int NAME_insert(NAME *list, TYPE value, int pos)` - inserts an item to position `pos`, returns `0` on failure
- `TYPE NAME_pop(NAME *list, int pos)` - removes the item at position `pos` from `list` and returns it
- `TYPE NAME_get(const NAME *list, int pos)` - retrieves the item at position `pos`
- `void NAME_set(NAME *list, TYPE value, int pos)` - sets the item on position `pos` to `value`
- `NAME_iterator NAME_iterate(const NAME *list)` - creates a new list iterator, `NAME_next` must be called before accessing the value at its position
- `int NAME_next(const NAME *list, NAME_iterator *iter)` - moves `iter` to the next position, returns `0` if there are no more elements
- `TYPE NAME_get_at(const NAME *list, NAME_iterator iter)` - returns the value at the current position of `iter`
- `void NAME_set_at(NAME *list, TYPE value, NAME_iterator iter)` - sets the item at the current position of `iter` to `value`
- `int NAME_insert_after(NAME *list, TYPE value, NAME_iterator iter)` - inserts `value` to `list` right after the position `iter` points to
- `TYPE NAME_pop_after(NAME *list, NAME_iterator iter)` - removes the item at one position after `iter` from `list` and returns it; undefined when `iter` points to the last item in the list

All `int pos` parameters have a special value `-1`, which is equivalent to `list->len` in insert and `list->len-1` in pop/get/set and insert/pop/get/set on that position are O(1) in both linked and array lists.

To iterate a list, do something like `NAME_iterator i; for (i=NAME_iterate(list); NAME_next(list, &i);) { do_something(NAME_get_at(list, i)); }`

See [list-example.c](list-example.c) for list examples and more documentation.

### alist.h
alist.h implements an array list (vector), which is basically an array that is reallocated to double its length once more space is required.

#### alist-specific functionality
Macros:
- `ALIST_PROTO(TYPE, NAME)` - macro for header entries for alist containing elements of type `TYPE`, named `NAME`
- `ALIST(TYPE, NAME)` - macro for functions for alist

Types defined:
- `NAME` - a struct representing the arraylist; fields:
    - `int len` - number of list elements, decrement (but not below 0) to pop elements from the end of the list
    - `int cap` - the length of the underlying array, do not change its value
    - `TYPE *arr` - the array of `TYPE` elements of length `cap`, of which the first `len` elements are defined
- `NAME_iterator` - a typedef of `int` representing an index in the array; inserting or popping an element prior to or at the position of the iterator invalidates it

Additional functions defined:
- `NAME *NAME_new_cap(int cap)` - allocates a new alist with initial capacity `cap` (`NAME_new` uses 8); `NAME_insert` will double the capacity each time it needs more space
- `int NAME_resize(NAME *list, int size)` - reallocs the list's capacity to `size`, truncates elements if `size < list->len`

The `_at` and `_after` functions are no faster than the versions used with an index; get and set are O(1), insert and pop are O(n) except on the tail, where they are O(1).

To manually iterate an alist, do `int i; for (i=0; i<list->len; ++i) { do_something(list->arr[i]); }`

### llist.h
llist.h implements a singly-linked list.

#### llist-specific functionality
Macros:
- `LLIST_PROTO(TYPE, NAME)` - macro for header entries for llist containing `TYPE` elements, named `NAME`
- `LLIST(TYPE, NAME)` - macro for functions for llist

Types defined:
- `NAME` - a struct representing the linked list; fields:
    - `int len` - number of elements in the list, has no effect on list functions
    - `NAME_pair *first` - the first element in the list
    - `NAME_pair *last` - the last element in the list
- `NAME_pair` - a list element; fields:
    - `TYPE car` - the value
    - `NAME_pair *cdr` - the next element in the list
- `NAME_iterator` - typedef of `NAME_pair *` used to traverse the list; popping the item at the current position of the iterator invalidates it

Additional functions defined:
- `NAME_pair *NAME_pair_new(TYPE value)` - allocates a new list element with the value `value`

List inserts, pops, gets and sets on head and tail are O(1), O(n) elsewhere. `_at` and `_after` functions are all O(1).

To manually iterate a llist, do `NAME_pair *p; for (p=list->first; p; p=p->cdr) { do_something(p->car); }`

---

## maps
Included is a hashmap template.

### Hashmap functionality
Macros:
- `HMAP_PROTO(KEY_TYPE, VALUE_TYPE, NAME)` - macro for prototypes for the hashmap of `KEY_TYPE` -&gt; `VALUE_TYPE`, named `NAME`
- `HMAP(KEY_TYPE, VALUE_TYPE, NAME, CMP_FUNC, HASH_FUNC)` - macro for hmap functions; `int CMP_FUNC(KEY_TYPE a, KEY_TYPE b)` is used to compare keys (return a value &lt;0 if `a<b`, 0 when `a==b` and &gt;0 when `a>b`) and `uint32_t HASH_FUNC(KEY_TYPE key)` to generate hashes

Types defined:
- `NAME` - the hashmap; fields:
    - `int len` - the number of map entries
    - `int cap` - the number of buckets
    - `NAME_entry *buckets` - the hashmap buckets
    - `double max_load` - the load (`len/cap`) before growing to double size, default 2.0; 0 to disable automatic growth at insert
    - `double min_load` - the load (`len/cap`) before resizing to half size, default 0.5; 0 to disable automatic shrinking at delete
- `NAME_bucket` - a bucket with entries for hash collisions; fields:
    - `int len` - the number of entries in the bucket
    - `int cap` - the length of the `entries` array
    - `NAME_entry *entries` - an array with entries
- `NAME_entry` - a struct representing a map entry; fields:
    - `uint32_t hash` - the hash of the key
    - `KEY_TYPE key` - the key
    - `VALUE_TYPE value` - the value
- `NAME_iterator` - a struct used for traversing the map, invalidated with any delete or a set when no such key exists; fields:
    - `int bucket` - the index of the current bucket
    - `int entry` - the index of the entry in the current bucket

Functions defined:
- `NAME *NAME_new(void)` - calls `NAME_new_cap(16)`
- `NAME *NAME_new_cap(int cap)` - allocates a new hmap with `cap` buckets.
- `void NAME_free(NAME *map)` - frees the map
- `int NAME_resize(NAME *map, int cap)` - resizes the map to `cap`; returns 1 on success and 0 on malloc failure
- `KEY_TYPE NAME_get(const NAME *map, KEY_TYPE key)` - retrieves the item with key `key`; return value is the zeroed `VALUE_TYPE` when no such key exists in the map
- `int NAME_contains(const NAME *map, KEY_TYPE key)` - returns `1` if `key` exists in the map, `0` otherwise
- `int NAME_get_contains(const NAME *map, KEY_TYPE key, VALUE_TYPE *value)` - sets `*value` to the value associated with `key` and returns `1` if `key` exists in the map, otherwise sets `*value` to a zeroed `VALUE_TYPE` and returns 0
- `int NAME_set(NAME *map, KEY_TYPE key, VALUE_TYPE value)` - sets the map entry with key `key` to `value` overwriting an existing entry with such key if it exists; returns 0 on malloc failure, 1 otherwise
- `int NAME_delete(NAME *map, KEY_TYPE key)` - removes the value associated with `key` from the map if it exists, otherwise does nothing; returns 1 if an entry was deleted, 0 otherwise
- `NAME_iterator NAME_iterate(NAME *map)` - creates a new map iterator, `NAME_next` must be called before accessing the key or value at its position
- `int NAME_next(const NAME *map, NAME_iterator *iter)` - moves `iter` to the next position, returns `0` if there are no more entries
- `KEY_TYPE NAME_key_at(const NAME *map, NAME_iterator iter)` - returns the key at the current position of the iterator
- `VALUE_TYPE NAME_value_at(const NAME *map, NAME_iterator iter)` - returns the value at the current position of the iterator

The hashmap is automatically resized to twice its current capacity once its load reaches 2.0 and to half its size when the load falls under 0.5.

See [map-example.c](map-example.c) for map examples and more documentation.

---

All code compiles with GCC with the following CFLAGS: `-Wall -Werror -ansi -pedantic -pedantic-errors`
