/* receive.c - receive */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  receive  -  Wait for a message and return the message to the caller
 *------------------------------------------------------------------------
 */
umsg32	receive(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/
	/*------ Timer Code with H/W support */
	
	uint32 		high = 0, low = 0;
	uint64 		start = 0, end = 0;
	asm volatile ( 	"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
	start = ( ((uint64)high << 32) | low);
	
	mask = disable();
	prptr = &proctab[currpid];
	if (prptr->prhasmsg == FALSE) {
		prptr->prstate = PR_RECV;
		resched();		/* Block until message arrives	*/
	}
	msg = prptr->prmsg;		/* Retrieve message		*/

	prptr->prSystemcallArray[prptr->prSystemcallCount] = 4;
	prptr->prSystemcallCount++;
	prptr->prhasmsg = FALSE;	/* Reset message flag		*/
	restore(mask);
	
	asm volatile ( 	"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
	end = ( ((uint64)high << 32) | low);
	prptr-> prRecvTime = end - start;
	
	return msg;
}
