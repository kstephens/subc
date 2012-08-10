/*
 *	NMH's Simple C Compiler, 2012
 *	realloc()
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void *realloc(void *p, int size) {
	void	*new;
	int	*pi, k;

	if ((new = malloc(size)) == NULL)
		return NULL;
	if (p) {
		pi = p;
		k = pi[-1] < size? pi[-1]: size;
		memcpy(new, p, k);
		free(p);
	}
	return new;
}
