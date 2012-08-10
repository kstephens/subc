/*
 *	NMH's Simple C Compiler, 2012
 *	kprintf()
 */

#include <syscall.h>

extern int _vformat(int mode, int max, void *dest, char *fmt, void **varg);

int kprintf(void *last, ...) {
	void	**args;
	int	fd;
	char	*fmt, *p;

	args = &last;
	args += __argc;
	fd = (int) *--args;
	fmt = *--args;
	return _vformat(-1, 0, (void *) fd, fmt, --args);
}
