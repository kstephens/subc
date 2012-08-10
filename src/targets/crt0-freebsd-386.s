#
#	NMH's Simple C Compiler, 2011,2012
#	C runtime module for FreeBSD/386
#

	.data
	.globl	Cenviron
Cenviron:
	.long	0

	.text
	.globl	_start
_start:	call	C_init
	leal	4(%esp),%esi	# argv
	movl	0(%esp),%ecx	# argc
	movl	%ecx,%eax	# environ = &argv[argc+1]
	incl	%eax
	shll	$2,%eax
	addl	%esi,%eax
	movl	%eax,Cenviron
	pushl	%ecx
	pushl	%esi
	pushl	$2		# __argc
	call	Cmain
	addl	$12,%esp
	pushl	%eax
	pushl	$1
x:	call	Cexit
	xorl	%ebx,%ebx
	divl	%ebx
	jmp	x

# internal switch(expr) routine
# %esi = switch table, %eax = expr

	.globl	switch
switch:	pushl	%esi
	movl	%edx,%esi
	movl	%eax,%ebx
	cld
	lodsl
	movl	%eax,%ecx
next:	lodsl
	movl	%eax,%edx
	lodsl
	cmpl	%edx,%ebx
	jnz	no
	popl	%esi
	jmp	*%eax
no:	loop	next
	lodsl
	popl	%esi
	jmp	*%eax

# int setjmp(jmp_buf env);

	.globl	Csetjmp
Csetjmp:
	movl	8(%esp),%edx
	movl	%esp,(%edx)
	movl	(%esp),%eax
	movl	%eax,4(%edx)
	xorl	%eax,%eax
	ret

# void longjmp(jmp_buf env, int v);

	.globl	Clongjmp
Clongjmp:
	movl	8(%esp),%eax
	movl	12(%esp),%edx
	movl	(%edx),%esp
	movl	4(%edx),%edx
	jmp	*%edx

# int _exit(int rc);

	.globl	C_exit
C_exit:	pushl	8(%esp)
	call	_exit
	addl	$4,%esp
	ret

# int _sbrk(int size);

	.globl	C_sbrk
C_sbrk:	pushl	8(%esp)
	call	sbrk
	addl	$4,%esp
	ret

# int _write(int fd, char *buf, int len);

	.globl	C_write
C_write:
	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	call	write
	addl	$12,%esp
	ret

# int _read(int fd, char *buf, int len);

	.globl	C_read
C_read:	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	call	read
	addl	$12,%esp
	ret

# int _lseek(int fd, int pos, int how);

	.globl	C_lseek
C_lseek:
	pushl	8(%esp)
	movl	16(%esp),%eax
	cdq
	pushl	%edx		# off_t, high word
	pushl	%eax		# off_t, low word
	pushl	28(%esp)
	call	lseek
	addl	$16,%esp
	ret

# int _creat(char *path, int mode);

	.globl	C_creat
C_creat:
	pushl	8(%esp)
	pushl	16(%esp)
	call	creat
	addl	$8,%esp
	ret

# int _open(char *path, int flags);

	.globl	C_open
C_open:	pushl	8(%esp)
	pushl	16(%esp)
	call	open
	addl	$8,%esp
	ret

# int _close(int fd);

	.globl	C_close
C_close:
	pushl	8(%esp)
	call	close
	addl	$4,%esp
	ret

# int _unlink(char *path);

	.globl	C_unlink
C_unlink:
	pushl	8(%esp)
	call	unlink
	addl	$4,%esp
	ret

# int _rename(char *old, char *new);

	.globl	C_rename
C_rename:
	pushl	8(%esp)
	pushl	16(%esp)
	call	rename
	addl	$8,%esp
	ret

# int _fork(void);

	.globl	C_fork
C_fork:	call	fork
	ret

# int _wait(int *rc);

	.globl	C_wait
C_wait:	pushl	8(%esp)
	call	wait
	addl	$4,%esp
	ret

# int _execve(char *path, char *argv[], char *envp[]);

	.globl	C_execve
C_execve:
	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	call	execve
	addl	$12,%esp
	ret

# int _time(void);

	.globl	C_time
C_time:	pushl	$0
	call	time
	addl	$4,%esp
	ret

# int raise(int sig);

	.globl	Craise
Craise:
	call	getpid
	pushl	8(%esp)
	pushl	%eax
	call	kill
	addl	$8,%esp
	ret

# int signal(int sig, int (*fn)());

	.globl	Csignal
Csignal:
	pushl	8(%esp)
	pushl	16(%esp)
	call	signal
	addl	$8,%esp
	ret
