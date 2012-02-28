CROSS_COMPILE=arm-linux-gnueabi-

test.bin: test.c test.ld startup.s
	$(CROSS_COMPILE)as -mcpu=arm926ej-s -g startup.s -o startup.o
	$(CROSS_COMPILE)gcc -c -mcpu=arm926ej-s -g test.c -o test.o
	$(CROSS_COMPILE)ld -T test.ld test.o startup.o -o test.elf
	$(CROSS_COMPILE)objcopy -O binary test.elf test.bin

run:
	qemu-system-arm -M versatilepb -m 128M -nographic -kernel test.bin

debugging:
	qemu-system-arm -M versatilepb -m 128M -nographic -s -S -kernel test.bin

debugger:
	arm-linux-gnueabi-gdb 

clean:
	rm -f test.elf test.bin *.o
