#ifndef FIFO_H_INCLUDED
#define FIFO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


//contains function pointer and its arguments
struct job
{
	void* (*procedure)(void* arg);
	void* arg;
};

//FIFO node
struct node 
{
	int tid;
	struct node* next;
	struct job job;
};

//Function Prototypes

void enqueue(struct node** head,struct node** tail,int tid,void* (*procedure)(void* arg), void* arg_p, sem_t* s, pthread_mutex_t* lock);
struct node* dequeue(struct node** head,struct node** tail, int tid, pthread_mutex_t* lock);



#endif // FIFO_H_INCLUDED
