/*
 *	NMH's Simple C Compiler, 2011,2012
 *	Miscellanea
 */

#include "defs.h"
#include "data.h"
#include "decl.h"

int chrpos(char *s, int c) {
	char	*p;

	p = strchr(s, c);
	return p? p-s: -1;
}

void copyname(char *name, char *s) {
	strncpy(name, s, NAMELEN);
	name[NAMELEN] = 0;
}

char *newfilename(char *file, int sfx) {
	char	*ofile;

	ofile = strdup(file);
	ofile[strlen(ofile)-1] = sfx;
	return ofile;
}

static FILE	*suspended;

void suspend(void) {
	suspended = Outfile;
	Outfile = NULL;
}

void resume(void) {
	Outfile = suspended;
	clear();
}

void match(int t, char *what) {
	if (Token == t) {
		Token = scan();
	}
	else {
		error("%s expected", what);
	}
}

void lparen(void) {
	match(LPAREN, "'('");
}

void rparen(void) {
	match(RPAREN, "')'");
}

void lbrace(void) {
	match(LBRACE, "'{'");
}

void rbrace(void) {
	match(RBRACE, "'}'");
}

void rbrack(void) {
	match(RBRACK, "']'");
}

void semi(void) {
	match(SEMI, "';'");
}

void colon(void) {
	match(COLON, "':'");
}

void ident(void) {
	match(IDENT, "identifier");
}

int eofcheck(void) {
	if (XEOF == Token) {
		error("missing '}'", NULL);
		return 1;
	}
	return 0;
}

int inttype(int p) {
	return PINT == p || PCHAR == p;
}

int comptype(int p) {
	p &= STCMASK;
	return p == PSTRUCT || p == PUNION;
}
