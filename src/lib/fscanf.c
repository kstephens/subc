/*
 *	NMH's Simple C Compiler, 2012
 *	fscanf()
 */

#include <stdio.h>
#include <string.h>

extern int _vscan(int mode, void *src, char *fmt, void **varg);

int fscanf(void *last, ...) {
	void	**args;
	FILE	*f;
	char	*fmt, *p;

	args = &last;
	args += __argc;
	f = *--args;
	fmt = *--args;
	return _vscan(1, f, fmt, --args);
}
