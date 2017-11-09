#include <xinu.h>
unsigned long *esp; 			// To store stack Pointer
unsigned long limit = 0;
void printprocstk (int prio){
	struct procent *process;	// Pointer to store proctab
	int i = 0;
	/*---- Printing Template ---*/
	kprintf ("Outcome of printprocstk %d \n", prio);
	kprintf ("%3s\t%-18s\t%-8s\t%5s\t%10s\t%18s\t%20s\t%10s\n", "Pid", "Name", "Priority","State","Stack Base", "Stack Size (bytes)","Stack Limit (bytes)", "Stack Ptr");
	kprintf ("%3s\t%-18s\t%-8s\t%5s\t%10s\t%18s\t%20s\t%10s\n", "---","----------","--------","-----","----------","------------------","-------------------","---------");
	for (i = 0; i < NPROC; i++){
		process = &proctab[i];
		if (process-> prstate != PR_FREE){
			if (process-> prprio > prio){
			limit = ((unsigned long)process->prstkbase - process ->prstklen) + sizeof(long);	
	kprintf ("%d\t%-18s\t%d\t\t%d\t0x%08x\t\t%d\t\t\t0x%08x\t", i, process-> prname, process-> prprio, process-> prstate, process-> prstkbase,( process-> prstklen), limit);			//Add code to print out Stack Limit
				if (process-> prstate == PR_CURR){ 	// That is if its the current process use asm else kprint the process->prstkptr value
					asm("movl %%esp, %0\n":"=r"(esp));
					kprintf ("0x%08x\n", esp);
				}
				else
					kprintf ("0x%08x\n", process->prstkptr);
			}
		}
	}
}
