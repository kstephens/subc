#
#	NMH's Simple C Compiler, 2011,2012
#	C runtime module for Linux/386
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
C_exit:	movl	8(%esp),%ebx
	movl	$1,%eax
	int	$0x80
	ret

# int _sbrk(int size);

	.data
curbrk:	.long	0

	.text
	.globl	C_sbrk
C_sbrk:
#	pushl	8(%esp)		# this works, but will link
#	call	sbrk		# against GNU libc, which is a
#	addl	$4,%esp		# notorious source of trouble.
#	ret			#
	cmpl	$0,curbrk
	jnz	sbrk
	xorl	%ebx,%ebx	# get break
	movl	$45,%eax	# brk
	int	$0x80
	movl	%eax,curbrk
sbrk:	cmpl	$0,8(%esp)
	jnz	setbrk
	mov	curbrk,%eax	# size==0, return break
	ret
setbrk:	movl	curbrk,%ebx	# set new break
	addl	8(%esp),%ebx
	movl	$45,%eax	# brk
	int	$0x80
	cmpl	%eax,curbrk	# brk(x)==curbrk -> error
	jnz	sbrkok
	movl	$-1,%eax
	ret
sbrkok:	movl	curbrk,%ebx	# update curr. break
	movl	%eax,curbrk
	movl	%ebx,%eax
	ret

# int _write(int fd, char *buf, int len);

	.globl	C_write
C_write:
	movl	8(%esp),%edx
	movl	12(%esp),%ecx
	movl	16(%esp),%ebx
	movl	$4,%eax
	int	$0x80
	ret

# int _read(int fd, char *buf, int len);

	.globl	C_read
C_read:	movl	8(%esp),%edx
	movl	12(%esp),%ecx
	movl	16(%esp),%ebx
	movl	$3,%eax
	int	$0x80
	ret

# int _lseek(int fd, int pos, int how);

	.globl	C_lseek
C_lseek:
	movl	8(%esp),%edx
	movl	12(%esp),%ecx
	movl	16(%esp),%ebx
	movl	$19,%eax
	int	$0x80
	ret

# int _creat(char *path, int mode);

	.globl	C_creat
C_creat:
	movl	8(%esp),%ecx
	movl	12(%esp),%ebx
	movl	$8,%eax
	int	$0x80
	ret

# int _open(char *path, int flags);

	.globl	C_open
C_open:	movl	8(%esp),%ecx
	movl	12(%esp),%ebx
	movl	$5,%eax
	int	$0x80
	ret

# int _close(int fd);

	.globl	C_close
C_close:
	movl	8(%esp),%ebx
	movl	$6,%eax
	int	$0x80
	ret

# int _unlink(char *path);

	.globl	C_unlink
C_unlink:
	movl	8(%esp),%ebx
	movl	$10,%eax
	int	$0x80
	ret

# int _rename(char *old, char *new);

	.globl	C_rename
C_rename:
	movl	8(%esp),%ecx
	movl	12(%esp),%ebx
	mov	$38,%eax
	int	$0x80
	ret

# int _fork(void);

	.globl	C_fork
C_fork:	movl	$2,%eax
	int	$0x80
	ret

# int _wait(int *rc);

	.globl	C_wait
C_wait:	movl	$-1,%ebx
	movl	8(%esp),%ecx
	xorl	%edx,%edx
	movl	$7,%eax
	int	$0x80
	ret

# int _execve(char *path, char *argv[], char *envp[]);

	.globl	C_execve
C_execve:
	movl	8(%esp),%edx
	movl	12(%esp),%ecx
	movl	16(%esp),%ebx
	movl	$11,%eax
	int	$0x80
	ret

# int _time(void);

	.globl	C_time
C_time:	xorl	%ebx,%ebx
	movl	$13,%eax
	int	$0x80
	ret

# int raise(int sig);

	.globl	Craise
Craise:
	movl	$20,%eax
	int	$0x80
	movl	%eax,%ebx
	movl	8(%esp),%ecx
	movl	$37,%eax
	int	$0x80
# int signal(int sig, int (*fn)());

	.globl	Csignal
Csignal:
	movl	8(%esp),%ecx
	movl	12(%esp),%ebx
	movl	$48,%eax
	int	$0x80
	ret

