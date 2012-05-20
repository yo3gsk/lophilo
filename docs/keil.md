
# Converting GNU project to run on Lophilo as bare ARM

Example uses lwip


## Setup

Copy sample project folder from Atmel.

git init

checkout dependencies in external_libs/

git checkout 

## Keil project creation

Start Keil

Project > New uVision project > create project for Atmel SAM9M10

Right-click project, add groups:

TCP/IP stack: lwip
	
	..\external_libs\lwip\src\api\*.c
	..\external_libs\lwip\src\core\*.c

main: your own project files

	main.c
	emacif.c
	<application>.c

arch: optimizations for local architecture 

	arch\sys_arch.c

at91: at91 files
	..\at91lib\boards\at91sam9m10-ek\*.c
	..\at91lib\drivers\macb\*.c
	..\at91lib\peripherals\*.c (add each subdirectory)

All files in 

Alt-F7 (Options for project) > C/C++

	Define at91sam9m10=1

	Add include path to local project
		..\basic-emac-lwip-project
	
	Add include path to lwip 
		..\external_libs\lwip\src\include
		..\external_libs\lwip\src\include\ipv4
		..\external_libs\lwip\src\include\ipv6
		..\external_libs\lwip\src\include\lwip
	
	Add include path to at91lib
		..\at91lib\peripherals
		..\at91lib
		..\at91lib\boards\at91sam9m10-ek
		..\at91lib\drivers

If you compile now, you'll see many errors such as:

	api_lib.c(149): warning:  #223-D: function "TCPIP_APIMSG" declared implicitly
	api_lib.c(149): error:  #20: identifier "lwip_netconn_do_getaddr" is undefined

Changes:

make sure that lwiopts.h (your custom project settings) has:

	#define LWIP_NETCONN                    0

swap the arguments around in emacif.c:

 return etharp_output(netif, ipaddr, p); 

to

 return etharp_output(netif, p, ipaddr);


This error:

	init.c(112): error:  #35: #error directive: "MEMP_NUM_SYS_TIMEOUT is too low to accomodate all required timeouts"

Change lwiopts.h:MEMP_NUM_SYS_TIMEOUT from 0 to 5

	init.c(146): error:  #35: #error directive: "If you want to use DHCP ARP checking, you have to define LWIP_DHCP=1 and LWIP_ARP=1 in your lwipopts.h"

Change lwiopts.h

	LWIP_DHCP from 0 to 1
	LWIP_UDP from 0 to 1

This error:

	init.c(221): error:  #35: #error directive: "MEMP_NUM_TCPIP_MSG option is deprecated. Remove it from your lwipopts.h."

Remove MEMP_NUM_TCPIP_MSG, MEMP_NUM_API_MSG

	init.c(265): error:  #35: #error directive: "lwip_sanity_check: WARNING: MEMP_NUM_TCP_SEG should be at least as big as TCP_SND_QUEUELEN. If you know what you are doing, define LWIP_DISABLE_TCP_SANITY_CHECKS to 1 to disable this error."

Move MEMP_NUM_TCP_SEG after TCP_SND_QUEUELEN and ...

	#define MEMP_NUM_TCP_SEG               TCP_SND_QUEUELEN


This error:

	basic-emac-lwip-project.sct(7): error: L6236E: No section matches selector - no section to be FIRST/LAST.

You need an sct.Options for Target > Linker > Scatter File 

Uncheck "Use Memory Layout from Target Dialog"

Scatter File > Edit

Copy:

;------------------------------------------------------------------------------
;
;------------------------------------------------------------------------------

Load_region 0x300000 0x10000 {    

    Fixed_region 0x300000 {
        *.o (VECTOR, +First)
        .ANY (+RO)
    }

    Relocate_region +0 {
        *(cstartup +First)
        def.o (+RW +ZI)
    }

    DDRAM_region 0x71000000 {
        .ANY (+RW +ZI)
    }

    ScatterAssert((ImageLength(Fixed_region) + ImageLength(Relocate_region)) <  0xF000)

    ARM_LIB_HEAP 0x30E000 EMPTY 0x1000 {
    }

    ARM_LIB_STACK 0x310000 EMPTY -0x1000 {
    }
}


Misc controls: 

	--entry resetHandler

Still not working? make sure "*(cstartup +First)" matches SAM9M10.s entry area name:

	AREA  cstartup, CODE
	ENTRY        ; Entry point for the application


This error:

	Error: L6218E: Undefined symbol CP15_DrainWriteBuffer (referred from cp15.o).

copy _asm_keil.s (cp15_asm_keil.s) from a Keil project.

Make sure to add it to the project compilation stream.

Other files you need: board_cstartup_keil.s