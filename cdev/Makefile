obj-m += lophilo.o
TARGETDIR=$(HOME)/lophilo-bin
MODULE_SUBDIR=$(TARGETDIR)/lib/modules/$(shell uname -r)

all: lophilo_user lophilo.ko /dev/lophilo

lophilo.ko: lophilo.c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	-sudo rmmod lophilo
	sudo insmod lophilo.ko

lophilo_user: lophilo_user.c
	gcc -g lophilo_user.c -o lophilo_user

/dev/lophilo: lophilo.ko
	sudo rm -f /dev/lophilo
	sudo mknod -m 0666 /dev/lophilo c 126 0

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f lophilo_user

install: lophilo_user lophilo.ko
	mkdir -p $(MODULE_SUBDIR)
	mkdir -p $(TARGETDIR)/bin
	cp lophilo_user $(TARGETDIR)/bin
	cp lophilo.ko $(MODULE_SUBDIR)
	
