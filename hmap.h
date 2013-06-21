/* hmap.h: a CPP-based template implementation of a hashmap */

#ifndef HMAP_H_INCLUDED
#define HMAP_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HMAP_PROTO(K, V, N) \
	typedef struct N##_entry { uint32_t hash; K key; V value; } N##_entry; \
	typedef struct N##_bucket { int len; int cap; struct N##_entry *entries; } N##_bucket; \
	typedef struct N { int len; int cap; struct N##_bucket *buckets; } N; \
	typedef struct N##_iterator { int bucket; int entry; } N##_iterator; \
	N *N##_new(void); \
	N *N##_new_cap(int cap); \
	void N##_free(N *map); \
	V N##_get(const N *map, K key); \
	int N##_contains(const N *map, K key); \
	int N##_get_contains(const N *map, K key, V *value); \
	int N##_set(N *map, K key, V value); \
	int N##_delete(N *map, K key); \
	N##_iterator N##_iterate(const N *map); \
	int N##_next(const N *map, N##_iterator *iter); \
	K N##_key_at(const N *map, const N##_iterator iter); \
	V N##_value_at(const N *map, const N##_iterator iter)

#define HMAP(K, V, N, C, H) \
	N *N##_new(void) \
	{ \
		return N##_new_cap(256); \
	} \
	N *N##_new_cap(int cap) \
	{ \
		N *map; \
		map = malloc(sizeof(struct N)); \
		if (!map) return NULL; \
		map->len = 0; \
		map->cap = cap; \
		map->buckets = malloc(cap * sizeof(struct N##_bucket)); \
		if (!map->buckets) { \
			free(map); \
			return NULL; \
		} \
		memset(map->buckets, 0, cap*sizeof(struct N##_bucket)); \
		return map; \
	} \
	void N##_free(N *map) \
	{ \
		int i; \
		for (i=0; i<map->cap; ++i) { \
			if (map->buckets[i].cap > 0) free(map->buckets[i].entries); \
		} \
		free(map->buckets); \
		free(map); \
	} \
	V N##_get(const N *map, K key) \
	{ \
		V value; \
		N##_get_contains(map, key, &value); \
		return value; \
	} \
	int N##_contains(const N *map, K key) \
	{ \
		V value; \
		return N##_get_contains(map, key, &value); \
	} \
	/* variable names prefixed with _hmap_ because of sizeof(V) */ \
	int N##_get_contains(const N *_hmap_map, K _hmap_key, V *_hmap_value) \
	{ \
		N##_bucket *_hmap_bucket; \
		int _hmap_i; \
		uint32_t _hmap_hash;\
		_hmap_hash = H(_hmap_key); \
		_hmap_bucket = &_hmap_map->buckets[_hmap_hash%_hmap_map->cap]; \
		for (_hmap_i=0; _hmap_i<_hmap_bucket->len; ++_hmap_i) { \
			if (_hmap_bucket->entries[_hmap_i].hash==_hmap_hash && !C(_hmap_bucket->entries[_hmap_i].key, _hmap_key)) { \
				*_hmap_value = _hmap_bucket->entries[_hmap_i].value; \
				return 1; \
			} \
		} \
		memset(_hmap_value, 0, sizeof(V)); \
		return 0; \
	} \
	int N##_set(N *map, K key, V value) \
	{ \
		N##_bucket *bucket; \
		N##_entry *tmp; \
		int i; \
		uint32_t hash; \
		hash = H(key); \
		bucket = &map->buckets[hash%map->cap]; \
		for (i=0; i<bucket->len; ++i) { \
			if (bucket->entries[i].hash==hash && !C(bucket->entries[i].key, key)) { \
				bucket->entries[i].value = value; \
				return 1; \
			} \
		} \
		if (bucket->len == bucket->cap) { \
			if (!bucket->cap) { \
				bucket->entries = malloc(sizeof(struct N##_entry)); \
				if (!bucket->entries) return 0; \
				bucket->cap = 1; \
			} else { \
				tmp = realloc(bucket->entries, 2*bucket->cap*sizeof(struct N##_entry)); \
				if (!tmp) return 0; \
				bucket->entries = tmp; \
				bucket->cap *= 2; \
			} \
		} \
		bucket->entries[bucket->len].key = key; \
		bucket->entries[bucket->len].value = value; \
		bucket->entries[bucket->len].hash = hash; \
		++bucket->len; \
		++map->len; \
		return 1; \
	} \
	int N##_delete(N *map, K key) \
	{ \
		N##_bucket *bucket; \
		int i; \
		uint32_t hash; \
		hash = H(key); \
		bucket = &map->buckets[hash%map->cap]; \
		for (i=0; i<bucket->len; ++i) { \
			if (bucket->entries[i].hash==hash && !C(bucket->entries[i].key, key)) { \
				for (; i+1<bucket->len; ++i) bucket->entries[i] = bucket->entries[i+1]; \
				--bucket->len; \
				--map->len; \
				return 1; \
			} \
		} \
		return 0; \
	} \
	N##_iterator N##_iterate(const N *map) \
	{ \
		N##_iterator iter; \
		iter.bucket = 0; \
		iter.entry = -1; \
		return iter; \
	} \
	int N##_next(const N *map, N##_iterator *iter) \
	{ \
		int i; \
		if (iter->entry+1 < map->buckets[iter->bucket].len) { \
			++iter->entry; \
			return 1; \
		} \
		for (i=iter->bucket+1; i<map->cap; ++i) { \
			if (map->buckets[i].len > 0) { \
				iter->bucket = i; \
				iter->entry = 0; \
				return 1; \
			} \
		} \
		return 0; \
	} \
	K N##_key_at(const N *map, const N##_iterator iter) \
	{ \
		return map->buckets[iter.bucket].entries[iter.entry].key; \
	} \
	V N##_value_at(const N *map, const N##_iterator iter) \
	{ \
		return map->buckets[iter.bucket].entries[iter.entry].value; \
	} \
	struct N /* to avoid extra semicolon outside of a function */

#endif /* ifndef HMAP_H_INCLUDED */
