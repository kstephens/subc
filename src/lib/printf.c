/*
 *	NMH's Simple C Compiler, 2012
 *	printf()
 */

#include <stdio.h>

extern int _vformat(int mode, int max, void *dest, char *fmt, void **varg);

int printf(void *last, ...) {
	void	**args;
	char	*fmt, *p;

	args = &last;
	args += __argc;
	fmt = *--args;
	return _vformat(1, 0, stdout, fmt, --args);
}
