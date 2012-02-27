obj-m += lophilo.o

all: user kernel install

kernel: lophilo.c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	-sudo rmmod lophilo
	sudo insmod lophilo.ko

user: lophilo_user.c
	gcc -g lophilo_user.c -o lophilo_user

install: user kernel
	sudo rm -f /dev/lophilo
	sudo mknod -m 0666 /dev/lophilo c 126 0

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f lophilo_user
