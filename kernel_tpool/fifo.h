#ifndef FIFO_H_INCLUDED
#define FIFO_H_INCLUDED

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> 
#include <linux/slab.h> 
#include <linux/fs.h> 
#include <linux/errno.h> 
#include <linux/types.h> 
#include <linux/proc_fs.h>
#include <linux/fcntl.h> 
#include <asm-generic/uaccess.h>
#include <linux/sched.h>
#include <asm/current.h> 
#include <linux/list.h>


//kthread functions
#include <linux/delay.h>
#include <linux/kthread.h>
//#include "lpc_ioctl.h"


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

void enqueue(struct node** head,struct node** tail,int tid,void* (*procedure)(void* arg), void* arg_p, struct semaphore* s, struct mutex* enqueue_lock);
struct node* dequeue(struct node** head,struct node** tail, int tid, struct mutex* lock);



#endif // FIFO_H_INCLUDED
