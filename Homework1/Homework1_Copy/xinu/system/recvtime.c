/* recvtime.c - recvtime */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  recvtime  -  Wait specified time to receive a message and return
 *------------------------------------------------------------------------
 */
umsg32	recvtime(
	  int32		maxwait		/* Ticks to wait before timeout */
        )
{	
	/*------ Timer Code with H/W support */
	
	uint32 		high = 0, low = 0;
	uint64 		start = 0, end = 0;
	asm volatile ( 	"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
	start = ( ((uint64)high << 32) | low);
	
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent	*prptr;		/* Tbl entry of current process	*/
	umsg32	msg;			/* Message to return		*/

	if (maxwait < 0) {
		return SYSERR;
	}
	mask = disable();

	/* Schedule wakeup and place process in timed-receive state */

	prptr = &proctab[currpid];
	if (prptr->prhasmsg == FALSE) {	/* Delay if no message waiting	*/
		if (insertd(currpid,sleepq,maxwait) == SYSERR) {
			restore(mask);
			return SYSERR;
		}
	
		prptr->prstate = PR_RECTIM;
		
		prptr->prSystemcallArray[prptr->prSystemcallCount] = 5;
		prptr->prSystemcallCount++;
		resched();
	}

	/* Either message arrived or timer expired */

	if (prptr->prhasmsg) {
		msg = prptr->prmsg;	/* Retrieve message		*/
		prptr->prhasmsg = FALSE;/* Reset message indicator	*/
	} else {
		msg = TIMEOUT;
	}
	restore(mask);
	
	asm volatile ( 	"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
	end = ( ((uint64)high << 32) | low);
	prptr-> prRecvtTime = end - start;
	return msg;
}
