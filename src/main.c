/*
 *	NMH's Simple C Compiler, 2011,2012
 *	Main program
 */

#include "defs.h"
#define _extern
 #include "data.h"
#undef _extern
#include "decl.h"

static void init(void) {
	Line = 1;
	Putback = '\n';
	Rejected = -1;
	Errors = 0;
	Mp = 0;
	Expandmac = 1;
	Syntoken = 0;
	Isp = 0;
	Inclev = 0;
	Globs = 0;
	Locs = NSYMBOLS;
	Nbot = 0;
	Ntop = POOLSIZE;
	Bsp = 0;
	Csp = 0;
	addglob("", 0, 0, 0, 0, 0, NULL, 0);
	addglob("__SUBC__", 0, TMACRO, 0, 0, 0, globname(""), 0);
	Infile = stdin;
	File = "(stdin)";
	Basefile = NULL;
	Outfile = stdout;
}

static void cmderror(char *s, char *a) {
	fprintf(stderr, "scc: ");
	fprintf(stderr, s, a);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

static int filetype(char *file) {
	int	k;

	k = strlen(file);
	if ('.' == file[k-2]) return file[k-1];
	return 0;
}

static void defarg(char *s) {
	char	*p;

	if (NULL == s) return;
	if ((p = strchr(s, '=')) != NULL)
		*p++ = 0;
	else
		p = "";
	addglob(s, 0, TMACRO, 0, 0, 0, globname(p), 0);
	if (*p) *--p = '=';
}

static void stats(void) {
	printf(	"Memory usage: "
		"Symbols: %5d/%5d, "
		"Name pool: %5d/%5d\n",
		Globs, NSYMBOLS,
		Nbot, POOLSIZE);
}

static void compile(char *file, char *def) {
	char	*ofile;

	init();
	defarg(def);
	if (file) {
		ofile = newfilename(file, 's');
		if ((Infile = fopen(file, "r")) == NULL)
			cmderror("no such file: %s", file);
		Basefile = File = file;
		if (!O_testonly) {
			if ((Outfile = fopen(ofile, "r")) != NULL)
				cmderror("will not overwrite file: %s",
					ofile);
			if ((Outfile = fopen(ofile, "w")) == NULL)
				cmderror("cannot create file: %s", ofile);
		}
	}
	if (O_testonly) Outfile = NULL;
	if (O_verbose) {
		if (O_testonly)
			printf("testing %s\n", file);
		else
			printf("compiling %s\n", file);
	}
	genprelude();
	Token = scan();
	while (XEOF != Token)
		top();
	genpostlude();
	if (file) {
		fclose(Infile);
		if (Outfile) fclose(Outfile);
	}
	if (O_debug & D_GSYM) dumpsyms("GLOBALS", "", 1, Globs);
	if (O_debug & D_STAT) stats();
}

static void collect(char *file) {
	if (O_componly || O_asmonly) return;
	if (Nf >= MAXFILES)
		cmderror("too many input files", NULL);
	Files[Nf++] = file;
}

static void assemble(char *file, int delete) {
	char	*ofile;
	char	cmd[TEXTLEN+1];

	file = newfilename(file, 's');
	collect(ofile = newfilename(file, 'o'));
	if (strlen(file) + strlen(ofile) + strlen(ASCMD) >= TEXTLEN)
		cmderror("assembler command too long", NULL);
	sprintf(cmd, ASCMD, ofile, file);
	if (O_verbose > 1) printf("%s\n", cmd);
	if (system(cmd))
		cmderror("assembler invocation failed", NULL);
	if (delete) remove(file);
}

static int concat(int k, char *buf, char *s) {
	int	n;

	n = strlen(s);
	if (k + n + 2 >= TEXTLEN)
		cmderror("linker command too long", buf);
	strcat(buf, " ");
	strcat(buf, s);
	return k + n + 1;
}

static void link(void) {
	int	i, k;
	char	cmd[TEXTLEN+1], *msg;
	char	cmd2[TEXTLEN+1];

	if (strlen(O_outfile) + strlen(LDCMD) + strlen(SCCDIR)*2 >= TEXTLEN)
		cmderror(msg, NULL);
	sprintf(cmd, LDCMD, O_outfile, SCCDIR);
	k = strlen(cmd);
	for (i=0; i<Nf; i++)
		k = concat(k, cmd, Files[i]);
	concat(k, cmd, SCCLIBC);
	concat(k, cmd, SYSLIBC);
	sprintf(cmd2, cmd, SCCDIR);
	if (O_verbose > 1) printf("%s\n", cmd2);
	if (system(cmd2))
		cmderror("linker invocation failed", NULL);
}

static void usage(void) {
	printf("Usage: scc [-h] [-ctvS] [-d opt] [-o file] [-D macro[=text]]"
		" file [...]\n");
}

static void longusage(void) {
	printf("\n");
	usage();
	printf(	"\n"
		"-c       compile only, do not link\n"
		"-d opt   activate debug option OPT\n"
		"-o file  write linker output to FILE\n"
		"-t       test only, generate no code\n"
		"-v       verbose, more v's = more verbose\n"
		"-D m=v   define macro M with optional value V\n"
		"-S       compile to assembly language\n"
		"\n" );
}

static char *nextarg(int argc, char *argv[], int *pi, int *pj) {
	char	*s;

	if (argv[*pi][*pj+1] || *pi >= argc-1) {
		usage();
		exit(EXIT_FAILURE);
	}
	s = argv[++*pi];
	*pj = strlen(s)-1;
	return s;
}

static int dbgopt(int argc, char *argv[], int *pi, int *pj) {
	char	*s;

	s = nextarg(argc, argv, pi, pj);
	if (!strcmp(s, "lsym")) return D_LSYM;
	if (!strcmp(s, "gsym")) return D_GSYM;
	if (!strcmp(s, "stat")) return D_STAT;
	printf(	"\n"
		"scc: valid -d options are: \n\n"
		"lsym - dump local symbol tables\n"
		"gsym - dump global symbol table\n"
		"stat - print usage statistics\n"
		"\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	int	i, j;
	char	*def;

	def = NULL;
	O_verbose = 0;
	O_componly = 0;
	O_asmonly = 0;
	O_testonly = 0;
	O_outfile = "a.out";
	for (i=1; i<argc; i++) {
		if (*argv[i] != '-') break;
		if (!strcmp(argv[i], "-")) {
			compile(NULL, def);
			exit(Errors? EXIT_FAILURE: EXIT_SUCCESS);
		}
		for (j=1; argv[i][j]; j++) {
			switch (argv[i][j]) {
			case 'c':
				O_componly = 1;
				break;
			case 'd':
				O_debug |= dbgopt(argc, argv, &i, &j);
				O_testonly = 1;
				break;
			case 'h':
				longusage();
				exit(EXIT_SUCCESS);
			case 'o':
				O_outfile = nextarg(argc, argv, &i, &j);
				break;
			case 't':
				O_testonly = 1;
				break;
			case 'v':
				O_verbose++;
				break;
			case 'D':
				if (def) cmderror("too many -D's", NULL);
				def = nextarg(argc, argv, &i, &j);
				break;
			case 'S':
				O_componly = O_asmonly = 1;
				break;
			default:
				usage();
				exit(EXIT_FAILURE);
			}
		}
	}
	if (i >= argc) {
		usage();
		exit(EXIT_FAILURE);
	}
	Nf = 0;
	while (i < argc) {
		if (filetype(argv[i]) == 'c') {
			compile(argv[i], def);
			if (Errors && !O_testonly)
				cmderror("compilation stopped", NULL);
			if (!O_asmonly && !O_testonly)
				assemble(argv[i], 1);
			i++;
		}
		else if (filetype(argv[i]) == 's') {
			if (!O_testonly) assemble(argv[i++], 0);
		}
		else {
			collect(argv[i++]);
		}
	}
	if (!O_componly && !O_testonly) link();
	return EXIT_SUCCESS;
}
