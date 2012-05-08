volatile unsigned int * const UART0DR = (unsigned int *)0xFFF8C000;

void print_uart0(const char *s);

void c_entry() {
 print_uart0("Hello world!\n");
}

void print_uart0(const char *s) {
 while(*s != '\0') { /* Loop until end of string */
 *UART0DR = (unsigned int)(*s); /* Transmit char */
 s++; /* Next char */
 }
}

