// Lab X Technologies - Peter McLoone (2011)
// peter.mcloone@labxtechnologies.com
// implement reset command using ICAP

#include <common.h>
#include <command.h>
#include "microblaze_fsl.h"

#define RUNTIME_FPGA_BASE (0x00000000)
#define BOOT_FPGA_BASE (0x00000000)
#define FINISH_FSL_BIT (0x80000000)

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
   // Synchronize command bytes
        putfslx(0x0FFFF, 0, FSL_ATOMIC); // Pad words
        putfslx(0x0FFFF, 0, FSL_ATOMIC);
        putfslx(0x0AA99, 0, FSL_ATOMIC); // SYNC
        putfslx(0x05566, 0, FSL_ATOMIC); // SYNC

        // Write the reconfiguration FPGA offset; the base address of the
        // "run-time" FPGA is #defined as a byte address, but the ICAP needs
        // a 16-bit half-word address, so we shift right by one extra bit.
        putfslx(0x03261, 0, FSL_ATOMIC); // Write GENERAL1
        putfslx(((RUNTIME_FPGA_BASE >> 1) & 0x0FFFF), 0, FSL_ATOMIC); // Multiboot start address[15:0]
        putfslx(0x03281, 0, FSL_ATOMIC); // Write GENERAL2
        putfslx(((RUNTIME_FPGA_BASE >> 17) & 0x0FF), 0, FSL_ATOMIC); // Opcode 0x00 and address[23:16]

        // Write the fallback FPGA offset (this image)
        putfslx(0x032A1, 0, FSL_ATOMIC); // Write GENERAL3
        putfslx((BOOT_FPGA_BASE & 0x0FFFF), 0, FSL_ATOMIC);
        putfslx(0x032C1, 0, FSL_ATOMIC); // Write GENERAL4
        putfslx(((BOOT_FPGA_BASE >> 16) & 0x0FF), 0, FSL_ATOMIC);

        // Write IPROG command
        putfslx(0x030A1, 0, FSL_ATOMIC); // Write CMD
        putfslx(0x0000E, 0, FSL_ATOMIC); // IPROG Command
        putfslx(0x02000, 0, FSL_ATOMIC); // Type 1 NOP

        // Trigger the FSL peripheral to drain the FIFO into the ICAP
        putfslx(FINISH_FSL_BIT, 0, FSL_ATOMIC);
	while(1);
	return 0;
}