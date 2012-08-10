/*
 *	NMH's Simple C Compiler, 2011,2012
 *	C runtime initialization
 */

#include <stdio.h>
#include <errno.h>

int	errno = EOK;

FILE	*_files[FOPEN_MAX];

FILE	*stdin, *stdout, *stderr;

void _init(void) {
	int	i;

	for (i=0; i<FOPEN_MAX; i++)
		_files[i] = NULL;
	stdin = fdopen(0, "r");
	stdout = fdopen(1, "w");
	stderr = fdopen(2, "w");
	stdin->mode = _IOLBF;
	stderr->mode = _IOLBF;
}
