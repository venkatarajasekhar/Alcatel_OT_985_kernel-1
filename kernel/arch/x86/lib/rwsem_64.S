

#include <linux/linkage.h>
#include <asm/rwlock.h>
#include <asm/alternative-asm.h>
#include <asm/frame.h>
#include <asm/dwarf2.h>

#define save_common_regs \
	pushq %rdi; \
	pushq %rsi; \
	pushq %rcx; \
	pushq %r8; \
	pushq %r9; \
	pushq %r10; \
	pushq %r11

#define restore_common_regs \
	popq %r11; \
	popq %r10; \
	popq %r9; \
	popq %r8; \
	popq %rcx; \
	popq %rsi; \
	popq %rdi

/* Fix up special calling conventions */
ENTRY(call_rwsem_down_read_failed)
	save_common_regs
	pushq %rdx
	movq %rax,%rdi
	call rwsem_down_read_failed
	popq %rdx
	restore_common_regs
	ret
	ENDPROC(call_rwsem_down_read_failed)

ENTRY(call_rwsem_down_write_failed)
	save_common_regs
	movq %rax,%rdi
	call rwsem_down_write_failed
	restore_common_regs
	ret
	ENDPROC(call_rwsem_down_write_failed)

ENTRY(call_rwsem_wake)
	decl %edx	/* do nothing if still outstanding active readers */
	jnz 1f
	save_common_regs
	movq %rax,%rdi
	call rwsem_wake
	restore_common_regs
1:	ret
	ENDPROC(call_rwsem_wake)

/* Fix up special calling conventions */
ENTRY(call_rwsem_downgrade_wake)
	save_common_regs
	pushq %rdx
	movq %rax,%rdi
	call rwsem_downgrade_wake
	popq %rdx
	restore_common_regs
	ret
	ENDPROC(call_rwsem_downgrade_wake)
