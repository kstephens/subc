/*
 *	NMH's Simple C Compiler, 2012
 *	scanf()
 */

#include <stdio.h>

extern int _vscan(int mode, void *dest, char *fmt, void **varg);

int scanf(void *last, ...) {
	void	**args;
	char	*fmt, *p;

	args = &last;
	args += __argc;
	fmt = *--args;
	return _vscan(1, stdin, fmt, --args);
}
