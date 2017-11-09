/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	/*------ Timer Code with H/W support */
	uint32 		high = 0, low = 0;
	uint64 		start = 0, end = 0;	
	
	asm volatile ( 	"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
	start = ( ((uint64)high << 32) | low);
	
	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		if (ptold->prprio > firstkey(readylist)) {
			return;
		}

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;

		ptold->prSystemcallArray[ptold->prSystemcallCount] = 3;
		
		ptold->prSystemcallCount++;
		insert(currpid, readylist, ptold->prprio);
		
		asm volatile ( 	"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
		end = ( ((uint64)high << 32) | low);
		ptold-> prReschedTime = end - start;
		
	}

	/* Force context switch to highest priority ready process */

	currpid = dequeue(readylist);
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/

	ptnew->prSystemcallArray[ptnew->prSystemcallCount] = 3;
	ptnew->prSystemcallCount++;
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */
	
	asm volatile ( 	"RDTSC\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t": "=r" (high), "=r" (low));
	end = ( ((uint64)high << 32) | low);
	ptnew-> prReschedTime = end - start;
	
	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
