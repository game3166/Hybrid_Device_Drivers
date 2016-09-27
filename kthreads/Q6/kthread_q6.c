/****************************************************************************/
/* Function: Basic POSIX message queue demo from VxWorks Prog. Guide p. 78  */
/*                                                                          */
/* Sam Siewert - 9/24/97						    */
/*									    */
/*                                                    			    */
/* Modified by: Aniket Lata - 7/8/15                                                                         */
/*                                                                          */
/****************************************************************************/

#include <linux/module.h>                                                                   
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

void thread_init(void);

static struct task_struct *threadA, *threadB;
struct mutex M1, M2;
//sem_t S1, S2;
//struct semaphore *S1, *S2;
unsigned long jtime;
int counter_A = 0, counter_B = 0;

void threadA_function(void *ptr)
{
	while(counter_A < 10)
	{
		mutex_unlock(&M1);
	   	mutex_lock(&M2);
		jtime = jiffies;
	   	printk(KERN_INFO "Time %lu In threadA", jiffies_to_msecs(jtime));
		printk(KERN_INFO "Counter A value: %d\n", counter_A++);
	}
}

void threadB_function(void *ptr)
{	
  	while(counter_B < 10)
	{		
     		
                mutex_lock_interruptible(&M1);
		msleep(1000);
		mutex_unlock(&M2);
		jtime = jiffies;
		//clockTime = jiffies_to_clock_t(jtime);
                printk(KERN_INFO "Time %lu In threadB", jiffies_to_msecs(jtime));
		printk(KERN_INFO "Counter B value: %d\n", counter_B++);
	}
}

void thread_init()
{
  
   // initialize semaphore, only to be used with threads in this process, set value to 1
    //sema_init(&S1, 1);
    //sema_init(&S2, 1);
    mutex_init(&M1);
    mutex_init(&M2);
   // Thread A

   char  thread1[8]="threadA";
   char  thread2[8]="threadB";
   printk(KERN_INFO "Initialize kernel thread A\n");
   threadA = kthread_create(threadA_function,NULL,thread1);
   if((threadA))
       {
       printk(KERN_INFO "threadA\n");
       wake_up_process(threadA);
       }
   printk(KERN_INFO "Initialize kernel thread B\n");
   threadB = kthread_create(threadB_function,NULL,thread2);
   if((threadB))
       {
       printk(KERN_INFO "threadB\n");
       wake_up_process(threadB);
       }
   //return 1;
   
}

void thread_cleanup(void) {
 int ret;
 ret = kthread_stop(threadA);
 if(!ret)
  printk(KERN_INFO "Thread A stopped\n");

 ret = kthread_stop(threadB);
 if(!ret)
  printk(KERN_INFO "Thread B stopped\n");

}

module_init(thread_init);
module_exit(thread_cleanup);
