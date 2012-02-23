MODNAME=lophilo

obj-m += $(MODNAME).o
lophilo-objs = module.o mmap.o data.o netlink.o timer.o

all: observer kernel

kernel:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	-sudo rmmod $(MODNAME)
	sudo insmod $(MODNAME).ko

observer: observer.c Makefile
	gcc -g observer.c -lnl-genl -lnl -o observer
