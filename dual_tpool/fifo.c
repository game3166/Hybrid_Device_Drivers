#include "fifo.h"


/*------------------------------------------------------------------------
 * Function: enqueue()
 * Descrip : Adds an element to the tail of the queue
 *------------------------------------------------------------------------
 */
void enqueue(struct node** head,struct node** tail,int tid,void* (*procedure)(void* arg), void* arg_p, sem_t* s, pthread_mutex_t* enqueue_lock)
{
	struct node* temp;
	pthread_mutex_lock(enqueue_lock);
	temp= (struct node*) malloc (sizeof (struct node));
	temp->tid=tid;
	temp->next=NULL;
	(temp->job).procedure=procedure;
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
	sem_post(s);
	printf("Enqueuing task from thread %d sem posted\r\n", temp->tid);
	pthread_mutex_unlock(enqueue_lock);
	
}	

/*------------------------------------------------------------------------
 * Function: dequeue()
 * Descrip : Pops an element from the head of the queue and returns its node
 *------------------------------------------------------------------------
 */

struct node* dequeue(struct node** head,struct node** tail, int tid, pthread_mutex_t* lock)
{
	struct node* temp;
		
	pthread_mutex_lock(lock);
	temp = *head;

	if (*head==NULL)
	{
		*head=*tail=NULL;
		printf("Nothing to Dequeue\r\n");
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
	

	pthread_mutex_unlock(lock);
	return (temp);
	
}
