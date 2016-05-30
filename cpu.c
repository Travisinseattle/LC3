/*
	cpu.c
	
	Programmer: George Mobus
	Programmer: Travis Holloway
	Programmer: Alec Walsh
	Programmer: Tyler Horn
	Date: 4/20/16
	Descritption:
		This file contains the implementation code for the CPU class.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"

#define FETCH 0
#define DECODE 1
#define EVAL_ADDR 2
#define FETCH_OP 3
#define EXECUTE 4
#define STORE 5
#define MEM_SIZE 100

ALU_p constructALU (void) {
	ALU_p alu = (ALU_p) malloc(sizeof(ALU_s));
	return alu;
}

CPU_p constructCPU (void) {
	CPU_p cpu = (CPU_p) malloc (sizeof (CPU_s));
	return cpu;
}

int initCPU (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	cpu->alu = constructALU();
	// initialize register file with random numbers
	cpu->ir = 0;
	cpu->pc = 0;
	cpu->sw = 0;
	cpu->mar = 0;
	cpu->mdr = 0;
	cpu->sext = 0;
	cpu->run_bit = 0;
}

/************************************** Getters *************************************/

Register getOPCODE (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & OPCODE_MASK;
	temp = temp >> 12;
	return (Register) temp;
}

Register getRD (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & RD_MASK;
	temp = temp >> 9;
	return (Register) temp;
}

Register getRS (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & RS_MASK;
	temp = temp >> 6;
	return (Register) temp;
}

Register getBit5 (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & BIT_FIVE;
	temp = temp >> 5;
	return (Register) temp;
}

Register getBit9 (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & BIT_NINE;
	temp = temp >> 9;
	return (Register) temp;
}

Register getBit10 (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & BIT_TEN;
	temp = temp >> 10;
	return (Register) temp;
}

Register getBit11 (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & BIT_ELEVEN;
	temp = temp >> 11;
	return (Register) temp;
}

Register getIR (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	return cpu->ir;
}

/************************************* Functions **********************************/

/* loadRegisters
   Load the array of Registers based on the value parsed from the IR for RD and RS.
*/
void loadRegisters(CPU_p cpu, Register reg1, Register reg2) {
  cpu->reg_file[getRD(cpu)] = reg1;
  cpu->reg_file[getRS(cpu)] = reg2;
}

/* signExtend
   Check for null CPU, make 3 registers: 'val' for a return register, 'bit' a register
   that will be used for checking the sign bit of the Immediate register, and 'negative' a register
   that will be set to all ones for the extra bits required to extend a n-bit number.
   Perform the check and OR val if the immediate is negative, then return val.
*/
Register signExtend(CPU_p cpu, int len) {
  if (cpu == NULL) return POINTER_ERROR;  
  /* Make a register for the return value and the bit value */
  Register val, bit, negative;
  
  /* Switch case to determine how many bits need extended.  Based on value of int, will sign
  extend for 5, 6, 8, and 11 bits by . */
  switch (len) {
	  case 5:  //for sign extention of 5 bits.
	  negative = 0x0FFE0;
	  val = cpu->ir & IMMED_MASK_5;
	  bit = cpu->ir & 1 << 4;
	  break;
	  case 6:  //for sign extention of 6 bits.
	  negative = 0x0FFC0;
	  val = cpu->ir & IMMED_MASK_6;
	  bit = cpu->ir & 1 << 5;
	  break;
	  case 8:  //for sign extention of 8 bits.
	  negative = 0x0FF00;
	  val = cpu->ir & IMMED_MASK_8;
	  bit = cpu->ir & 1 << 7;
	  break;
	  case 11:  //for sign extention of 11 bits.
	  negative = 0x0F800;
	  val = cpu->ir & IMMED_MASK_11;
	  bit = cpu->ir & 1 << 10;
	  break;
	  default:
	  break;	  
  }
  
  /* if the value of bit is > 0, it means the immediate register is negative, so flip all other bits
     to 1. */
  if (bit > 0) {
    /* OR 'negative' with 'val' to flip the most signifigant bits */
    val = val | negative;
  }
  
  return val;
}
/* zeroExtend
	Zero extender for trap instructions.
*/
Register zeroExtend(CPU_p cpu){
	Register val;
	val = cpu->ir & IMMED_MASK_8;
	val = val&0x00FF;
	return val;
}
/* setcc
	A function to flag whether cpu->alu->r is set to 0 or some other value.  Used 
	for determining if the system should perform a break operation.
*/
Register setcc (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	if (cpu->alu->r == 0){
		cpu->sw = 0x0001;
	}
	else if(cpu->alu->r > 0){
		cpu->sw = 0x0010;
	}else{
		cpu->sw = 0x0100;
	}
	return cpu->sw;
}

void debug (CPU_p cpu, unsigned short mem[MEM_SIZE]) {
	//Declare varibles
	int menu;
	int memory = 0;
   int counter;
   printf("Registers				Memory\n");
	printf("=============================================================\n");
   for (counter = 0; counter < 8; counter++) {
      printf("R%d: %x   %04x: %x %x\n", counter, cpu ->reg_file[0]);
   }
   printf("\nMemory Space: \n");
   printf("=============================================================\n");
   for (memory = 0; memory < MEM_SIZE; memory++){
      printf("Memory: %04x: %x\n", memory, mem[memory]);
   }
	printf("PC: %x		SW: %x		IR: %x\n\n", cpu->pc, cpu->sw, cpu->ir);
	//Capture menu choices.
	scanf("%d", &menu);
    getchar();
	
	switch (menu) {
		case 1:  //Load a program.
			break;
		case 2:  //Save a program.
			break;
		case 3:  //Run a program.
			break;
		case 4:  //return from function.
			break;
		case 5:  //Dump the memory.
			break;
		case 6:  //Fill the memory.
			break;
		default:  //Break.
			break;
	}
	
	
}

/* loadMemory
	A function to preset the values in memory that will be used to execute
	the program.  Not all memory will be preset, some will be reserved for 
	storing data generated by the program. Comments represent what I expect
	to happen when the PC reads the memory at that index.
*/
void interpreter(unsigned short mem[MEM_SIZE]) {
	FILE *infile;
	infile = fopen("memory.hex", "r");
	int counter = 0;
	do {
		fscanf(infile, "%u", mem[counter]);
		counter++;
	} while (mem[counter-1] != NULL);
	fclose(infile);
}

int controller (CPU_p cpu, unsigned short mem[MEM_SIZE], Byte debug_value) {
	//Initiate varibles.
	Byte RD, RS;
	Register opcode = 0;
	Register branch_taken_addr;
	ALU_p alu = cpu->alu;
	
    int state = FETCH;
    for (;;) {   
        switch (state) {
			case FETCH:
			/* there is a flag set when you call the inital program.  this allows the debug
			   menu to display and step if it is called.  (./cpuDriver -d). */
				if (debug_value == 1) {
					debug(cpu, mem);
				}
				/* Set cpu->mar to value of pc, which represents an index in the memory.
				   Set cpu->mdr to be equal to memory at index of cpu->mar.
				   Load cpu->ir by setting it equal to cpu->mdr. */
				cpu->mar = cpu->pc;
				cpu->mdr = mem[cpu->mar];
				cpu->ir = cpu->mdr;
				// increment PC
				cpu->pc++;
				// put printf statements in each state to see that it is working
				state = DECODE;
				break;
			case DECODE:
				/* Parse the Opcode and store the result using getOPCODE() function. */
				opcode = getOPCODE(cpu);
				switch (opcode) {
   					case HALT:
						break;
					case ADD:
						break;
					case AND:
						break;
					case BRnzp:
						break;
					case JMP:
						break;
					case JSR:
						break;
					case LD:
						break;
					case LDI:
						break;
					case LDR:
						break;
					case LEA:
						RD = getRD(cpu);
						cpu->sext = signExtend(cpu,9);
						break;
					case NOT:
						RD = getRD(cpu);
						RS = getRS(cpu);
						break;
					case ST:
						RD = getRD(cpu);
						cpu->sext = signExtend(cpu,9);
						break;
					case STI:
						RD = getRD(cpu);
						cpu->sext = signExtend(cpu,9);
						break;
					case STR:
						RD = getRD(cpu);
						RS = getRS(cpu);
						cpu->sext = signExtend(cpu,6);
						break;
					case TRAP:
						break;
					default:
						break;
				}
				state = EVAL_ADDR;
				break;
			case EVAL_ADDR:
				/* Parse the portions of the IR into integer's using the getters for
				each part.  These will be used to access the matching index of
				the array of Registers in the CPU, or memory locations.
				*/
				switch (opcode) {
					case HALT:
						break;
					case ADD:
						break;
					case AND:
						break;
					case BRnzp:
						break;
					case JMP:
						break;
					case JSR:
						break;
					case LD:
						break;
					case LDI:
						break;
					case LDR:
						break;
					case LEA:
						cpu->mar = cpu->pc + cpu->sext;
						break;
					case NOT:
						break;
					case ST:
						cpu->mar = cpu->pc + cpu->sext;
						break;
					case STI:
						cpu->mar = mem[cpu->pc + cpu->sext];
						break;
					case STR:
						cpu->mar = cpu->reg_file[RS] + cpu->sext;
						break;
					case TRAP:
						break;
					default:
						break;
				}
				state = FETCH_OP;
				break;
			case FETCH_OP:
				/* Using the values parsed from the previous step, load the ALU from
				the array of registers in the CPU, or from cpu->sext values, ect.
				*/
				switch (opcode) {
					case HALT:
						break;
					case ADD:
						break;
					case AND:
						break;
					case BRnzp:
						break;
					case JMP:
						break;
					case JSR:
						break;
					case LD:
						break;
					case LDI:
						break;
					case LDR:
						break;
					case LEA:
						cpu->mdr = cpu->mar;
						break;
					case NOT:
						break;
					case ST:
						//Same as below
					case STI:
						//Same as below
					case STR:
						cpu->mdr = cpu->reg_file[RD];
						break;
					case TRAP:
						break;
					default:
						break;
				}
				state = EXECUTE;
				break;
			case EXECUTE:
				switch (opcode) {
					case HALT:
						break;
					case ADD:
						break;
					case AND:
						break;
					case BRnzp:
						//Tyler Working on these
						break;
					case JMP:
						break;
						//Tyler Working on these
					case JSR:
						//Tyler Working on these
						break;
					case LD:
						break;
					case LDI:
						break;
					case LDR:
						break;
					case LEA:
						cpu->reg_file[RD] = cpu->mdr;
						setcc(cpu);
						break;
					case NOT:
						cpu->mdr = ~(RS);
						break;
					case ST:
						//Same as below
					case STI:
						//Same as below
					case STR:
						mem[cpu->mar]=cpu->mdr;
						break;
					case TRAP:
						break;
					default:
						break;
				}
				state = STORE;
				break;
			case STORE:
				/* Finalize the operation by either moving the result to the RD register, or loading/storing
					to memory.  */
				switch (opcode) {
					case HALT:
						break;
					case ADD:
						break;
					case AND:
						break;
					case BRnzp:
						break;
					case JMP:
						break;
					case JSR:
						break;
					case LD:
						break;
					case LDI:
						break;
					case LDR:
						break;
					case LEA:
						break;
					case NOT:
						cpu->reg_file[RD] = cpu->mdr;
						break;
					case ST:
						//Unused
						break;
					case STI:
						//Unused
						break;
					case STR:
						//Unused
						break;
					case TRAP:
						break;
					default:
						break;
				}
				state = FETCH;
				break;
		}
	}
}





