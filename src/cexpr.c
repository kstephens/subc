/*
 *	NMH's Simple C Compiler, 2011,2012
 *	Constant expression parser
 */

#include "defs.h"
#include "data.h"
#include "decl.h"
#include "prec.h"

/*
 * cfactor :=
 *	  INTLIT
 *	| IDENT
 *	| - cfactor
 *	| ~ cfactor
 *	| ( constexpr )
 *
 * cterm :=
 *	  cfactor
 *      | cterm * cprefix
 *      | cterm / cprefix
 *      | cterm % cprefix
 *
 * csum :=
 *	  cterm
 *      | csum + cterm
 *      | csum - cterm
 *
 * cshift :=
 *	  csum
 *      | cshift << csum
 *      | cshift >> csum
 *
 * crelation :=
 *	    cshift
 *        | crelation < cshift
 *        | crelation > cshift
 *        | crelation <= cshift
 *        | crelation >= cshift
 *
 * cequation :=
 *	    crelation
 *        | cequation == crelation
 *        | cequation != crelation
 *
 * cbinand :=
 *	    cequation
 *        | cbinand & cequation
 *
 * cbinxor :=
 *	    cbinand
 *        | cbinxor ^ cbinand
 *
 * cbinor :=
 *	    cbinxor
 *        | cbinor ^ cbinxor
 *
 * constexpr :=
 *	 cbinor
 */

static int constfac(void) {
	int	y, v;

	v = Value;
	if (INTLIT == Token) {
		Token = scan();
		return v;
	}
	if (MINUS == Token) {
		Token = scan();
		return -constfac();
	}
	if (TILDE == Token) {
		Token = scan();
		return ~constfac();
	}
	if (LPAREN == Token) {
		Token = scan();
		v = constexpr();
		rparen();
		return v;
	}
	if (Token == IDENT) {
		y = findsym(Text);
		if (!y || Types[y] != TCONSTANT)
			error("not a constant: %s", Text);
		Token = scan();
		return y? Vals[y]: 0;
	}
	else {
		error("constant expression expected at: %s", Text);
		Token = scan();
		return 1;
	}
}

static int constop(int op, int v1, int v2) {
	if ((SLASH == op || MOD == op) && 0 == v2) {
		error("constant divide by zero", NULL);
		return 0;
	}
	switch (op) {
	case SLASH:	v1 /= v2; break;
	case STAR:	v1 *= v2; break;
	case MOD:	v1 %= v2; break;
	case PLUS:	v1 += v2; break;
	case MINUS:	v1 -= v2; break;
	case LSHIFT:	v1 <<= v2; break;
	case RSHIFT:	v1 >>= v2; break;
	case GREATER:	v1 = v1 > v2; break;
	case GTEQ:	v1 = v1 >= v2; break;
	case LESS:	v1 = v1 < v2; break;
	case LTEQ:	v1 = v1 <= v2; break;
	case EQUAL:	v1 = v1 == v2; break;
	case NOTEQ:	v1 = v1 != v2; break;
	case AMPER:	v1 &= v2; break;
	case CARET:	v1 ^= v2; break;
	case PIPE:	v1 |= v2; break;
	}
	return v1;
}

int constexpr(void) {
	int	v, ops[9], vals[10], sp = 0;

	vals[0] = constfac();
	while (SLASH == Token || STAR == Token || MOD == Token ||
		PLUS == Token || MINUS == Token || LSHIFT == Token ||
		RSHIFT == Token || GREATER == Token || GTEQ == Token ||
		LESS == Token || LTEQ == Token || EQUAL == Token ||
		NOTEQ == Token || AMPER == Token || CARET == Token ||
		PIPE == Token
	) {
		while (sp > 0 && Prec[Token] <= Prec[ops[sp-1]]) {
			v = constop(ops[sp-1], vals[sp-1], vals[sp]);
			vals[--sp] = v;
		}
		ops[sp++] = Token;
		Token = scan();
		vals[sp] = constfac();
	}
	while (sp > 0) {
		v = constop(ops[sp-1], vals[sp-1], vals[sp]);
		vals[--sp] = v;
	}
	return vals[0];
}
