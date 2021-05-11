# SilvioElfInfection

for Linux x86_64

This is a little proof of concept exercise how to infect an ELF file (ET_EXEC & ET_DYN) with shellcode. 
The approach is the "Silvio Infection Method" which is described in lots of books and websites.

prog.c is the source code for our "victim" ELF. It prints out its own PID in an infinity loop.
compile with
gcc -Wall -o prog -no-pie prog.c

silvinfect.c is the souce code for our program, that infects the victim with own code.
compile with
gcc -Wall -o silvinfect silvinfect.c

last but not least the file shellcode contains code to print out a string on the command line.


USAGE:
------

    ./silvinfect [victim] [shellcode]


After chmod +x infected & ./infected this should run your original victim program, but at the beginning it executes the code given in shellcode. Try it out!


**Infect a program with a reverse tcp shell**

The file rev_shell_shellcode contains a shellcode that opens a tcp connection to 192.168.178.142:49320 ( = 0xc0 a8 b2 8e). On the host side (192.168.178.142) listen to this port e.g. by typing the command
    
    nc -l 49320



TODO:
-----

JMP_Code should jump back to the original entry point address. This is tricky for PIE code (ET_DYN).


THANKS TO:
----------

- youben11, see https://github.com/youben11/silvio-text-infect/blob/master/silvio_64.c

- elfmaster
