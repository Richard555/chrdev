#Use below command to build kernel mode module.
make COPTS="-g3 -O0"

#Use below command to build user mode AP.
gcc -g3 -O0 -o testdriver testdriver.c

