/* hmap.h: a CPP-based template implementation of a hashmap */

#ifndef HMAP_H_INCLUDED
#define HMAP_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define HMAP_BUCKET_SIZE 1 /* starting bucket size */
#define HMAP_MIN_CAP 16 /* minimum number of buckets when autoresizing down */
#define HMAP_MAX_LOAD 2.0 /* max load before growing to twice the current capacity; negative to disable */
#define HMAP_MIN_LOAD 0.5 /* min load before shrinking to half; negative to disable; must be less than HMAP_MAX_LOAD/2 */

#define HMAP_PROTO(K, V, N) \
	typedef struct N##_entry { uint32_t hash; K key; V value; } N##_entry; \
	typedef struct N##_bucket { int len; int cap; struct N##_entry *entries; } N##_bucket; \
	typedef struct N { int len; int cap; struct N##_bucket *buckets; double max_load; double min_load; } N; \
	typedef struct N##_iterator { int bucket; int entry; } N##_iterator; \
	N *N##_new(void); \
	N *N##_new_cap(int cap); \
	void N##_free(N *map); \
	int N##_resize(N *map, int cap); \
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
		return N##_new_cap(HMAP_MIN_CAP); \
	} \
	N *N##_new_cap(int cap) \
	{ \
		N *map; \
		map = malloc(sizeof(struct N)); \
		if (!map) return NULL; \
		map->len = 0; \
		map->cap = cap; \
		map->max_load = HMAP_MAX_LOAD; \
		map->min_load = HMAP_MIN_LOAD; \
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
	int N##_resize(N *map, int cap) \
	{ \
		N##_bucket *buckets, *oldb, *newb; \
		N##_entry *entries; \
		int i, j, k, newcap; \
		buckets = malloc(cap * sizeof(struct N##_bucket)); \
		if (!buckets) return 0; \
		memset(buckets, 0, cap*sizeof(struct N##_bucket)); \
		for (i=0; i<map->cap; ++i) { \
			oldb = &map->buckets[i]; \
			for (j=0; j<oldb->len; ++j) { \
				newb = &buckets[oldb->entries[j].hash%cap]; \
				if (newb->cap == newb->len) { \
					newcap = newb->cap>0 ? 2*newb->cap : HMAP_BUCKET_SIZE; \
					entries = realloc(newb->entries, newcap*sizeof(struct N##_entry)); \
					if (!entries) { \
						for (k=0; cap; ++k) if (buckets[k].cap > 0) free(buckets[k].entries); \
						free(buckets); \
						return 0; \
					} \
					newb->entries = entries; \
					newb->cap = newcap; \
				} \
				newb->entries[newb->len++] = oldb->entries[j]; \
			} \
		} \
		for (i=0; i<map->cap; ++i) { \
			if (map->buckets[i].cap > 0) free(map->buckets[i].entries); \
		} \
		free(map->buckets); \
		map->cap = cap; \
		map->buckets = buckets; \
		return 1; \
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
	int N##_set(N *_hmap_map, K _hmap_key, V _hmap_value) \
	{ \
		N##_bucket *_hmap_bucket; \
		N##_entry *_hmap_tmp; \
		int _hmap_i; \
		uint32_t _hmap_hash; \
		_hmap_hash = H(_hmap_key); \
		_hmap_bucket = &_hmap_map->buckets[_hmap_hash%_hmap_map->cap]; \
		for (_hmap_i=0; _hmap_i<_hmap_bucket->len; ++_hmap_i) { \
			if (_hmap_bucket->entries[_hmap_i].hash==_hmap_hash && !C(_hmap_bucket->entries[_hmap_i].key, _hmap_key)) { \
				_hmap_bucket->entries[_hmap_i].value = _hmap_value; \
				return 1; \
			} \
		} \
		if (_hmap_bucket->len == _hmap_bucket->cap) { \
			if (!_hmap_bucket->cap) { \
				_hmap_bucket->entries = malloc(HMAP_BUCKET_SIZE * sizeof(struct N##_entry)); \
				if (!_hmap_bucket->entries) return 0; \
				_hmap_bucket->cap = HMAP_BUCKET_SIZE; \
			} else { \
				_hmap_tmp = realloc(_hmap_bucket->entries, 2*_hmap_bucket->cap*sizeof(struct N##_entry)); \
				if (!_hmap_tmp) return 0; \
				_hmap_bucket->entries = _hmap_tmp; \
				_hmap_bucket->cap *= 2; \
			} \
		} \
		_hmap_bucket->entries[_hmap_bucket->len].key = _hmap_key; \
		_hmap_bucket->entries[_hmap_bucket->len].value = _hmap_value; \
		_hmap_bucket->entries[_hmap_bucket->len].hash = _hmap_hash; \
		++_hmap_bucket->len; \
		++_hmap_map->len; \
		if (_hmap_map->max_load >= 0 && _hmap_map->len*1.0/_hmap_map->cap > _hmap_map->max_load) { \
			N##_resize(_hmap_map, 2*_hmap_map->cap); \
		} \
		return 1; \
	} \
	int N##_delete(N *_hmap_map, K _hmap_key) \
	{ \
		N##_bucket *_hmap_bucket; \
		int _hmap_i; \
		uint32_t _hmap_hash; \
		N##_entry *_hmap_tmp; \
		_hmap_hash = H(_hmap_key); \
		_hmap_bucket = &_hmap_map->buckets[_hmap_hash%_hmap_map->cap]; \
		for (_hmap_i=0; _hmap_i<_hmap_bucket->len; ++_hmap_i) { \
			if (_hmap_bucket->entries[_hmap_i].hash==_hmap_hash && !C(_hmap_bucket->entries[_hmap_i].key, _hmap_key)) { \
				for (; _hmap_i+1<_hmap_bucket->len; ++_hmap_i) _hmap_bucket->entries[_hmap_i] = _hmap_bucket->entries[_hmap_i+1]; \
				--_hmap_bucket->len; \
				--_hmap_map->len; \
				if (_hmap_map->min_load >= 0 && _hmap_map->len*1.0/_hmap_map->cap < _hmap_map->min_load && _hmap_map->cap > HMAP_MIN_CAP) { \
					N##_resize(_hmap_map, _hmap_map->cap/2>HMAP_MIN_CAP ? _hmap_map->cap/2 : HMAP_MIN_CAP); \
				} else if (_hmap_bucket->len < _hmap_bucket->cap/2) { \
					_hmap_tmp = realloc(_hmap_bucket->entries, _hmap_bucket->cap/2*sizeof(struct N##_entry)); \
					if (_hmap_tmp) { \
						_hmap_bucket->entries = _hmap_tmp; \
						_hmap_bucket->cap /= 2; \
					} \
				} \
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
