/*
 *	NMH's Simple C Compiler, 2011,2012
 *	sprintf()
 */

#include <stdio.h>
#include <string.h>

extern int _vformat(int mode, int max, void *dest, char *fmt, void **varg);

int sprintf(void *last, ...) {
	void	**args;
	char	*buf;
	char	*fmt, *p;

	args = &last;
	args += __argc;
	buf = *--args;
	fmt = *--args;
	return _vformat(0, 0, buf, fmt, --args);
}
