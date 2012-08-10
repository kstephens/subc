/*
 *	NMH's Simple C Compiler, 2011,2012
 *	_vformat()
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int _fwrite(void *buf, int len, FILE *f);

static int	ofd;
static FILE	*outf;
static int	olen;
static int	limit;
static char	*vbuf;
static int	err;

/*
 * Convert integer N to string. Write the string to P.
 * P must point to the *end* of the buffer initially;
 * the output is written backward!
 * BASE is the numeric base (8,10,16). SGNCH points
 * to a buffer to which ITOA writes the sign of N. The
 * sign is *not* included in the buffer P.
 */

static char *itoa(char *p, int n, int base, char *sgnch) {
	int	s = 0, a = 'a';

	if (base < 0) base = -base, a = 'A';
	if (n < 0) s = 1, n = -n;
	*--p = 0;
	while (n || 0 == *p) {
		*--p = n % base + '0';
		if (n % base > 9) *p += a-10-'0';
		n /= base;
	}
	if (s) *sgnch = '-';
	return p;
}

static void append(char *what, int len) {
	int	k;

	if (ofd >= 0) {
		if (_write(ofd, what, len) != len)
			err = 1;
		olen += len;
	}
	else if (outf) {
		if (_fwrite(what, len, outf) != len)
			err = 1;
		olen += len;
	}
	else if (0 == limit) {
		memcpy(vbuf + olen, what, len);
		olen += len;
		vbuf[olen] = 0;
	}
	else {
		k = limit-olen-1;
		len = len < k? len: k;
		memcpy(vbuf + olen, what, len);
		olen += len;
		vbuf[olen] = 0;
	}
}

/*
 * Format and write output in the style of PRINTF.
 * When MODE==0, write the output to DEST (assuming
 * that DEST is a CHAR*).
 * When MODE==1, assume that DEST is a FILE* and
 * write the output to that stream.
 * In MODE==0, write at most MAX characters to DEST
 * where MAX==0 means unlimited).
 */

int _vformat(int mode, int max, void *dest, char *fmt, void **varg) {
	#define BUFLEN	256

	char	lbuf[BUFLEN], *p, *end;
	int	left, len, alt, k;
	char	pad[1], sgnch[2], *pfx;
	int	na = 0;

	end = &lbuf[BUFLEN];
	sgnch[1] = 0;
	if (0 == mode) {
		outf = NULL;
		ofd = -1;
		vbuf = dest;
		*vbuf = 0;
	}
	else if (-1 == mode) {
		outf = NULL;
		ofd = (int) dest;
		err = 0;
	}
	else {
		outf = dest;
		ofd = -1;
		err = 0;
	}
	olen = 0;
	limit = max;
	while (*fmt) {
		left = len = 0;
		*sgnch = 0;
		pfx = "";
		if ('%' == *fmt && fmt[1] != '%') {
			fmt++;
			*pad = ' ';
			alt = 0;
			while (strchr("-+0 #", *fmt)) {
				if ('-' == *fmt) {
					left = 1, *pad = ' ';
					fmt++;
				}
				else if ('0' == *fmt) {
					if (!left) *pad = '0';
					fmt++;
				}
				else if ('#' == *fmt) {
					alt = 1;
					fmt++;
				}
				else if ('+' == *fmt) {
					*sgnch = '+';
					fmt++;
				}
				else if (' ' == *fmt) {
					if (!*sgnch) *sgnch = ' ';
					fmt++;
				}
			}
			if ('*' == *fmt)
				len = (int) *varg--, fmt++;
			else
				while (isdigit(*fmt))
					len = len * 10 + *fmt++ - '0';
			switch (*fmt++) {
			case 'c':
				*pad = ' ';
				*sgnch = 0;
				lbuf[0] = (char) *varg--;
				lbuf[1] = 0;
				p = lbuf;
				na++;
				break;
			case 'd':
				p = itoa(end, (int) *varg--, 10, sgnch);
				na++;
				break;
			case 'n':
				p = itoa(end, olen, 10, sgnch);
				break;
			case 'o':
				p = itoa(end, (int) *varg--, 8, sgnch);
				if (alt) pfx = "0";
				na++;
				break;
			case 's':
				*sgnch = 0;
				*pad = ' ';
				p = *varg--;
				na++;
				break;
			case 'p':
			case 'x':
			case 'X':
				k = 'X' == fmt[-1]? -16: 16;
				p = itoa(end, (int) *varg--, k, sgnch);
				if (alt) pfx = k<0? "0X": "0x";
				na++;
				break;
			}
		}
		else {
			if ('%' == *fmt) fmt++;
			lbuf[0] = *fmt++;
			lbuf[1] = 0;
			p = lbuf;
		}
		k = strlen(p) + strlen(pfx) + strlen(sgnch);
		if ('0' == *pad) {
			if (*sgnch) append(sgnch, 1);
			append(pfx, strlen(pfx));
			pfx = "";
		}
		while (!left && len-- > k)
			append(pad, 1);
		if (*sgnch && *pad != '0') {
			append(sgnch, 1);
			append(pfx, strlen(pfx));
		}
		if (!*sgnch) append(pfx, strlen(pfx));
		append(p, strlen(p));
		while (left && len-- > k)
			append(pad, 1);
		if (outf && err) break;
	}
	return na;
}
