# SilvioElfInfection

for Linux x86_64

This is a little proof of concept exercise how to infect an ELF file (ET_EXEC) with shellcode. 
The approach is the "Silvio Infection Method" which is described in lots of books and websites.

prog.c is the source code for our "victim" ELF. It prints out its own PID in an infinity loop.
compile with
gcc -Wall -o prog -no-pie prog.c

silvinfect.c is the soucecode for our program, that infects the victim with own code.
compile with
gcc -Wall -o silvinfect silvinfect.c

last but not least the file shellcode contains code to print out a string on the command line.


USAGE:
------

./silvinfect [victim] [shellcode]

When successful a new file is created. It is called infected.
After chmod +x infected & ./infected this should run your original victim program, but at the beginning it executes the code given in shellcode. Try it out!


PROBLEMS:
---------

This works not with shared libraries (ET_DYN) because there is a problem with the real original entry point address. 
