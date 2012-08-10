/*
 *	NMH's Simple C Compiler, 2011,2012
 *	remove()
 */

#include <syscall.h>

int remove(char *path) {
	return _unlink(path);
}
