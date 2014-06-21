
#if !defined(__DYNGEN_EXEC_H__)
#define __DYNGEN_EXEC_H__

#ifdef __sun__
#define _FILEDEFED
#endif

#include <stddef.h>
#include <stdint.h>

#ifdef __OpenBSD__
#include <sys/types.h>
#endif

/* XXX: This may be wrong for 64-bit ILP32 hosts.  */
typedef void * host_reg_t;

#ifdef CONFIG_BSD
typedef struct __sFILE FILE;
#else
typedef struct FILE FILE;
#endif
extern int fprintf(FILE *, const char *, ...);
extern int fputs(const char *, FILE *);
extern int printf(const char *, ...);

#if defined(__i386__)
#define AREG0 "ebp"
#define AREG1 "ebx"
#define AREG2 "esi"
#elif defined(__x86_64__)
#define AREG0 "r14"
#define AREG1 "r15"
#define AREG2 "r12"
#elif defined(_ARCH_PPC)
#define AREG0 "r27"
#define AREG1 "r24"
#define AREG2 "r25"
#elif defined(__arm__)
#define AREG0 "r7"
#define AREG1 "r4"
#define AREG2 "r5"
#elif defined(__hppa__)
#define AREG0 "r17"
#define AREG1 "r14"
#define AREG2 "r15"
#elif defined(__mips__)
#define AREG0 "fp"
#define AREG1 "s0"
#define AREG2 "s1"
#elif defined(__sparc__)
#ifdef CONFIG_SOLARIS
#define AREG0 "g2"
#define AREG1 "g3"
#define AREG2 "g4"
#else
#ifdef __sparc_v9__
#define AREG0 "g5"
#define AREG1 "g6"
#define AREG2 "g7"
#else
#define AREG0 "g6"
#define AREG1 "g1"
#define AREG2 "g2"
#endif
#endif
#elif defined(__s390__)
#define AREG0 "r10"
#define AREG1 "r7"
#define AREG2 "r8"
#elif defined(__alpha__)
#define AREG0 "$15"
#define AREG1 "$9"
#define AREG2 "$10"
#elif defined(__mc68000)
#define AREG0 "%a5"
#define AREG1 "%a4"
#define AREG2 "%d7"
#elif defined(__ia64__)
#define AREG0 "r7"
#define AREG1 "r4"
#define AREG2 "r5"
#else
#error unsupported CPU
#endif

#define xglue(x, y) x ## y
#define glue(x, y) xglue(x, y)
#define stringify(s)	tostring(s)
#define tostring(s)	#s

#if defined(__s390__) && !defined(__s390x__)
# define GETPC() ((void*)(((unsigned long)__builtin_return_address(0) & 0x7fffffffUL) - 1))
#elif defined(__arm__)
# define GETPC() ((void *)((unsigned long)__builtin_return_address(0) - 2))
#else
# define GETPC() ((void *)((unsigned long)__builtin_return_address(0) - 1))
#endif

#endif /* !defined(__DYNGEN_EXEC_H__) */