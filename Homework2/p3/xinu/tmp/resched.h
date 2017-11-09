/* resched.h */

/* Constants and variables related to deferred rescheduling */

#define	DEFER_START	1	/* Start deferred rescehduling		*/
#define	DEFER_STOP	2	/* Stop  deferred rescehduling		*/

/* Structure that collects items related to deferred rescheduling	*/

#define TIME_ALLOTMENT 100
#define PRIORITY_BOOST_PERIOD 1000

#define q1_time  TIME_ALLOTMENT
#define q2_time  (2*TIME_ALLOTMENT)
#define q3_time  (4*TIME_ALLOTMENT)
#define q4_time  (8*TIME_ALLOTMENT)
struct	defer	{
	int32	ndefers;	/* Number of outstanding defers 	*/
	bool8	attempt;	/* Was resched called during the	*/
				/*   deferral period?			*/
};

extern	struct	defer	Defer;
