OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(reset)
SECTIONS
{
        . = ALIGN(4);
	.text : { 
		_stext = .;
		*(.text)
		*(.rodata)                 /* read-only data (constants) */
		*(.rodata*)
		. = ALIGN(4);
 		_etext = . ;
	}

	/* collect all initialized .data sections */
	. = ALIGN(4);
		.dummy : {
			_edummy = .;
		}
	.data : AT ( LOADADDR(.dummy) ) { 
		_sdata = .;
		*(.vectors)
		*(.data)
		_edata = .;
	}

	/* collect all uninitialized .bss sections */
	.bss (NOLOAD) : { 
		. = ALIGN(4);
		_sbss = .;
		*(.bss)
		_ebss = .;
	}
}
end = .;  /* define a global symbol marking the end of application */

