/*
 *	NMH's Simple C Compiler, 2012
 *	time()
 */

#include <time.h>
#include <syscall.h>

int time(int *tp) {
	int	t;

	t = _time();
	if (tp) *tp = t;
	return t;
}
