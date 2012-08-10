/*
 *	NMH's Simple C Compiler, 2011,2012
 *	malloc()
 */

#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
#include <errno.h>

#define THRESHOLD	128

int		*_arena = 0;
int		_asize;
static int	*freep;

static void defrag(void) {
	int	*p, *q, *end;

	end = _arena + _asize;
	for (p = _arena; p < end; p += abs(*p)) {
		if (*p > 0) {
			for (q = p; q < end && *q > 0; q += *q)
				;
			*p = q - p;
		}
	}
}

void *malloc(int size) {
	int	*p, *end;
	int	k, n, tries;

	size = (size + sizeof(int) - 1) / sizeof(int);
	if (NULL == _arena) {
		if (size >= THRESHOLD)
			_asize = size + 1;
		else
			_asize = size * 50;
		_arena = _sbrk(_asize * sizeof(int));
		if (_arena == (int *)-1) {
			errno = ENOMEM;
			return NULL;
		}
		_arena[0] = _asize;
		freep = _arena;
	}
	for (tries = 0; tries < 3; tries++) {
		end = _arena + _asize;
		p = freep;
		do {
			if (*p > size) {
				if (size + 1 == *p) {
					*p = -*p;
				}
				else {
					k = *p;
					*p = -(size+1);
					p[size+1] = k - size - 1;
				}
				freep = p;
				return p+1;
			}
			p += abs(*p);
			if (p == end) p = _arena;
			if (p < _arena || p >= end || 0 == *p) {
				_write(2, "malloc(): corrupt arena\n", 24);
				abort();
			}
		} while (p != freep);
		if (0 == tries)
			defrag();
		else {
			if (size >= THRESHOLD)
				n = size + 1;
			else
				n = size * 50;
			if (_sbrk(n * sizeof(int)) == (void *)-1) {
				errno = ENOMEM;
				return NULL;
			}
			k = _asize;
			_asize += n;
			*end = _asize - k;
		}
	}
	errno = ENOMEM;
	return NULL;
}
