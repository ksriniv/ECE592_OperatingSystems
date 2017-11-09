/*--- xsh_reset.c - xsh_reset -----*/

#include <xinu.h>

#define KEYB_CTLR_IO_ADDR 0x64
#define KEYB_RESET_CMD 0xFE

/*------------------------------------------------------------------------
 *  * xsh_reset - reset the CPU by asking the keyboard controller (8042) to
 *   * toggle the reset line to the x86 CPU
 *    *------------------------------------------------------------------------
 *     */
shellcmd xsh_reset(int nargs, char *args[]){
		printf("Going to reboot now...\n");
			/* Request the keyboard controller to assert the CPU's reset pin */
			outb(KEYB_CTLR_IO_ADDR, KEYB_RESET_CMD);
				return 0;
}
