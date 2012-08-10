/*
 *	NMH's Simple C Compiler, 2012
 *	fread()
 */

#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include <errno.h>

int _refill(FILE *f) {
	int	k;

	if (f->ptr >= f->end) {
		f->ptr = 0;
		k = _read(f->fd, f->buf, f->size);
		if (k <= 0) {
			f->end = 0;
			f->iom |= _FERROR;
			if (k < 0) errno = EIO;
			return 0;
		}
		f->end = k;
	}
	return 1;
}

int _fread(void *p, int size, FILE *f) {
	int	k, len, end, ptr, total;

	if ((f->iom & _FREAD) == 0) return 0;
	if (f->iom & _FERROR) return 0;
	f->last = _FREAD;
	total = size;
	if (f->ch != EOF) {
		*(char *)p++ = f->ch;
		f->ch = EOF;
		size--;
	}
	if ((f->mode & _IOACC) == _IONBF) {
		if ((total = _read(f->fd, p, size)) != size) {
			f->iom |= _FERROR;
			errno = EIO;
		}
		return total;
	}
	if (!_refill(f)) return 0;
	end = f->end;
	ptr = f->ptr;
	k = end - ptr;
	if (size < k) k = size;
	memcpy(p, f->buf + ptr, k);
	f->ptr += k;
	p += k;
	size -= k;
	len = f->size;
	while (size > len) {
		if (_read(f->fd, p, len) != len) {
			f->iom |= _FERROR;
			errno = EIO;
			return total-size;
		}
		p += len;
		size -= len;
	}
	if (size != 0) {
		if (!_refill(f)) return total-size;
		memcpy(p, f->buf, size);
		f->ptr = size;
	}
	return total;
}

int fread(void *p, int size, int count, FILE *f) {
	int	k;

	if ((k = _fread(p, size * count, f)) < 0)
		return -1;
	return k / size;
}
