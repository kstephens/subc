#
#	NMH's Simple C Compiler, 2012
#	C runtime module for FreeBSD/x86-64
#

# FreeBSD voodoo stuff

	.section .note.ABI-tag,"a",@note
	.align	4
abitag: .long	8, 4, 1
	.string	"FreeBSD"
	.long	802000
	.p2align 2
	.data
	.globl	__progname
	.globl	environ
environ:
	.quad	0
__progname:
	.quad   0

# End of voodoo stuff

	.data
	.globl	Cenviron
Cenviron:
	.quad	0

	.text
	.globl	_start
_start:	pushq	%rdi
	call	C_init
	popq	%rdi
	leaq	8(%rdi),%rsi	# argv
	movq	(%rdi),%rcx	# argc
	movq	%rcx,%rax	# environ = &argv[argc+1]
	incq	%rax
	shlq	$3,%rax
	addq	%rsi,%rax
	movq	%rax,Cenviron
	pushq	%rcx
	pushq	%rsi
	pushq	$2		# __argc
	call	Cmain
	addq	$24,%rsp
	pushq	%rax
	pushq	$1
x:	call	Cexit
	xorq	%rbx,%rbx
	divq	%rbx
	jmp	x

# internal switch(expr) routine
# %rsi = switch table, %rax = expr

	.globl	switch
switch:	pushq	%rsi
	movq	%rdx,%rsi
	movq	%rax,%rbx
	cld
	lodsq
	movq	%rax,%rcx
next:	lodsq
	movq	%rax,%rdx
	lodsq
	cmpq	%rdx,%rbx
	jnz	no
	popq	%rsi
	jmp	*%rax
no:	loop	next
	lodsq
	popq	%rsi
	jmp	*%rax

# int setjmp(jmp_buf env);

	.globl	Csetjmp
Csetjmp:
	movq	16(%rsp),%rdx
	movq	%rsp,(%rdx)
	movq	(%rsp),%rax
	movq	%rax,8(%rdx)
	xorq	%rax,%rax
	ret

# void longjmp(jmp_buf env, int v);

	.globl	Clongjmp
Clongjmp:
	movq	16(%rsp),%rax
	movq	24(%rsp),%rdx
	movq	(%rdx),%rsp
	movq	8(%rdx),%rdx
	jmp	*%rdx

# int _exit(int rc);

	.globl	C_exit
C_exit:	movq	16(%rsp),%rdi
	call	_exit
	ret

# int _sbrk(int size);

	.globl	C_sbrk
C_sbrk:	movq	16(%rsp),%rdi
	xorq	%rax,%rax
	call	sbrk
	ret

# int _write(int fd, char *buf, int len);

	.globl	C_write
C_write:
	movq	16(%rsp),%rdx
	movq	24(%rsp),%rsi
	movq	32(%rsp),%rdi
	xorq	%rax,%rax
	call	write
	ret

# int _read(int fd, char *buf, int len);

	.globl	C_read
C_read:	movq	16(%rsp),%rdx
	movq	24(%rsp),%rsi
	movq	32(%rsp),%rdi
	xorq	%rax,%rax
	call	read
	ret

# int _lseek(int fd, int pos, int how);

	.globl	C_lseek
C_lseek:
	movq	16(%rsp),%rdx
	movq	24(%rsp),%rsi
	movq	32(%rsp),%rdi
	xorq	%rax,%rax
	call	lseek
	ret

# int _creat(char *path, int mode);

	.globl	C_creat
C_creat:
	movq	16(%rsp),%rsi
	movq	24(%rsp),%rdi
	xorq	%rax,%rax
	call	creat
	ret

# int _open(char *path, int flags);

	.globl	C_open
C_open: movq	16(%rsp),%rsi
	movq	24(%rsp),%rdi
	xorq	%rax,%rax
	call	open
	ret

# int _close(int fd);

	.globl	C_close
C_close:
	movq	16(%rsp),%rdi
	xorq	%rax,%rax
	call	close
	ret

# int _unlink(char *path);

	.globl	C_unlink
C_unlink:
	movq	16(%rsp),%rdi
	xorq	%rax,%rax
	call	unlink
	ret

# int _rename(char *old, char *new);

	.globl	C_rename
C_rename:
	movq	16(%rsp),%rsi
	movq	24(%rsp),%rdi
	xorq	%rax,%rax
	call	rename
	ret

# int _fork(void);

	.globl	C_fork
C_fork:	call	fork
	ret

# int _wait(int *rc);

	.data
w:	.long	0
	.text
	.globl	C_wait
C_wait:	movq	$w,%rdi
	xorq	%rax,%rax
	call	wait
	movl	w,%eax
	cdq
	movq	16(%rsp),%rdx
	movq	%rax,(%rdx)
	ret

# int _execve(char *path, char *argv[], char *envp[]);

	.globl	C_execve
C_execve:
	movq	16(%rsp),%rdx
	movq	24(%rsp),%rsi
	movq	32(%rsp),%rdi
	xorq	%rax,%rax
	call	execve
	ret

# int _time(void);

	.globl	C_time
C_time:	xorq	%rdi,%rdi
	xorq	%rax,%rax
	call	time
	ret

# int raise(int sig);

	.globl	Craise
Craise:
	xorq	%rax,%rax
	call	getpid
	movq	%rax,%rdi
	movq	16(%rsp),%rsi
	xorq	%rax,%rax
	call	kill
	ret

# int signal(int sig, int (*fn)());

	.globl	Csignal
Csignal:
	movq	16(%rsp),%rsi
	movq	24(%rsp),%rdi
	xorq	%rax,%rax
	call	signal
	ret
