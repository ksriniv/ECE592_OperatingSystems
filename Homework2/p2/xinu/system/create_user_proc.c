#include <xinu.h>

uint32 USERPROC = 1000;

struct procent *proc;

extern uint32 ctr1000;
extern qid16 user_queue;

void timed_execution (uint32 run_time){
	uint32 current_time = ctr1000;
	proc = &proctab[currpid];
	//kprintf ("USER PROCESS RUNNING: %d", ctr1000);
	while ( proc->prruntime != 0) {
		while(ctr1000 -  current_time <= 1);
		current_time = ctr1000;
		proc-> prruntime--;
	}
	
	kill (currpid);

}

pid32 create_user_proc ( void *funcaddr, uint32 ssize, uint32 run_time, char *name, uint32 nargs, ... ){


	uint32		savsp, *pushsp;
	intmask 	mask;    	/* Interrupt mask		*/
	pid32		pid;		/* Stores new process id	*/
	struct	procent	*prptr;		/* Pointer to proc. table entry */
	int32		i;
	uint32		*a;		/* Points to list of args	*/
	uint32		*saddr;		/* Stack address		*/

	mask = disable();
	if (ssize < MINSTK)
		ssize = MINSTK;
	ssize = (uint32) roundmb(ssize);
	if ( ((pid=newpid()) == SYSERR) ||
	     ((saddr = (uint32 *)getstk(ssize)) == (uint32 *)SYSERR) ) {
		restore(mask);
		return SYSERR;
	}
	prcount++;
	prptr = &proctab[pid];

	/* Initialize process table entry for new process */
	prptr->prstate = PR_SUSP;	/* Initial state is suspended	*/
	prptr->prprio = USERPROC;
	prptr->prstkbase = (char *)saddr;
	prptr->prstklen = ssize;
	prptr->prruntime = run_time;
	prptr->prstarttime = ctr1000;
	prptr->prname[PNMLEN-1] = NULLCH;
	for (i=0 ; i<PNMLEN-1 && (prptr->prname[i]=name[i])!=NULLCH; i++)
		;
	prptr->prsem = -1;
	prptr->prparent = (pid32)getpid();
	prptr->prhasmsg = FALSE;

	/* Set up stdin, stdout, and stderr descriptors for the shell	*/
	prptr->prdesc[0] = CONSOLE;
	prptr->prdesc[1] = CONSOLE;
	prptr->prdesc[2] = CONSOLE;

	/* Initialize stack as if the process was called		*/

	*saddr = STACKMAGIC;
	savsp = (uint32)saddr;

	/* Push arguments */
	a = (uint32 *)(&nargs + 1);	/* Start of args		*/
	a += nargs -1;			/* Last argument		*/
	for ( ; nargs > 0 ; nargs--)	/* Machine dependent; copy args	*/
		*--saddr = *a--;	/* onto created process's stack	*/
	*--saddr = (long)INITRET;	/* Push on return address	*/

	/* The following entries on the stack must match what ctxsw	*/
	/*   expects a saved process state to contain: ret address,	*/
	/*   ebp, interrupt mask, flags, registers, and an old SP	*/

	*--saddr = (long)funcaddr;	/* Make the stack look like it's*/
					/*   half-way through a call to	*/
					/*   ctxsw that "returns" to the*/
					/*   new process		*/
	*--saddr = savsp;		/* This will be register ebp	*/
					/*   for process exit		*/
	savsp = (uint32) saddr;		/* Start of frame for ctxsw	*/
	*--saddr = 0x00000200;		/* New process runs with	*/
					/*   interrupts enabled		*/

	/* Basically, the following emulates an x86 "pushal" instruction*/

	*--saddr = 0;			/* %eax */
	*--saddr = 0;			/* %ecx */
	*--saddr = 0;			/* %edx */
	*--saddr = 0;			/* %ebx */
	*--saddr = 0;			/* %esp; value filled in below	*/
	pushsp = saddr;			/* Remember this location	*/
	*--saddr = savsp;		/* %ebp (while finishing ctxsw)	*/
	*--saddr = 0;			/* %esi */
	*--saddr = 0;			/* %edi */
	*pushsp = (unsigned long) (prptr->prstkptr = (char *)saddr);
	restore(mask);
	
	kprintf ( "P%u - creation :: %u :: %u\n", pid, prptr->prstarttime, prptr->prruntime);
	return pid;



}

void user_process_print ( qid16 q) {
 qid16 head = firstid (user_queue);
 


 while (queuetab[head].qnext != -1){
	
 kprintf (" Curr is QPrev: %d Key is %d, Qnext is %d\n", queuetab[head].qprev, queuetab[head].qkey, queuetab[head].qnext);
 	head = queuetab[head].qnext;
 }

}






