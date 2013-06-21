/*
 * T = uint32_t, 32-bit GNU/Linux OS, -O0
 *
 * max  | len   | cap   | load  | eff   | coll  | bytes | value
 * -----|-------+-------+-------+-------+-------+-------+------
 * 0.20 | 102   | 512   | 0.199 | 0.166 | 0.059 | 72.35 | 0.467
 * 0.50 | 256   | 512   | 0.500 | 0.331 | 0.227 | 36.28 | 0.483
 * 0.80 | 409   | 512   | 0.799 | 0.439 | 0.298 | 27.34 | 0.647
 * 1.00 | 512   | 512   | 1.000 | 0.485 | 0.350 | 24.75 | 0.672
 * 1.25 | 640   | 512   | 1.250 | 0.528 | 0.416 | 22.74 | 0.670
 * 1.50 | 768   | 512   | 1.500 | 0.559 | 0.474 | 21.45 | 0.659
 * 2.00 | 1024  | 512   | 2.000 | 0.603 | 0.570 | 19.89 | 0.638
 * 2.50 | 1280  | 512   | 2.500 | 0.625 | 0.634 | 19.20 | 0.616
 * 3.00 | 1536  | 512   | 3.000 | 0.638 | 0.681 | 18.81 | 0.598
 * 4.00 | 2048  | 512   | 4.000 | 0.656 | 0.753 | 18.28 | 0.571
 * 5.00 | 2560  | 512   | 5.000 | 0.670 | 0.801 | 17.92 | 0.560
 * 6.00 | 3072  | 512   | 6.000 | 0.667 | 0.834 | 18.00 | 0.533
 * 7.00 | 3584  | 512   | 7.000 | 0.669 | 0.857 | 17.94 | 0.522
 * 8.00 | 4096  | 512   | 8.000 | 0.682 | 0.875 | 17.58 | 0.532
 *
 * max: HMAP_MAX_LOAD
 * len: number of map entries
 * cap: number of buckets
 * load: len/cap
 * eff: used space / total space
 * coll: % of entries that are collisions
 * bytes: bytes/entry
 * value: eff*eff/coll; TODO: improve formula?
 */

#include <stdio.h>
#include "hmap.h"

#define T uint32_t

typedef struct stats {
	int len;
	int cap;
	int map;
	int used;
	int unused;
	int collisions;
} stats;

T randt(void);
int cmp(T a, T b);
uint32_t hash(T n);

HMAP_PROTO(int, int, map);
HMAP(int, int, map, cmp, hash);

int cmp(T a, T b)
{
	return a - b;
}

uint32_t hash(T n)
{
	return n;
}

T randt(void)
{
	T t;
	uint32_t *p;
	int i;

	p = (uint32_t *)&t;
	for (i=0; i<sizeof(T)/sizeof(uint32_t); ++i) {
		*p = rand();
		++p;
	}

	return t;
}

stats map_stats(const map *m)
{
	stats s;
	map_bucket *b;
	int i;

	s.len = m->len;
	s.cap = m->cap;
	s.map = 0;
	s.used = 0;
	s.unused = 0;
	s.collisions = 0;

	s.map += sizeof(struct map);
	for (i=0; i<m->cap; ++i) {
		b = &m->buckets[i];
		s.map += sizeof(struct map_bucket);
		s.used += b->len * sizeof(struct map_entry);
		s.unused += (b->cap-b->len) * sizeof(struct map_entry);
		if (b->len > 1) {
			s.collisions += b->len - 1;
		}
	}

	return s;
}

void print_header(void)
{
	printf("hmap BUCKET_SIZE:%d, MIN_CAP:%d, MIN_LOAD:%g, MAX_LOAD:%g\n", HMAP_BUCKET_SIZE, HMAP_MIN_CAP, HMAP_MIN_LOAD, HMAP_MAX_LOAD);
	printf("%-5s | %-5s | %-5s | %-5s | %-5s | %-5s\n", "len", "cap", "load", "eff", "coll", "bytes");
	printf("------+-------+-------+-------+-------+------\n");
}

void print_stats(stats s)
{
	printf("%-5d | %-5d | %.3f | %.3f | %.3f | %2.2f\n", s.len, s.cap, s.len*1.0/s.cap, s.used*1.0/(s.used+s.unused+s.map), s.collisions*1.0/s.len, (s.map+s.used+s.unused)*1.0/s.len);
}

int main(void)
{
	int i;
	map *m;
	int lastcap;
	stats s;

	srand(0);

	m = map_new();
	lastcap = m->cap;
	s = map_stats(m);

	print_header();
	for (i=0; i<=5; ++i)
	{
		while (m->cap == lastcap) {
			s = map_stats(m);
			map_set(m, randt(), randt());
		}
		print_stats(s);
		lastcap = m->cap;
	}

	return 0;
}
