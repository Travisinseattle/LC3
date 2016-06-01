/*
	cpuDriver.c
	
	Programmer: Travis Holloway
	Date: 4/26/16
	Descritption:
		This file is the driver for the cpu program.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "cpu.c"
#include "hexDecimalBinary.c"


//Array of unsigned shorts that represents memory.
static unsigned short memory[MEM_SIZE];


int main(int argc, char *argv[]) {
	
	/* Initiate the CPU */
	CPU_p cpu = constructCPU();
	initCPU(cpu);
	
	/* Initate the memory */
	//loadMemory(memory);
	
	
	/* Start the controller, requires debug flag to be set (./cpuDriver -d) in order
		to run the debug console.*/
	//controller(cpu, memory, debug_value);
	debug(cpu,memory);
	
  return(0);
}
