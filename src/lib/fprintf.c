/*
 *	NMH's Simple C Compiler, 2012
 *	fprintf()
 */

#include <stdio.h>

extern int _vformat(int mode, int max, void *dest, char *fmt, void **varg);

int fprintf(void *last, ...) {
	void	**args;
	FILE	*f;
	char	*fmt, *p;

	args = &last;
	args += __argc;
	f = *--args;
	fmt = *--args;
	return _vformat(1, 0, f, fmt, --args);
}
