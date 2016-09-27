# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y

# Add your debugging flag (or not) to EXTRA_FLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

EXTRA_FLAGS += $(DEBFLAGS) -I$(LDDINCDIR) -I$(LIB) -I../../include/linux

ifneq ($(KERNELRELEASE),)
# call from kernel build system

obj-m	:= kthread_q6.o

else

KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINCDIR=$(PWD)/../include modules LIB=/usr/lib/gcc/x86_64-linux-gnu/4.8

endif

kthreadSpawn: kthread_q6.c
	$(CC) kthread_q6.c -o kthread_q6 

clean:
	rm -rf *.o kthread_q6*~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

depend .depend dep:
	$(CC) $(EXTRA_FLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
