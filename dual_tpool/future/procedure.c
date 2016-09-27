#include "procedure.h"


/*------------------------------------------------------------------------
 * Function: task1()
 * Descrip : Dummy task simulating exported functions
 *------------------------------------------------------------------------
 */
void task1()
{
	sleep(1);
	printf("Thread #%u working on task1\n", (int)pthread_self());
	sleep(5);
	
	pthread_mutex_lock(&m[0]);
	pthread_cond_signal(&c[0]);
	pthread_mutex_unlock(&m[0]);
	
}


/*------------------------------------------------------------------------
 * Function: task2()
 * Descrip : Dummy task simulating exported functions
 *------------------------------------------------------------------------
 */
void task2()
{
	sleep(10);
	printf("Thread #%u working on task2\n", (int)pthread_self());
	
}
