/*
 *	NMH's Simple C Compiler, 2012
 *	fseek()
 */

#include <stdio.h>
#include <syscall.h>
#include <errno.h>

int fseek(FILE *f, int pos, int how) {
	int	adjust = 0;
	char	b[101];

	if (how != SEEK_SET && how != SEEK_CUR && how != SEEK_END) {
		errno = EINVAL;
		return -1;
	}
	if (SEEK_CUR == how && (f->mode & _IOACC) != _IONBF) {
		adjust = f->end - f->ptr;
		if (_FREAD == f->last) {
			adjust = -adjust;
			if (f->ch != EOF) adjust--;
		}
	}
	if (fflush(f) < 0) return -1;
	f->ch = EOF;
	if ((pos = _lseek(f->fd, pos + adjust, how)) < 0) {
		errno = EIO;
		return -1;
	}
	return pos;
}
