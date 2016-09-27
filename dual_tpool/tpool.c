#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <string.h>


#include "fifo.h"
#include "lpc_ioctl.h"
#define NUM_THREADS 2

//request queue
sem_t sem_req;
pthread_mutex_t dequeue_req=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t enqueue_req=PTHREAD_MUTEX_INITIALIZER;


//response queue
sem_t sem_res;
pthread_mutex_t dequeue_res=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t enqueue_res=PTHREAD_MUTEX_INITIALIZER;


//wake up producer threads
pthread_mutex_t m[NUM_THREADS];
pthread_cond_t c[NUM_THREADS];
//conditional variable check
int done[NUM_THREADS];

//consumer thread arguments
struct con_arg
{
	int tid;
	double stuff;
	struct node** head;
	struct node** tail;	
	
};

//producer thread arguments
struct prod_arg
{
	int tid;
	double stuff;
	struct node** head;
	struct node** tail;	
	
};
// File descriptor for kernel functions
int kfd;


//function prototypes

void* consumer_thread(void* arg);
void* producer_thread(void* arg);


//RPC functions
void task1();
void task2();
struct job procedures[10]; //array of job pointers to store all jobs
int main()
{
	
//	request queue
	struct node* req_head;//request queue head
	struct node* req_tail;//requeste queue tail
	sem_init(&sem_req,0,0);	//initialize semaphore for respose queue
	req_head=NULL;
	req_tail=NULL;
	
//response queue
		
	struct node* res_head; //response queue head
	struct node* res_tail;//response queue tail
	sem_init(&sem_res,0,0);	//initialize semaphore for response queue
	res_head=NULL;
	res_tail=NULL;
	
//producers

	pthread_t prod_threads[NUM_THREADS]; //producer threads
	struct prod_arg prod_arg_list[NUM_THREADS]; //prodcer thread args
	

//consumers

	pthread_t con_threads[NUM_THREADS]; //consumer threads
	struct con_arg con_arg_list[NUM_THREADS];//consumer thread args
		
//general variables

	int i;//iterator
	int rvalue;//return value
	void* temp;
//LPC requests
	lpcreq lpc_req;

//Populate function pointers
	procedures[0].procedure=(void*)task1;
	procedures[1].procedure=(void*)task2;
	//procedures[2].procedure=(void*)task3;

//open file to kernel driver and 
	
	printf("Opening Device file\r\n");	
	kfd=open("/dev/syscall", O_RDWR);
	if(kfd<0)
		printf("Cannot open device file for kernel driver\r\n");
	printf("Device file successfully open\r\n");
		
	//producer threads
	for (i = 0; i < NUM_THREADS; i++)
	{
		//intialize conditional variables
		pthread_mutex_init(&m[i],NULL);
		pthread_cond_init(&c[i],NULL);	
		done[i]=0;
		
		//thread ID's
		prod_arg_list[i].tid=i;
		//FIFO pointers
		prod_arg_list[i].head=&req_head;
		prod_arg_list[i].tail=&req_tail;
		if( (rvalue=pthread_create(&prod_threads[i], NULL, producer_thread, &prod_arg_list[i])) != 0)
		{
			printf("Pthread creation failed, return code: %d\r\n",rvalue);
			return EXIT_FAILURE;
		
		}		
	
	}
	//printf("producer threads created\r\n");
	
	//sleep(1);
	//create consumer threadsERROR
	for (i = 0; i < NUM_THREADS; i++)
	{
		con_arg_list[i].tid=i;
		con_arg_list[i].head=&req_head;
		con_arg_list[i].tail=&req_tail;
		if( (rvalue=pthread_create(&con_threads[i], NULL, consumer_thread, &con_arg_list[i])) != 0)
		{
			printf("Pthread creation failed, return code: %d\r\n",rvalue);
			return EXIT_FAILURE;
		
		}	
	
	}
	
	//printf("consumer threads created\r\n");
	if( ( rvalue=ioctl(kfd,REQUEST_LPC,temp) ) ==-1)			
	{
		printf("ERROR in IOCTL\r\n");
	}
	
	temp=(void*) malloc(sizeof(char)*2048);
	/*if( ( rvalue=ioctl(kfd,POLL_LPC,temp) ) ==-1)			
	{
		printf("ERROR in IOCTL\r\n");
	}
	printf("Woke up from request YAYYY!!!!\r\n");
	//printf("Dat Received from kernel is %d\r\n",*( (int*)&rvalue ) );
	printf("Kernel : Func Index: %d mesaage: %s\r\n", ((lpcreq*)(temp))->func_index, ((lpcreq*)(temp))->str );*/


	
	for(i=0; i<NUM_THREADS; i++)
	{
		pthread_join(prod_threads[i],NULL);	
	
	}
	
	close(kfd);	
	
	//while(1);

}


/*------------------------------------------------------------------------
 * Function: consumer_thread()
 * Descrip : Dequeus an element from the queue and executes the procedure
 *------------------------------------------------------------------------
 */
void* consumer_thread(void* arg)
{
	int ret_value;	
	struct con_arg* args=(struct con_arg*)arg;
		
	void* (*func_buff)(void* arg);
	void*  arg_buff;
	//arg_buff=NULL;
	struct node* temp;
	char message[1000]={""};
	strcpy(message,"Function 1");
	
	while(1)
	{
		sem_wait(&sem_req);	
		temp=dequeue(args->head,args->tail,args->tid,&dequeue_req);
		//printf("Dequeue done from %d\r\n",temp->tid);
		func_buff=( temp->job ).procedure;
		arg_buff=(void*) (&(temp->tid));		
		//printf("TID1 is %d, TID2 is %d\r\n",temp->tid,*(&(temp->tid)));
		func_buff(arg_buff);		
		printf("function executed tid %d\r\n",temp->tid);
		free(temp);

		if( ( ret_value=ioctl(kfd,EXEC_LPC,&(args->tid) ) ) ==-1)
		{
			printf("ERROR\r\n");
		}
		//else
		//{
		//printf("Return Value is %d\r\n",ret_value);
		//}
		printf("The return value is %d\r\n",args->tid); 
		break;
	}
	

	pthread_exit(NULL);
}
/*------------------------------------------------------------------------
 * Function: producer_thread()
 * Descrip : request a LPC and wakes up on response
 *------------------------------------------------------------------------
 */
void* producer_thread(void* arg)
{
	
	struct prod_arg* args=(struct prod_arg*)arg;
	//printf("Producer tid is %d\r\n",args->tid);
	
	enqueue(args->head,args->tail,args->tid,(void*)task1, NULL,&sem_req,&enqueue_req);	
	
	printf("producer tid %d locked\r\n", args->tid);	
	pthread_mutex_lock(&m[args->tid]);
	while (done==0)
		pthread_cond_wait(&c[args->tid], &m[args->tid]);
	pthread_mutex_unlock(&m[args->tid]);		
	
	
	printf("Producer %d woken up after task completion\r\n",args->tid);	

	pthread_exit(NULL);
}
/*------------------------------------------------------------------------
 * Function: task1()
 * Descrip : Dummy task simulating exported functions
 *------------------------------------------------------------------------
 */
void task1(void* arg)
{
	//sleep(1);
	int tid=0;
	tid=*((int*)(arg));
	printf("Thread #%u working on task with TID %d\n", (int)pthread_self(),tid);
	//sleep(1);
	
	pthread_mutex_lock(&m[tid]);
	pthread_cond_signal(&c[tid]);
	done[tid]=1;
	pthread_mutex_unlock(&m[tid]);
	
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

