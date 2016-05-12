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
	cpu->pc = 0;
	cpu->ir = 0;
	cpu->sext = 0;
}

Register getIR (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	return cpu->ir;
}

/************************************** Getters *************************************/

Register getOPCODE (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & OPCODE_MASK;
	temp = temp >> 13;
	return (Register) temp;
}

Register getRD (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & RD_MASK;
	temp = temp >> 10;
	return (Register) temp;
}

Register getRS (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & RS_MASK;
	temp = temp >> 7;
	return (Register) temp;
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
   that will be used for checking the sign bit of the IMMED7, and 'negative' a register
   that will be set to all ones for the extra 9 bits required to extend a 7-bit number.
   Perform the check and or val if the IMMED7 is negative, return val.
*/
Register signExtend(CPU_p cpu) {
  if (cpu == NULL) return POINTER_ERROR;  
  /* Make a register for the return value and the bit value */
  Register val, bit;
  /* set a negative Register with the first 9 bits set to 1, the remaining to 0. */
  Register negative = 0xFF80;

  /*To start, copy the 7 bits that compose the immed7 from cpu->ir to
    val. */
  val = cpu->ir & IMMED_MASK;
  
  /* set bit using bitshifting and check against cpu->ir. */
  bit = cpu->ir & 1 << 6;

  /* if the value of bit is > 0, it means immed7 is negative, so flip all other bits
     to 1. */
  if (bit > 0) {
    /* OR 'negative' with 'val' to flip the 9 most signifigant bits */
    val = val | negative;
  }
  
  return val;
}

/* add
   Method to perform an add function in the ALU.  Creates a pointer to the alu
   in the CPU, loads A and B registers with values obtained from the CPU registers
   based on the specified locations of RD and RS.  Performs an add operation on A and
   B and places the result in the R register.  Load's the designated RD register with
   the value found in the R register. 
*/
void add(CPU_p cpu) {
  //Create the pointer to the CPU's ALU.
  ALU_p alu = cpu->alu;
  //Load values based on IR code.
  alu->a = cpu->reg_file[getRD(cpu)];
  alu->b = cpu->reg_file[getRS(cpu)];
  //perform ADD operation and store to register R.
  alu->r = alu->a + alu->b;
  //load RD register with value in register R.
  cpu->reg_file[getRD(cpu)] = alu->r;
}

/* adi
   Method to perform an ADI function in the ALU.  Creates a pointer to the alu
   in the CPU, loads A and B registers with values obtained from the CPU registers
   based on the specified locations of RD and sign extended IMMED7.  Performs an ADD 
   operation on A and B and places the result in the R register.  Load's the designated 
   RD register with the value found in the R register. 
*/
void adi(CPU_p cpu) {
  //Create the pointer to the CPU's ALU.
  ALU_p alu = cpu->alu;
  //Load values based on IR code.
  alu->a = cpu->reg_file[getRD(cpu)];
  alu->b = signExtend(cpu);
  //perform ADD operation and store to register R.
  alu->r = alu->a + alu->b;
  //load RD register with value in register R.
  cpu->reg_file[getRD(cpu)] = alu->r;
}

/* nand
   Method to perform an NAND function in the ALU.  Creates a pointer to the alu
   in the CPU, loads A and B registers with values obtained from the CPU registers
   based on the specified locations of RD and RS.  Performs an NAND operation on A
   and B and places the result in the R register.  Load's the designated RD register
   with the value found in the R register. 
*/
void nand(CPU_p cpu) {
  //Create the pointer to the CPU's ALU.
  ALU_p alu = cpu->alu;
  //Load values based on IR code.
  alu->a = cpu->reg_file[getRD(cpu)];
  alu->b = cpu->reg_file[getRS(cpu)];  
  //perform NAND operation and store to register R.
  alu->r = ~(alu->a & alu->b);;
  //load RD register with value in register R.
  cpu->reg_file[getRD(cpu)] = alu->r;
}

/* Old code to perform 3 operations, used in Assignment 3. */
void parseOP(CPU_p cpu) {
  Byte op = getOPCODE (cpu);

  switch (op) {
  case 0:
    add(cpu);
    break;
  case 1:
    adi(cpu);
    break;
  case 2:
    nand(cpu);
    break;
  default:
    break;
  }
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
	
	//Setup print statements.
	printf("********************** SC-1 Simulator: Travis Holloway ********************\n\n");
	printf("Registers				Memory\n");
	printf("=============================================================\n");
	printf("R0: %x			0x0000:	%x	%x\n", cpu->reg_file[0], mem[0], mem[1]);
	printf("R1: %x			0x0002:	%x	%x\n", cpu->reg_file[1], mem[2], mem[3]);
	printf("R2: %x			0x0004:	%x	%x\n", cpu->reg_file[2], mem[4], mem[5]);
	printf("R3: %x			0x0006:	%x	%x\n", cpu->reg_file[3], mem[6], mem[7]);
	printf("R4: %x			0x0008:	%x	%x\n", cpu->reg_file[4], mem[8], mem[9]);
	printf("R5: %x			0x000A:	%x	%x\n", cpu->reg_file[5], mem[10], mem[11]);
	printf("R6: %x			0x000C:	%x	%x\n", cpu->reg_file[6], mem[12], mem[13]);
	printf("R7: %x			0x000E:	%x	%x\n\n", cpu->reg_file[7], mem[14], mem[15]);
	printf("PC: %x		SW: %x		IR: %x\n\n", cpu->pc, cpu->sw, cpu->ir);
	printf("Please choose from the following options:\n");
	printf("Load = 1, Save = 2, Run = 3, Step = 4, Memory Dump = 5, Memory Fill = 6.\n\n");
	
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
	/* old code for testing of assignment 4.
	mem[0] = 0x2500;  //ADD; RD = reg 1, RS = reg 2, cpu->sext = unsused.  Result is 0 + 0 = 0.	
	mem[1] = 0x4501;  //ADI; RD = reg 1, RS = reg 2, cpu->sext = 1.  Result is 0 + 1 = 1.
	mem[2] = 0x6500;  //NAND; RD = reg 1, RS = reg 2, cpu->sext = unsused.  Result is FFFF (0 + 0 flipped).
	mem[3] = 0x8E02;  //LDI; RD = reg 3, RS = reg 4, cpu->sext = 2.  Result is 0 + 2 = 2.
	mem[4] = 0xAE0A;  //LD; RD = reg 3, RS = reg 4, cpu->sext = 10.  Result is value at Memory[10] or 15.	
	mem[5] = 0xD70C;  //ST; RD = reg 5, RS = reg 6, cpu->sext = 12.  Result is value at Memory[12] or 0.	
	mem[6] = 0xFC18;  //BRZ; RD = reg 7, RS = reg 0, cpu->sext = 24 (24 + 7 = 31).  Move PC to Memory[31].
	mem[7] = 0x0000;  //Unused
	mem[8] = 0x0000;  //Unused
	mem[9] = 0x0000;  //Unused
	mem[10] = 0x000F;  //Target location for load operation.
	mem[11] = 0x0000;  //Unused
	mem[12] = 0xFFFF;  //Target loaction for Store, set to 0xFFFF so a change can be verified.
	mem[13] = 0x0000;  //Unused
	mem[14] = 0x0000;  //Unused
	mem[15] = 0x0000;  //Unused
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
	mem[31] = 0x0000;  //HALT; RD = reg 0, RS = reg 0, cpu->sext = Unused. Program ends.
	*/
	
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
				if (debug_value == 1) {
					debug(cpu, mem);
				}
				// get memory[PC] into IR - memory is a global array
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
				RD = getRD(cpu);
				RS = getRS(cpu);
				cpu->sext = signExtend(cpu);				
				state = EVAL_ADDR;
				break;
			case EVAL_ADDR:
				/* Parse the portions of the IR into integer's using the getters for
				each part.  These will be used to access the matching index of
				the array of Registers in the CPU, or memory locations.
				*/
				switch (opcode) {
					case ADD:						
						break;
					case ADI:
						break;
					case NAND:
						break;
					case LDI:
						break;
					case LD:
						//Load cpu->mar with value in rs + the immed7 sign extended.
						cpu->mar = cpu->reg_file[RS] + cpu->sext;
						break;
					case ST:
						//Load cpu->mar with value in rd + the immed7 sign extended.
						cpu->mar = cpu->reg_file[RD] + cpu->sext;
						break;
					case BRZ:
						//Add pc to sign extended immed7 to compute branch address.
						branch_taken_addr = cpu->pc + cpu->sext;
						break;
					case HALT:
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
					case ADD:	//Load A and B registers in the ALU using RD and RS registers.
						alu->a = cpu->reg_file[RD];
						alu->b = cpu->reg_file[RS];
						break;
					case ADI:	//Load A and B registers in the ALU using RS and cpu->sext
						alu->a = cpu->reg_file[RS];
						alu->b = cpu->sext;
						break;
					case NAND:	//Load A and B registers in the ALU using RD and RS registers.
						alu->a = cpu->reg_file[RD];
						alu->b = cpu->reg_file[RS];
						break;
					case LDI: 	//Load cpu->mdr with value of cpu->sext.
						cpu->mdr = cpu->sext;
						break;
					case LD:  //Load cpu->mdr with data at mem[cpu->mar].
						cpu->mdr = mem[cpu->mar];
						break;
					case ST:	//Load mdr with data in cpu->reg_file[RS].
						cpu->mdr = cpu->reg_file[RS];
						break;
					case BRZ:	//No action required.
						break;
					case HALT:	//No action required.
						break;
					default:
						break;
				}
				state = EXECUTE;
				break;
			case EXECUTE:
				switch (opcode) {
					case ADD:	//Peform the requested operation in the ALU and store result in alu->r;
						alu->r = alu->a + alu->b;
						setZeroFlag(cpu);
						break;
					case ADI:	//Peform the requested operation in the ALU and store result in alu->r;
						alu->r = alu->a + alu->b;
						setZeroFlag(cpu);
						break;
					case NAND:	//Peform the requested operation in the ALU and store result in alu->r;
						alu->r = ~(alu->a & alu->b);
						setZeroFlag(cpu);
						break;
					case LDI: 	//No action required.
						break;
					case LD:	//No action required.
						break;
					case ST:	//Save data in cpu->mdr to location of mem[cpu->mar].
						mem[cpu->mar] = cpu->mdr;
						break;
					case BRZ:   //If cpu->zero is not 0, set cpu->pc to branch address.
						if (cpu->zero == 1) {
							cpu->pc = branch_taken_addr;
						}						
						break;
					case HALT:	//Halt has been called, end the program.
						return(0);
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
					case ADD:	//Move the result of the operation from cpu->r to cpu->reg_file[RD];
						cpu->reg_file[RD] = alu->r;
						break;
					case ADI:	//Move the result of the operation from cpu->r to cpu->reg_file[RD];
						cpu->reg_file[RD] = alu->r;
						break;
					case NAND:	//Move the result of the operation from cpu->r to cpu->reg_file[RD];
						cpu->reg_file[RD] = alu->r;
						break;
					case LDI: 	//Load RD with the value of cpu->mdr.
						cpu->reg_file[RD] = cpu->mdr;
						break;
					case LD:	//Load RD with the value in cpu->mdr.
						cpu->reg_file[RD] = cpu->mdr;
						break;
					case ST:	//Store the value of cpu->reg_file[RS] in the location of cpu->mar.
						mem[cpu->mar] = cpu->reg_file[RS];
						break;
					case BRZ:   //No action required.						
						break;
					case HALT:	//No action required.
						break;
					default:
						break;
				}
				state = FETCH;
				break;
		}
	}
}





