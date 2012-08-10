/*
 *	NMH's Simple C Compiler, 2012
 *	sscanf()
 */

#include <stdio.h>
#include <string.h>

extern int _vscan(int mode, void *src, char *fmt, void **varg);

int sscanf(void *last, ...) {
	void	**args;
	char	*buf;
	char	*fmt, *p;

	args = &last;
	args += __argc;
	buf = *--args;
	fmt = *--args;
	return _vscan(0, buf, fmt, --args);
}
