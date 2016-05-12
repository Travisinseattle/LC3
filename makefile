
cpuDriver: cpu.o  hexDecimalBinary.o cpuDriver.c
	gcc -o cpuDriver cpuDriver.c

hexDecimalBinary.o: hexDecimalBinary.c
	gcc -c hexDecimalBinary.c

cpu.o: cpu.h cpu.c
	gcc -c cpu.c
