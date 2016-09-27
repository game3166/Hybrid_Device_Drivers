#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <string.h>

//multi threading
#include <pthread.h>
#include <semaphore.h>


//user libraries
#include "fifo.h"
#include "lpc_ioctl.h"
#define NUM_THREADS 1

//Global Variables

//request queue
sem_t sem_req;//semaphore for sharing queue
pthread_mutex_t dequeue_req=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t enqueue_req=PTHREAD_MUTEX_INITIALIZER;

//worker thread arguments
struct wor_arg
{
	int utid; //user thread id	
	struct node** head; //head of queue
	struct node** tail;	//tail of queue
	
};

// list of all exposed procedures
struct job procedures[10]; //array of job pointers to store all job

//function prototypes
void* worker_thread(void* arg);
void* kexec_lpc(void* arg);

//RPC functions
void task1(void* arg);
void task2(void* arg);
//Insert structure for all function arguments


//kernel file descriptor
int kfd;

//Main/Leader Thread
int main()
{
	//Request Queue Initalization
	struct node* req_head;//request queue head
	struct node* req_tail;//requeste queue tail
	req_head=NULL;
	req_tail=NULL;
	sem_init(&sem_req,0,0);	//initialize semaphore for request queue	

	//Worker threads
	pthread_t wor_threads[NUM_THREADS]; //worker thread list
	struct wor_arg wor_arg_list[NUM_THREADS];//Worker thread arg list

	//general variable
	int i;	//interator
	int rvalue; //return value
	void* temp; //will be malloc'ed and free'ed as necessary
	//LPC requests
	lpcreq lpc_req;//temporary argument for functions


	//Populate function pointers
	procedures[0].procedure=(void*)task1;
	procedures[1].procedure=(void*)task2;

	//open device file to kernel driver

	kfd=open("/dev/syscall", O_RDWR);
	if(kfd<0)
		printf("Cannot open device file for kernel driver\r\n");

	kexec_lpc("a");

	//Launch Worker Threads
	
	for (i = 0; i < NUM_THREADS; i++)
	{
		wor_arg_list[i].utid=i+1; //user friendly id for each thread
		wor_arg_list[i].head=&req_head; //request queue head
		wor_arg_list[i].tail=&req_tail; //request queue tail
		if( (rvalue=pthread_create(&wor_threads[i], NULL, worker_thread, &wor_arg_list[i])) != 0)
		{
			printf("worker pthread creation failed, return code: %d\r\n",rvalue);
			return EXIT_FAILURE;
		
		}

	}

	
	//Leader Thread code
	temp=(void*) malloc(sizeof(char)*2048);//random large size assigned
	 
	while (1) //make this terminate gracefully on a signal or something
	{
		//block in kernel waiting for a request
		if( ( rvalue=ioctl(kfd,POLL_LPC,temp) ) ==-1)			
		{
			printf("ERROR in IOCTL\r\n");
			return EXIT_FAILURE;
		}
		printf("Kernel : Func Index: %d mesaage: %s\r\n", ((lpcreq*)(temp))->func_index, ((lpcreq*)(temp))->str );
		
		enqueue(&req_head,&req_tail,0,procedures[((lpcreq*)(temp))->func_index].procedure, temp ,&sem_req,&enqueue_req);

	}
	
	//terminate all threads gracefully	
	for(i=0; i<NUM_THREADS; i++)
	{
		pthread_join(wor_threads[i],NULL);	
	
	}
	free(temp);
	close(kfd);//close kernel driver device file
}

	
/*------------------------------------------------------------------------
 * Function: worker_thread()
 * Descrip : Dequeus an element from the queue and executes the procedure
 *------------------------------------------------------------------------
 */
void* worker_thread(void* arg)
{
	int ret_value;	
	struct wor_arg* args=(struct wor_arg*)arg;
		
	void* (*func_buff)(void* arg);
	void*  arg_buff;
	//arg_buff=NULL;
	struct node* temp;
	char message[1000]={""};
	strcpy(message,"Function 1");
	
	while(1)
	{
		sem_wait(&sem_req);	
		printf("Dequeued\r\n");
		temp=dequeue(args->head,args->tail,args->utid,&dequeue_req);
		
		func_buff=( temp->job ).procedure;
		arg_buff=( temp->job ).arg;
		printf("arg is %s\r\n",((lpcreq*)(( temp->job ).arg))->str);
		func_buff(arg_buff);//execute requestd function
		
		printf("function executed tid ");
		free(temp); //free element from the queue

		//Pass return value to the kernel	
		if( ( ret_value=ioctl(kfd,RESPONSE_LPC,temp) ) ==-1)			
		{
			printf("ERROR in IOCTL\r\n");
			exit(1);
		}		

		 
		
	}
	

	pthread_exit(NULL);
}

/*------------------------------------------------------------------------
 * Function: kexec_lpc()
 * Descrip : LPC call to kernel driver
 *------------------------------------------------------------------------
 */
void* kexec_lpc(void* arg)
{
	int temp;
	int ret_value;
	temp=1;
		
	if( ( ret_value=ioctl(kfd,EXEC_LPC,&temp ) ) ==-1)
	{
		printf("ERROR in ioctl: kexec_lpc\r\n");
	}
	printf("The return value from kernel is %d\r\n",temp);

}
/*------------------------------------------------------------------------
 * Function: task1()
 * Descrip : Dummy task simulating exported functions
 *------------------------------------------------------------------------
 */
void task1(void* arg)
{
	printf("In task\r\n");
	printf("Task1: Arg : %s\r\n", ((lpcreq*)(arg))->str);
}

/*------------------------------------------------------------------------
 * Function: task2()
 * Descrip : Dummy task simulating exported functions
 *------------------------------------------------------------------------
 */
void task2(void* arg)
{
	printf("In task\r\n");	
	
	printf("Task2: Arg : %s\r\n", ((lpcreq*)(arg))->str);
	
}

