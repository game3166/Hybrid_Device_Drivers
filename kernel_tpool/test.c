#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "lpc_ioctl.h"

int main(void)
{
	int fd;
	int ret_value;

	procinfo q;



	char buf[4]={""};
	printf("starting Program\r\n");
	printf("tada\r\n");
	
	fd=open("/dev/syscall", O_RDWR);
	//write(fd,&buf,3);
	//read(fd,&buf,3);
	if(fd<0)
		printf("major issue here\r\n");
	q.pid=5121;
	if( ( ret_value=ioctl(fd,EXEC_LPC,5) ) ==-1)
	{
//		perror("Query failed\r\n");
		printf("ERROR\r\n");
	}
	else
	{
	printf("Return Value is %d\r\n",ret_value);
	//printf("PID is %d\r\nPPID is %d \r\nNumber is %d\r\nStart time is%16lu \r\n",q.pid,q.ppid,q.number,q.start_time);

	} 
	
	close(fd);
	printf("Information from user space\r\n");
	printf("PID is%d\r\nPPID is %d\r\n",getpid(),getppid());

//	perror("perror output:");
	
	return 0;
}

