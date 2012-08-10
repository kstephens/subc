/*
 *	NMH's Simple C Compiler, 2012
 *	fputs()
 */

#include <stdio.h>
#include <string.h>

int _fwrite(char *p, int len, FILE *f);

int fputs(char *s, FILE *f) {
	int	k;

	k = strlen(s);
	if (_fwrite(s, k, f) != k)
		return EOF;
	return k;
}
