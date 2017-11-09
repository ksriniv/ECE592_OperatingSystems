/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
unsigned int running_queue = 0;
extern uint32 USERPROC;

void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
        int roundrobin = 0;
	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
	    if (ptold->prprio != USERPROC) {
                if (ptold->prprio > firstkey(readylist)) {
			return;
		}

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		insert(currpid, readylist, ptold->prprio);
	     }

            else {
                if (running_queue == 0)
                    running_queue = 1;
                if ( ptold-> prtimeallotted <= 0 ){
                    switch (ptold->queuevar) {

                        case 1: insert (currpid, userqueue2, ptold->prprio);
                                ptold->prtimeallotted = q2_time;
                                ptold->queuevar++;
                                break;
                        case 2: insert (currpid, userqueue3, ptold->prprio);
                                ptold->prtimeallotted = q3_time;
                                ptold->queuevar++;
                                break;
                        case 3: insert (currpid, userqueue4, ptold->prprio);
                                ptold->prtimeallotted = q4_time;
                                ptold->queuevar++;
                                break;
                        case 4: insert (currpid, userqueue2, ptold->prprio);
                                ptold->prtimeallotted = q4_time;
                                break;
                        default: break;
                    }
                }

                else {
                    switch (running_queue){
                        
                        case 1: if (nonempty(userqueue1)){
                                    roundrobin = 1;
                                }
                                else
                                    return;
                                break;
                        case 2: if (nonempty(userqueue2)){
                                    roundrobin = 2;
                                }
                                else
                                    return;
                                break;
                        case 3: if (nonempty(userqueue3)){
                                    roundrobin = 3;
                                }
                                else
                                    return;
                                break;
                        case 4: if (nonempty(userqueue4)){
                                    roundrobin = 4;
                                }
                                else
                                    return;
                                break;
                         }
                }
            }
        }
	/* Force context switch to highest priority ready process */
        
        if (firstkey(readylist) != 0){
	    currpid = dequeue(readylist);
            running_queue = 0;
        }
        else if (nonempty(userqueue1)){
            if (roundrobin == 1){
                currpid = dequeue(userqueue1);
                insert (currpid, userqueue1, ptold->prprio);
            }
            else{
                
                insert (currpid, userqueue1, ptold->prprio);
                currpid = dequeue(userqueue1);
            }
        }
	else if (nonempty(userqueue2)){
            if (roundrobin == 2){
                currpid = dequeue(userqueue2);
                insert (currpid, userqueue2, ptold->prprio);
            }
            else{

                insert (currpid, userqueue2, ptold->prprio);
                currpid = dequeue(userqueue2);
            }
        }
	else if (nonempty(userqueue3)){
            if (roundrobin == 3){
                currpid = dequeue(userqueue3);
                insert (currpid, userqueue3, ptold->prprio);
            }
            else{

                insert (currpid, userqueue3, ptold->prprio);
                currpid = dequeue(userqueue3);
            }
        }
	else if (nonempty(userqueue4)){
            if (roundrobin == 4){
                currpid = dequeue(userqueue4);
                insert (currpid, userqueue4, ptold->prprio);
            }
            else{
                
                insert (currpid, userqueue4, ptold->prprio);
                currpid = dequeue(userqueue4);
            }
         }
        else{
            currpid = dequeue(readylist);
            running_queue = 0;
        }
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/
        ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */

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
