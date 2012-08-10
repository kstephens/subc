/*
 *	NMH's Simple C Compiler, 2011,2012
 *	Code generator (emitter)
 */

#include "defs.h"
#include "data.h"
#include "decl.h"
#include "cgen.h"

int	Acc = 0;

void clear(void) {
	Acc = 0;
}

void load(void) {
	Acc = 1;
}

int label(void) {
	static int id = 1;

	return id++;
}

void spill(void) {
	if (Acc) genpush();
}

void genraw(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "%s", s);
}

void gen(char *s) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "\t%s\n", s);
}

void ngen(char *s, char *inst, int n) {
	if (NULL == Outfile) return;
	fputc('\t', Outfile);
	fprintf(Outfile, s, inst, n);
	fputc('\n', Outfile);
}

void ngen2(char *s, char *inst, int n, int a) {
	if (NULL == Outfile) return;
	fputc('\t', Outfile);
	fprintf(Outfile, s, inst, n, a);
	fputc('\n', Outfile);
}

void lgen(char *s, char *inst, int n) {
	if (NULL == Outfile) return;
	fputc('\t', Outfile);
	fprintf(Outfile, s, inst, LPREFIX, n);
	fputc('\n', Outfile);
}

void lgen2(char *s, int v1, int v2) {
	if (NULL == Outfile) return;
	fputc('\t', Outfile);
	fprintf(Outfile, s, v1, LPREFIX, v2);
	fputc('\n', Outfile);
}

void sgen(char *s, char *inst, char *s2) {
	if (NULL == Outfile) return;
	fputc('\t', Outfile);
	fprintf(Outfile, s, inst, s2);
	fputc('\n', Outfile);
}

void sgen2(char *s, char *inst, int v, char *s2) {
	if (NULL == Outfile) return;
	fputc('\t', Outfile);
	fprintf(Outfile, s, inst, v, s2);
	fputc('\n', Outfile);
}

void genlab(int id) {
	if (NULL == Outfile) return;
	fprintf(Outfile, "%c%d:", LPREFIX, id);
}

char *labname(int id) {
	static char	name[100];

	sprintf(name, "%c%d", LPREFIX, id);
	return name;
}

char *gsym(char *s) {
	static char	name[NAMELEN+2];

	name[0] = PREFIX;
	name[1] = 0;
	strcat(name, s);
	return name;
}

/* administrativa */

void gendata(void) {
	if (Textseg) cgdata();
	Textseg = 0;
}

void gentext(void) {
	if (!Textseg) cgtext();
	Textseg = 1;
}

void genprelude(void) {
	Textseg = 0;
	gentext();
	cgprelude();
}

void genpostlude(void) {
	cgpostlude();
}

void genname(char *name) {
	genraw(gsym(name));
	genraw(":");
}

void genpublic(char *name) {
	cgpublic(gsym(name));
}

/* loading values */

void genaddr(int y) {
	gentext();
	spill();
	if (CAUTO == Stcls[y])
		cgldla(Vals[y]);
	else if (CLSTATC == Stcls[y])
		cgldsa(Vals[y]);
	else
		cgldga(gsym(Names[y]));
	load();
}

void genldlab(int id) {
	gentext();
	spill();
	cgldlab(id);
	load();
}

void genlit(int v) {
	gentext();
	spill();
	cglit(v);
	load();
}

void genargc(void) {
	gentext();
	spill();
	cgargc();
	load();
}

/* binary ops */

void genand(void) {
	gentext();
	cgpop2();
	cgand();
}

void genior(void) {
	gentext();
	cgpop2();
	cgior();
}

void genxor(void) {
	gentext();
	cgpop2();
	cgxor();
}

void genshl(int swap) {
	gentext();
	cgpop2();
	if (swap) cgswap();
	cgshl();
}

void genshr(int swap) {
	gentext();
	cgpop2();
	if (swap) cgswap();
	cgshr();
}

static int ptr(int p) {
	int	sp;

	sp = p & STCMASK;
	return INTPTR == p || INTPP == p ||
		CHARPTR == p || CHARPP == p ||
		VOIDPTR == p || VOIDPP == p ||
		STCPTR == sp || STCPP == sp ||
		UNIPTR == sp || UNIPP == sp ||
		FUNPTR == p;
}

static int needscale(int p) {
	int	sp;

	sp = p & STCMASK;
	return INTPTR == p || INTPP == p || CHARPP == p || VOIDPP == p ||
		STCPTR == sp || STCPP == sp || UNIPTR == sp || UNIPP == sp;
}

int genadd(int p1, int p2) {
	int	rp = PINT;

	gentext();
	if (ptr(p1)) {
		if (needscale(p1)) {
			if (	(p1 & STCMASK) == STCPTR ||
				(p1 & STCMASK) == UNIPTR
			)
				cgscaleby(objsize(deref(p1), TVARIABLE, 1));
			else
				cgscale();
		}
		cgpop2();
		rp = p1;
	}
	else if (ptr(p2)) {
		cgpop2();
		if (needscale(p2)) {
			if (	(p2 & STCMASK) == STCPTR ||
				(p2 & STCMASK) == UNIPTR
			)
				cgscale2by(objsize(deref(p2), TVARIABLE, 1));
			else
				cgscale2();
		}
		rp = p2;
	}
	else {
		cgpop2();
	}
	cgadd();
	return rp;
}

int gensub(int p1, int p2, int swap) {
	int	rp = PINT;

	gentext();
	cgpop2();
	if (swap) cgswap();
	if (!inttype(p1) && !inttype(p2) && p1 != p2)
		error("incompatible pointer types in binary '-'", NULL);
	if (ptr(p1) && !ptr(p2)) {
		if (needscale(p1)) {
			if (	(p1 & STCMASK) == STCPTR ||
				(p1 & STCMASK) == UNIPTR
			)
				cgscale2by(objsize(deref(p1), TVARIABLE, 1));
			else
				cgscale2();
		}
		rp = p1;
	}
	cgsub();
	if (needscale(p1) && needscale(p2)) {
		if (	(p1 & STCMASK) == STCPTR ||
			(p1 & STCMASK) == UNIPTR
		)
			cgunscaleby(objsize(deref(p1), TVARIABLE, 1));
		else
			cgunscale();
	}
	return rp;
}

void genmul(void) {
	gentext();
	cgpop2();
	cgmul();
}

void gendiv(int swap) {
	gentext();
	cgpop2();
	if (swap) cgswap();
	cgdiv();
}

void genmod(int swap) {
	gentext();
	cgpop2();
	if (swap) cgswap();
	cgmod();
}

static void binopchk(int op, int p1, int p2) {
	if (ASPLUS == op)
		op = PLUS;
	else if (ASMINUS == op)
		op = MINUS;
	if (inttype(p1) && inttype(p2))
		return;
	else if (comptype(p1) || comptype(p2))
		/* fail */;
	else if (PLUS == op && (inttype(p1) || inttype(p2)))
		return;
	else if (MINUS == op && (!inttype(p1) || inttype(p2)))
		return;
	else if ((EQUAL == op || NOTEQ == op || LESS == op ||
		 GREATER == op || LTEQ == op || GTEQ == op)
		&&
		(p1 == p2 ||
		 VOIDPTR == p1 && !inttype(p2) ||
		 VOIDPTR == p2 && !inttype(p1))
	)
		return;
	error("invalid operands to binary operator", NULL);
}

int genbinop(int op, int p1, int p2) {
	binopchk(op, p1, p2);
	switch (op) {
	case PLUS:	return genadd(p1, p2);
	case MINUS:	return gensub(p1, p2, 1);
	case STAR:	genmul(); break;
	case SLASH:	gendiv(1); break;
	case MOD:	genmod(1); break;
	case LSHIFT:	genshl(1); break;
	case RSHIFT:	genshr(1); break;
	case AMPER:	genand(); break;
	case CARET:	genxor(); break;
	case PIPE:	genior(); break;
	case EQUAL:	cgeq(); break;
	case NOTEQ:	cgne(); break;
	case LESS:	cglt(); break;
	case GREATER:	cggt(); break;
	case LTEQ:	cgle(); break;
	case GTEQ:	cgge(); break;
	}
	return PINT;
}

/* unary ops */

void genbool(void) {
	gentext();
	cgbool();
}

void genind(int p) {
	gentext();
	if (PCHAR == p)
		cgindb();
	else
		cgindw();
}

void genlognot(void) {
	gentext();
	cglognot();
}

void genneg(void) {
	gentext();
	cgneg();
}

void gennot(void) {
	gentext();
	cgnot();
}

void genscale(void) {
	gentext();
	cgscale();
}

void genscale2(void) {
	gentext();
	cgscale2();
}

/* jump/call/function ops */

void genjump(int dest) {
	gentext();
	cgjump(dest);
}

void genbrfalse(int dest) {
	gentext();
	cgbrfalse(dest);
}

void genbrtrue(int dest) {
	gentext();
	cgbrtrue(dest);
}

void gencall(int y) {
	gentext();
	cgcall(gsym(Names[y]));
	load();
}

void gencalr(void) {
	gentext();
	cgcalr();
	load();
}

void genentry(void) {
	gentext();
	cgentry();
}

void genexit(void) {
	gentext();
	cgexit();
}

void genpush(void) {
	gentext();
	cgpush();
}

void genpushlit(int n) {
	gentext();
	spill();
	cgpushlit(n);
}

void genstack(int n) {
	if (n) {
		gentext();
		cgstack(n);
	}
}

void genlocinit(void) {
	int	i;

	gentext();
	for (i=0; i<Nli; i++)
		cginitlw(LIval[i], LIaddr[i]);
}

/* data definitions */

void genbss(char *name, int len) {
	gendata();
	cgbss(name, (len + INTSIZE-1) / INTSIZE * INTSIZE);
}

void genalign(int k) {
	gendata();
	while (k++ % INTSIZE)
		cgdefb(0);
}

void gendefb(int v) {
	gendata();
	cgdefb(v);
}

void gendefl(int id) {
	gendata();
	cgdefl(id);
}

void gendefp(int v) {
	gendata();
	cgdefp(v);
}

void gendefs(char *s, int len) {
	int	i;

	gendata();
	for (i=1; i<len-1; i++) {
		if (isalnum(s[i]))
			cgdefc(s[i]);
		else
			cgdefb(s[i]);
	}
}

void gendefw(int v) {
	gendata();
	cgdefw(v);
}

/* increment ops */

static void genincptr(int *lv, int inc, int pre) {
	int	y, size;

	size = objsize(deref(lv[LVPRIM]), TVARIABLE, 1);
	gentext();
	y = lv[LVSYM];
	if (!y && !pre) cgldinc();
	if (!pre) rvalue(lv);
	if (!y) {
		if (pre)
			if (inc)
				cginc1pi(size);
			else
				cgdec1pi(size);
		else
			if (inc)
				cginc2pi(size);
			else
				cgdec2pi(size);
	}
	else if (CAUTO == Stcls[y]) {
		if (inc)
			cgincpl(Vals[y], size);
		else
			cgdecpl(Vals[y], size);
	}
	else if (CLSTATC == Stcls[y]) {
		if (inc)
			cgincps(Vals[y], size);
		else
			cgdecps(Vals[y], size);
	}
	else {
		if (inc)
			cgincpg(gsym(Names[y]), size);
		else
			cgdecpg(gsym(Names[y]), size);
	}
	if (pre) rvalue(lv);
}

void geninc(int *lv, int inc, int pre) {
	int	y, b;

	gentext();
	y = lv[LVSYM];
	if (needscale(lv[LVPRIM])) {
		genincptr(lv, inc, pre);
		return;
	}
	b = PCHAR == lv[LVPRIM];
	/* will duplicate move to aux register in (*char)++ */
	if (!y && !pre) cgldinc();
	if (!pre) rvalue(lv);
	if (!y) {
		if (pre)
			if (inc)
				b? cginc1ib(): cginc1iw();
			else
				b? cgdec1ib(): cgdec1iw();
		else
			if (inc)
				b? cginc2ib(): cginc2iw();
			else
				b? cgdec2ib(): cgdec2iw();
	}
	else if (CAUTO == Stcls[y]) {
		if (inc)
			b? cginclb(Vals[y]): cginclw(Vals[y]);
		else
			b? cgdeclb(Vals[y]): cgdeclw(Vals[y]);
	}
	else if (CLSTATC == Stcls[y]) {
		if (inc)
			b? cgincsb(Vals[y]): cgincsw(Vals[y]);
		else
			b? cgdecsb(Vals[y]): cgdecsw(Vals[y]);
	}
	else {
		if (inc)
			b? cgincgb(gsym(Names[y])):
			   cgincgw(gsym(Names[y]));
		else
			b? cgdecgb(gsym(Names[y])):
			   cgdecgw(gsym(Names[y]));
	}
	if (pre) rvalue(lv);
}

/* switch table generator */

void genswitch(int *vals, int *labs, int nc, int dflt) {
	int	i, ltbl;

	ltbl = label();
	gentext();
	cgldswtch(ltbl);
	cgcalswtch();
	gendata();
	genlab(ltbl);
	gendefw(nc);
	for (i = 0; i < nc; i++)
		cgcase(vals[i], labs[i]);
	gendefl(dflt);
}

/* assigments */

void genasop(int op, int p1, int p2) {
	binopchk(op, p1, p2);
	switch (op) {
	case ASDIV:	gendiv(0); break;
	case ASMUL:	genmul(); break;
	case ASMOD:	genmod(0); break;
	case ASPLUS:	genadd(p2, p1); break;
	case ASMINUS:	gensub(p2, p1, 0); break;
	case ASLSHIFT:	genshl(0); break;
	case ASRSHIFT:	genshr(0); break;
	case ASAND:	genand(); break;
	case ASXOR:	genxor(); break;
	case ASOR:	genior(); break;
	}
}

void genstore(int op, int *lv, int *lv2) {
	if (NULL == lv) return;
	gentext();
	if (ASSIGN != op) {
		if (lv[LVSYM]) rvalue(lv);
		genasop(op, lv[LVPRIM], lv2[LVPRIM]);
	}
	if (!lv[LVSYM]) {
		cgpopptr();
		if (PCHAR == lv[LVPRIM])
			cgstorib();
		else
			cgstoriw();

	}
	else if (CAUTO == Stcls[lv[LVSYM]]) {
		if (PCHAR == lv[LVPRIM])
			cgstorlb(Vals[lv[LVSYM]]);
		else
			cgstorlw(Vals[lv[LVSYM]]);
	}
	else if (CLSTATC == Stcls[lv[LVSYM]]) {
		if (PCHAR == lv[LVPRIM])
			cgstorsb(Vals[lv[LVSYM]]);
		else
			cgstorsw(Vals[lv[LVSYM]]);
	}
	else {
		if (PCHAR == lv[LVPRIM])
			cgstorgb(gsym(Names[lv[LVSYM]]));
		else
			cgstorgw(gsym(Names[lv[LVSYM]]));
	}
}

/* rvalue computation */

void rvalue(int *lv) {
	if (NULL == lv) return;
	gentext();
	if (!lv[LVSYM]) {
		genind(lv[LVPRIM]);
	}
	else if (CAUTO == Stcls[lv[LVSYM]]) {
		spill();
		if (PCHAR == lv[LVPRIM]) {
			cgclear();
			cgldlb(Vals[lv[LVSYM]]);
		}
		else {
			cgldlw(Vals[lv[LVSYM]]);
		}
	}
	else if (CLSTATC == Stcls[lv[LVSYM]]) {
		spill();
		if (PCHAR == lv[LVPRIM]) {
			cgclear();
			cgldsb(Vals[lv[LVSYM]]);
		}
		else
			cgldsw(Vals[lv[LVSYM]]);
	}
	else {
		spill();
		if (PCHAR == lv[LVPRIM]) {
			cgclear();
			cgldgb(gsym(Names[lv[LVSYM]]));
		}
		else
			cgldgw(gsym(Names[lv[LVSYM]]));
	}
	load();
}
