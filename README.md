# MyOS
Simple Operating system hobby project

current lines of code: 4296 (keeping track for lols)

# Trello
https://trello.com/b/Lh08g5r6/myos

# Requirements:
## Unix Environment
The makefile and build scripts currently assume you are using a Unix like shell. This could be fixed fairly easily, but for an out of the box experience you will need to be running on something like Mac or linux(or WSL).
## GCC cross compiler:
will need this to build. Follow the instructions at https://wiki.osdev.org/GCC_Cross-Compiler

## QEMU
Emulator used to run the OS, should be in your distros repositories.
## Bochs (Optional)
Another Emulator that can be used, has better debugging but is quite slow. use with "make run_bochs"
      
# Building
Once you have set up a GCC cross compiler simply invoke "make" to build or "make run" to build and run.
