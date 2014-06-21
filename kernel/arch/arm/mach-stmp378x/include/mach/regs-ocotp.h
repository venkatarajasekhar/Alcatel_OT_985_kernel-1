
#define REGS_OCOTP_BASE	(STMP3XXX_REGS_BASE + 0x2C000)
#define REGS_OCOTP_PHYS	0x8002C000
#define REGS_OCOTP_SIZE	0x2000

#define HW_OCOTP_CTRL		0x0
#define BM_OCOTP_CTRL_BUSY	0x00000100
#define BM_OCOTP_CTRL_ERROR	0x00000200
#define BM_OCOTP_CTRL_RD_BANK_OPEN	0x00001000
#define BM_OCOTP_CTRL_RELOAD_SHADOWS	0x00002000
#define BM_OCOTP_CTRL_WR_UNLOCK	0xFFFF0000
#define BP_OCOTP_CTRL_WR_UNLOCK	16

#define HW_OCOTP_DATA		0x10

#define HW_OCOTP_CUST0		(0x20 + 0 * 0x10)
#define HW_OCOTP_CUST1		(0x20 + 1 * 0x10)
#define HW_OCOTP_CUST2		(0x20 + 2 * 0x10)
#define HW_OCOTP_CUST3		(0x20 + 3 * 0x10)

#define HW_OCOTP_CUSTn		0x20