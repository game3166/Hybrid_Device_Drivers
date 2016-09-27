#include "fifo.h"


/*------------------------------------------------------------------------
 * Function: enqueue()
 * Descrip : Adds an element to the tail of the queue
 *------------------------------------------------------------------------
 */
void enqueue(struct node** head,struct node** tail,int tid,void* (*procedure)(void* arg), void* arg_p, struct semaphore* s, struct mutex* enqueue_lock)
{
	struct node* temp;	
	temp= (struct node*) kmalloc (sizeof (struct node),GFP_KERNEL);
	temp->tid=tid;
	temp->next=NULL;
	(temp->job).procedure=procedure;
	mutex_lock_interruptible(enqueue_lock);
	if(*head==NULL && *tail==NULL)
	{
		*head=*tail=temp;
		//printf("First element in queue\r\n");
	}
	else
	{
		(*tail)->next=temp;
		*tail=temp;
	}		
	up(s); //sem post
	printk(KERN_INFO "Enqueuing task from thread %d sem posted\r\n", temp->tid);
	mutex_unlock(enqueue_lock);
	
}	

/*------------------------------------------------------------------------
 * Function: dequeue()
 * Descrip : Pops an element from the head of the queue and returns its node
 *------------------------------------------------------------------------
 */

struct node* dequeue(struct node** head,struct node** tail, int tid, struct mutex* lock)
{
	struct node* temp;
		
	mutex_lock_interruptible(lock);
	temp = *head;

	if (*head==NULL)
	{
		*head=*tail=NULL;
		printk(KERN_INFO"Nothing to Dequeue\r\n");
	}
	else
	{
		if ( ( (*head)->next) !=NULL)
		{
			*head=(*head)->next;		
		}
		else
		{
			*head=(*head)->next;
			*tail=NULL;
		}
	}
	

	mutex_unlock(lock);
	return (temp);
	
}
