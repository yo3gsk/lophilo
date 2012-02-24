= ldr fail =

root@mini2440:~# gdb ./node/node

(gdb) run
Program received signal SIGILL, Illegal instruction.
0x00117d48 in _start ()
(gdb) bt
#0  0x00117d48 in _start ()
(gdb)  x/1xw 0x5fee4
1i 0x5fee40x5fee4:      0x65646f6e
(gdb)     x/1i 0x5fee4
0x5fee4:        ldr     r6, [r5, #116]  ; 0x74

...the platform parameters were not set for the compiler...

= same location, different error =

root@mini2440:~# gdb ./node/node
(gdb)  x/1xw 0x5fee4
0x5fee4:        0x65646f6e
(gdb)   x/1i 0x5fee4
0x5fee4:        strbvs  r6, [r4, #-3950]!       ; 0xf6e