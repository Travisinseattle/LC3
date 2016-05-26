/*
	cpu.c
	
	Programmer: George Mobus
	Programmer: Travis Holloway
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
	cpu->zero = 0;
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

/* setZeroFlag
	A function to flag whether cpu->alu->r is set to 0 or some other value.  Used 
	for determining if the system should perform a break operation.
*/
Byte setZeroFlag (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	if (cpu->alu->r == 0) cpu->zero = 1;
	else cpu->zero = 0;
	printf("cpu->zero is: %x\n", cpu->zero);
	return cpu->zero;
}

/* debug
	A function to print out the contents of the various registers of the CPU and
	the contents of memory.  Provides menu options to load, save and interact with
	programs that the CPU will excecute.
*/
void debug (CPU_p cpu, unsigned short mem[32]) {
	//Declare varibles
	int menu;
	

void debug (CPU_p cpu, unsigned short mem[32]) {
	//Declare varibles
	int menu;
	int memory = 0;
   int counter;
   printf("Registers				Memory\n");
	printf("=============================================================\n");
   for (counter = 0; counter < 8; counter++) {
      printf("R%d: %x   %04x: %x %x\n", counter, cpu ->reg_file[0);
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
void loadMemory(unsigned short mem[32]) {

	mem[0] = 0x880C;   //LDI: RD = R2, RS = Unused, cpu->sext = Immed7 sign extended for 0x12.  	
	mem[1] = 0xAD00;   //LD: RD = R2, RS = R2, cpu->sext = Immed7 sign extended for 0x0.
	mem[2] = 0x4901;   //ADI: RD = R2, RS = R2, cpu->sext = Immed7 sign extended for 0x1.
	mem[3] = 0xB100;   //LD: RD = R4, RS = R2, cpu->sext = Immed7 sign extended for 0x0.
	mem[4] = 0x2E00;   //ADD: RD = R3, RS = R4, cpu->sext = Immed7 sign extended for 0x0.
	mem[5] = 0xE002;   //BRZ OUT: RD = 0, RS = 0, cpu->sext = Immed7 sign extended for 0x2 (pc(6) + IMMED7(2) = 8.
	mem[6] = 0x4901;   //ADI: RD = R2, RS = R2, cpu->sext = Immed7 sign extended for 0x1.
	mem[7] = 0xC980;   //ST: RD = R2, RS = R3, cpu->sext = Immed7 sign extended for 0x0.
	mem[8] = 0x0000;   ////HALT; RD = reg 0, RS = reg 0, cpu->sext = Unused. Program ends.
	mem[9] = 0x0000;   //Unused
	mem[10] = 0x0000;  //Unused
	mem[11] = 0x0000;  //Unused
	mem[12] = 0x0002;  //Hard value for opperand 1
	mem[13] = 0xFFFE;  //Hard value for opperand 2
	mem[14] = 0x0000;  //Result location for addition operation.
	mem[15] = 0x0000;  //
	mem[16] = 0x0000;  //Unused
	mem[17] = 0x0000;  //Unused
	mem[18] = 0x0000;  //Unused
	mem[19] = 0x0000;  //Unused
	mem[20] = 0x0000;  //Unused
	mem[21] = 0x0000;  //Unused
	mem[22] = 0x0000;  //Unused
	mem[23] = 0x0000;  //Unused
	mem[24] = 0x0000;  //Unused
	mem[25] = 0x0000;  //Unused
	mem[26] = 0x0000;  //Unused
	mem[27] = 0x0000;  //Unused
	mem[28] = 0x0000;  //Unused
	mem[29] = 0x0000;  //Unused
	mem[30] = 0x0000;  //Unused
	mem[31] = 0x0000;  //Unused
}

int controller (CPU_p cpu, unsigned short mem[32], Byte debug_value) {
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
						break;
					case NOT:
						break;
					case RET:
						break;
					case ST:
						break;
					case STI:
						break;
					case STR:
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
						break;
					case NOT:
						break;
					case RET:
						break;
					case ST:
						break;
					case STI:
						break;
					case STR:
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
						break;
					case NOT:
						break;
					case RET:
						break;
					case ST:
						break;
					case STI:
						break;
					case STR:
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
						break;
					case RET:
						break;
					case ST:
						break;
					case STI:
						break;
					case STR:
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
						break;
					case RET:
						break;
					case ST:
						break;
					case STI:
						break;
					case STR:
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





