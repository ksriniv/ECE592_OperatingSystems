/* resched:.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;
extern uint32 ctr1000;
/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
extern uint32 USERPROC;
extern qid16 user_queue;

void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */
//	kprintf (" Mostly this is resched-> firstkey: %u \n", firstkey(readylist));

	ptold = &proctab[currpid];
	/*-------- If NULL is the only element in the Readylist,
	 * then schedule User Process, else schedule the system process*/
	if (ptold->prstate == PR_CURR) { 
		 /* Process remains eligible */
			if (ptold->prprio < USERPROC) {	
				if (ptold->prprio > firstkey(readylist)) {
					//	kprintf ("case 1 \n");
						return;
				}
		
			/* Old process will no longer remain current */
				ptold->prstate = PR_READY;
				insert(currpid, readylist, ptold->prprio);
			}

			else {

				kprintf ("P%d-running::%d-%d\n", currpid, ptold->prstarttime, ctr1000);
				if (ptold->prruntime < lastkey(user_queue) && nonempty(user_queue))
						return;
				//kprintf ("User process added to queue\n");
				ptold->prstate = PR_READY;
				insert(currpid, user_queue, ptold->prruntime);
			}

	}		
	/* Force context switch to highest priority ready process */
	if (firstkey(readylist) != 0 ){
				currpid = dequeue(readylist);
			//	kprintf ("System Process %d \n", currpid );
	}
	else if (nonempty(user_queue)){
				currpid = dequeue_user(user_queue);
			//	kprintf ("User process dequeued \n");
	}
	else{
			//	kprintf ("CurrPid : %d and Nonempty: %d\n", currpid, nonempty(user_queue));
				currpid = dequeue(readylist);	
	}
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	/* Old process returns here when resumed */
	
	preempt = QUANTUM;		/* Reset time slice for process	*/
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
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
