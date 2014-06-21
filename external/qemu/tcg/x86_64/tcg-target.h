
#define TCG_TARGET_X86_64 1

#define TCG_TARGET_REG_BITS 64
//#define TCG_TARGET_WORDS_BIGENDIAN

#define TCG_TARGET_NB_REGS 16

enum {
    TCG_REG_RAX = 0,
    TCG_REG_RCX,
    TCG_REG_RDX,
    TCG_REG_RBX,
    TCG_REG_RSP,
    TCG_REG_RBP,
    TCG_REG_RSI,
    TCG_REG_RDI,
    TCG_REG_R8,
    TCG_REG_R9,
    TCG_REG_R10,
    TCG_REG_R11,
    TCG_REG_R12,
    TCG_REG_R13,
    TCG_REG_R14,
    TCG_REG_R15,
};

#define TCG_CT_CONST_S32 0x100
#define TCG_CT_CONST_U32 0x200

/* used for function call generation */
#define TCG_REG_CALL_STACK TCG_REG_RSP 
#define TCG_TARGET_STACK_ALIGN 16
#define TCG_TARGET_CALL_STACK_OFFSET 0

/* optional instructions */
#define TCG_TARGET_HAS_bswap16_i32
#define TCG_TARGET_HAS_bswap16_i64
#define TCG_TARGET_HAS_bswap32_i32
#define TCG_TARGET_HAS_bswap32_i64
#define TCG_TARGET_HAS_bswap64_i64
#define TCG_TARGET_HAS_neg_i32
#define TCG_TARGET_HAS_neg_i64
#define TCG_TARGET_HAS_not_i32
#define TCG_TARGET_HAS_not_i64
#define TCG_TARGET_HAS_ext8s_i32
#define TCG_TARGET_HAS_ext16s_i32
#define TCG_TARGET_HAS_ext8s_i64
#define TCG_TARGET_HAS_ext16s_i64
#define TCG_TARGET_HAS_ext32s_i64
#define TCG_TARGET_HAS_ext8u_i32
#define TCG_TARGET_HAS_ext16u_i32
#define TCG_TARGET_HAS_ext8u_i64
#define TCG_TARGET_HAS_ext16u_i64
#define TCG_TARGET_HAS_ext32u_i64
#define TCG_TARGET_HAS_rot_i32
#define TCG_TARGET_HAS_rot_i64

// #define TCG_TARGET_HAS_andc_i32
// #define TCG_TARGET_HAS_andc_i64
// #define TCG_TARGET_HAS_orc_i32
// #define TCG_TARGET_HAS_orc_i64

#define TCG_TARGET_HAS_GUEST_BASE

/* Note: must be synced with dyngen-exec.h */
#define TCG_AREG0 TCG_REG_R14
#define TCG_AREG1 TCG_REG_R15
#define TCG_AREG2 TCG_REG_R12

static inline void flush_icache_range(unsigned long start, unsigned long stop)
{
}