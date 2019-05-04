#include "eagle_setup_ipc.hpp"

#include "xil_mmu.h"
#include "xil_cache.h"
#include "xscugic.h"

extern "C" void Xil_L1DCacheFlush(void);

extern u32 MMUTable;

void eagle_setup_ipc(void) {
    // Original: 0b100110111100010 = 0x04de2
    //         0b10110110111111010 = 0x16DFA

    // Configuration of the Level 1 Page Table
    // =======================================
    // 
    // See Figure 3-5 on p.78 of the Zynq-7000 Technical Reference Manual
    // https://www.xilinx.com/support/documentation/user_guides/ug585-Zynq-7000-TRM.pdf
    //
    // [31:20] x    → base address of section
    // [19]    0    → NS
    // [18]    0    → 1 MiB "sections"
    // [17]    0    → Global
    // [16]    1    → Shareable
    // [15]    0    → Access Permission [2]
    // [14:12] 100  → TEX → Normal memory, non-cacheable
    // [11:10] 11   → Access Permission [1:0] → Full Access
    // [9]     0
    // [8:5]   1111 → Domain
    // [4]     1    → Execute Never
    // [3:2]   00   → CB → non-cacheable
    // [1:0]   10   → 1 MiB "sections"
    
	eagle_SetTlbAttributes(0xFFFF0000, 0b1'0'100'11'0'1111'1'00'10);
}

void eagle_DCacheFlush(void) {
	Xil_L1DCacheFlush();
	//Xil_L2CacheFlush();
}

void eagle_SetTlbAttributes(u32 addr, u32 attrib) {
	u32 *ptr;
	u32 section;

	mtcp(XREG_CP15_INVAL_UTLB_UNLOCKED, 0);
	dsb();

	mtcp(XREG_CP15_INVAL_BRANCH_ARRAY, 0);
	dsb();
	eagle_DCacheFlush();

	section = addr / 0x100000;
	ptr = &MMUTable + section;
	*ptr = (addr & 0xFFF00000) | attrib;
	dsb();
}