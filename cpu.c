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


//P.g 537 on PDF for the OPCODES

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
	cpu->pc = 0x3000;
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

Register get9Offset(CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu -> ir & IMMED_MASK_9;
	return (Register) temp;
}

Register get11Offset(CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu -> ir & IMMED_MASK_11;
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

Register getS2 (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & S2_MASK;
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

Register getBit12 (CPU_p cpu) {
	if (cpu == NULL) return POINTER_ERROR;
	Register temp = cpu->ir & BIT_ELEVEN;
	temp = temp >> 12;
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
  extend for 5, 6, 8, 9, and 11 bits by . */
  switch (len) {
	  case 5:  //for sign extention of 5 bits.
	  negative = 0xFFE0;
	  val = cpu->ir & IMMED_MASK_5;
	  bit = cpu->ir & (1 << 4);
	  break;
	  case 6:  //for sign extention of 6 bits.
	  negative = 0xFFC0;
	  val = cpu->ir & IMMED_MASK_6;
	  bit = cpu->ir & (1 << 5);
	  break;
	  case 8:  //for sign extention of 8 bits.
	  negative = 0xFF00;
	  val = cpu->ir & IMMED_MASK_8;
	  bit = cpu->ir & (1 << 7);
	  break;
	  case 9:  //for sign extention of 9 bits.
	  negative = 0xFE00;
	  val = cpu->ir & IMMED_MASK_9;
	  bit = cpu->ir & (1 << 8);
	  break;
	  case 11:  //for sign extention of 11 bits.
	  negative = 0xF800;
	  val = cpu->ir & IMMED_MASK_11;
	  bit = cpu->ir & (1 << 10);
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

/* setcc
	A function to flag whether cpu->alu->r is set to 0 or some other value.  Used 
	for determining if the system should perform a break operation.
*/
void setcc (CPU_p cpu) {
	if (cpu->alu->r == 0){
		cpu->sw = 0x0001;
	}
	else if(cpu->alu->r > 0){
		cpu->sw = 0x0010;
	}else{
		cpu->sw = 0x0100;
	}
}
/* trapVectorTable
	Simulates all traps.
*/
int trapVectorTable(CPU_p cpu, unsigned short mem[MEM_SIZE]){
	int trap = cpu->sext;
	int i = 0;
	switch(trap){
		//GETC Trap
		case 32:
			scanf("%c",&(cpu->reg_file[0]));
			break;
		//OUT Trap
		case 33:
			printf("%c",cpu->reg_file[0]);
			break;
		//PUTS Trap
		case 34:
			while(mem[cpu->reg_file[0]+i]!=0x0000){
				printf("%c",mem[cpu->reg_file[0]+i]);
				i++;
			}
			break;
		//IN Trap
		case 35:
			printf("Input a character>");
			scanf("%c",&(cpu->reg_file[0]));
			break;
		//HALT Trap
		case 37:
			return 1;
			break;
	}
	return 0;
}

int debug (CPU_p cpu, unsigned short mem[MEM_SIZE]) {
	for(;;){
	//Declare varibles
	int menu;
	int memory = DEBUG_MEMORY;
   int counter;
   printf("\nRegisters				Memory\n");
	printf("=============================================================\n");
   for (counter = 0; counter < 8; counter++) {
      printf("R%d: %04X   			", counter, cpu ->reg_file[counter]);
	  printf("0x%04X: %04X  0x%04X: %04X\n", memory, mem[memory],memory+1, mem[memory+1]);
	  memory+=2;
   }
   printf("=============================================================\n");
	printf("PC: %04X		SW: %04X		IR: %04X\n\n", cpu->pc, cpu->sw, cpu->ir);
	//Capture menu choices.
	printf("1)Load\n2)Save\n3)Run\n4)Step\n5)Dump\n6)Fill\nSelect a menu choice: ");
	scanf("%d", &menu);
    getchar();
	
	switch (menu) {
		case 1:  //Load a program.
			loadMemory(cpu, mem);
			DEBUG_MEMORY = cpu->pc;
			break;
		case 2:  //Save a program.
			saveMemory(mem);
			break;
		case 3:  //Run a program.
			return 0;
			break;
		case 4:  //return from function.
			return 1;
			break;
		case 5:  //Dump the memory.
			DEBUG_MEMORY = memoryDump();
			break;
		case 6:  //Fill the memory.
			memoryFill(mem);
			break;
		case 7: //Edit other elements
			editRP(cpu);
		default:  //Break.
			break;
	}
	}
}

/* loadMemory
	A function to preset the values in memory that will be used to execute
	the program.  Not all memory will be preset, some will be reserved for 
	storing data generated by the program. Comments represent what I expect
	to happen when the PC reads the memory at that index.
*/
void loadMemory(CPU_p cpu,unsigned short mem[MEM_SIZE]) {
	FILE *infile;
	printf("Input file name to load: ");
	char fileName[50];
	scanf("%s",fileName);
	//printf("%s\n",fileName);
	infile = fopen(fileName, "rw");
	//printf("File opened\n");
	int counter;
	fscanf(infile,"%X",&counter);
	cpu->pc = counter;
	//printf("Read counter! %04X\n",counter);
	while(fscanf(infile, "%X", &mem[counter])!=-1){
		//printf("Trying to read\n %04X",counter);
		counter++;
	} 
	fclose(infile);
   //Commands actually start at mem[1], first index is the .orig
}

/* saveMemory
	This function saves the current memory to a file, 
	allowing programs to be saved.
*/
void saveMemory(unsigned short mem[MEM_SIZE]){
	FILE *outfile;
	printf("Input file name to save to: ");
	char fileName[50];
	scanf("%s",fileName);
	outfile = fopen(fileName, "w");
	int i;
	printf("\nInput starting memory address to save: ");
	scanf("%X",&i);
	fprintf(outfile,"%04X\n",i);
	int z;
	printf("\nInput number of memory addresses to save: ");
	scanf("%d",&z);
	z+=i;
	for(;i<z;i++){
		fprintf(outfile,"%04X\n",mem[i]);
	}
	fclose(outfile);
}
/* memoryDump()
	Asks for user input and then returns that info.
*/
int memoryDump(){
	printf("\nInput starting memory address to view: ");
	int i;
	scanf("%X",&i);
	return i;
}
/* memoryFill()
	Asks for user input corresponding to a memory location
	then asks for a hex value to be placed there.
*/
void memoryFill(unsigned short mem[MEM_SIZE]){
	printf("\nInput starting memory address to edit: ");
	int i;
	scanf("%X",&i);
	printf("\nInput value to store: ");
	int z;
	scanf("%X",&z);
	mem[i] = z;
}
/*Debug Editer
	Only to be used for testing.
*/
void editRP(CPU_p cpu){
	printf("\n1)Edit Registers, 2)Edit PC : ");
	int i;
	int x;
	int z;
	scanf("%d",&i);
	if(i==1){
		printf("\nWhich Register?: ");
		scanf("%d",&x);
		printf("\nTo what value?: ");
		scanf("%X",&z);
		cpu->reg_file[x] = z;
	}else{
		printf("\nEnter PC value: ");
		scanf("%X",&z);
		cpu->pc = z;
	}
}

Byte getNZP(Register IR) {
	Byte nzp = IR >> 9;
	return nzp;
}

void controller (CPU_p cpu, unsigned short mem[MEM_SIZE], Byte debug_value) {
	//Initiate varibles.
	Byte RD, RS, S2, nzp_flag, bit5, bit11;
	//nzp_flag is used by branch case;
	Register opcode = 0;
	Register TEMP;
	ALU_p alu = cpu->alu;
	
    int state = FETCH;
    for (;;) {   
        switch (state) {

        //NOTE: By contract, do not JSR to another JMP or Branch statement.

			case FETCH:
				if (debug_value == 1) {
					debug_value = debug(cpu, mem);
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
					case ADD:
						bit5 = getBit5(cpu);
						/* First get the 5th bit.*/
						if (bit5 > 0) { 
						/* If bit is set, call signExtend to get the 5-bit 
						immediate value.*/
							RD = getRD(cpu);
							RS = getRS(cpu);
							cpu->sext = signExtend(cpu,5);
						} else { 
						/* Otherwise, capture destination, and source 1 and 2 registers.*/
							RD = getRD(cpu);
							RS = getRS(cpu);
							S2 = getS2(cpu);
							}
						break;
					case AND:
						bit5 = getBit5(cpu);
						/* First get the 5th bit.*/
						if (bit5 > 0) { 
						/* If bit is set, call signExtend to get the 5-bit 
						immediate value.*/
							RD = getRD(cpu);
							RS = getRS(cpu);
							cpu->sext = signExtend(cpu,5);
						} else { 
						/* Otherwise, capture destination, and source 1 and 2 registers.*/
							RD = getRD(cpu);
							RS = getRS(cpu);
							S2 = getS2(cpu);
							}
						break;
					case BRnzp:
						cpu->sext = signExtend(cpu, 9);
						break;
					case JMP:
						RS = getRS(cpu);
						break;
					case JSR:
						TEMP = cpu->pc;
						bit11 = getBit11(cpu);
						if (bit11 > 0) {
							/*JSR case: Load cpu->sext with offset11. */
							cpu->sext = signExtend(cpu, 11);
						} else {
							/*JSRR case: Capture the base register. */
							RS = getRS(cpu);
						}
						break;
					case LD:
						RD = getRD(cpu);
						cpu->sext = signExtend(cpu, 9);
						break;
					case LDI:
						RD = getRD(cpu);
						cpu->sext = signExtend(cpu, 9);
						break;
					case LDR:
						RD = getRD(cpu);
						RS = getRS(cpu);
						cpu->sext = signExtend(cpu, 6);
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
						cpu->sext = signExtend(cpu,8);
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
					case ADD: //No operation required.
						break;
					case AND: //No operation required.
						break;
					case BRnzp:
					/*Evaluate the address and store result in alu->r for use if branch is taken. */
						alu->a = cpu->pc;
						alu->b = cpu->sext;
						alu->r = alu->a + alu->b;
						break;
					case JMP: //No operation required.
						break;
					case JSR:
					/*JSR Case: if bit11 is set, evaluate the address of the new pc by loading
					the alu->a with cpu->pc and alu->b with cpu->sext and adding them together.  Store
					result in alu->r.*/
						if (bit11 > 0) {
							alu->a = cpu->pc;
							alu->b = cpu->sext;
							alu->r = alu->a + alu->b;
						}
						break;
					case LD:
					/* In order to evaluate the value to placed in cpu->mar, first load
					the adder with the cpu->pc and cpu->sext and then add them together.
					Then load cpu-mar with the result.  Finally, evaluate the result for
					positive/negative/zero and set the cpu->sw using setcc().*/
						alu->a = cpu->pc;
						alu->b = cpu->sext;
						alu->r = alu->a + alu->b;
						cpu->mar = alu->r;
						setcc(cpu);
						break;
					case LDI:
					/* In order to evaluate the value to placed in cpu->mar, first load
					the adder with the cpu->pc and cpu->sext and then add them together.
					Then load cpu-mar with the result.  Finally, evaluate the result for
					positive/negative/zero and set the cpu->sw using setcc().*/
						alu->a = cpu->pc;
						alu->b = cpu->sext;
						alu->r = alu->a + alu->b;
						cpu->mar = alu->r;
						setcc(cpu);
						break;
					case LDR:
					/* In order to evaluate the value to placed in cpu->mar, first load
					the adder with the cpu->reg_file[RS] and cpu->sext and then add them
					together. Then load cpu-mar with the result.  Finally, evaluate the
					result for positive/negative/zero and set the cpu->sw using setcc().*/
						alu->a = cpu->reg_file[RS];
						alu->b = cpu->sext;
						alu->r = alu->a + alu->b;
						cpu->mar = alu->r;
						setcc(cpu);
						break;
					case LEA:
					/* In order to evaluate the value to be placed in the RD, first load
					the adder with the cpu->pc and cpu->sext and then add them together.
					Finally, evaluate the result for positive/negative/zero and set the 
					cpu->sw using setcc().*/
						alu->a = cpu->pc;
						alu->b = cpu->sext;
						alu->r = alu->a + alu->b;
						setcc(cpu);
						break;
					case NOT: //No operation required.
						cpu->alu->a = cpu->reg_file[RS];
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
					case TRAP: //No operation required.
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
					case ADD:
						if (bit5 > 0) {
						/* If bit5 is set, alu->a is loaded with cpu->reg_file[RS],
						alu->b with cpu->sext.*/
							alu->a = cpu->reg_file[RS];
							alu->b = cpu->sext;
						} else {
						/* Otherwise, alu->a is loaded with cpu->reg_file[RS],
						alu->b with cpu->reg_file[S2].*/
							alu->a = cpu->reg_file[RS];
							alu->b = cpu->reg_file[S2];
						}
						break;
					case AND:
						if (bit5 > 0) {
						/* If bit5 is set, alu->a is loaded with cpu->reg_file[RS],
						alu->b with cpu->sext.*/
							alu->a = cpu->reg_file[RS];
							alu->b = cpu->sext;
						} else {
						/* Otherwise, alu->a is loaded with cpu->reg_file[RS],
						alu->b with cpu->reg_file[S2].*/
							alu->a = cpu->reg_file[RS];
							alu->b = cpu->reg_file[S2];
						}
						break;
					case BRnzp:  //No operation required.
						break;
					case JMP:  //No operation required.
						break;
					case JSR:  //No operation required.
						break;
					case LD:
					/* Load the cpu->mdr with the data found at the memory index of cpu->mar. */
						cpu->mdr = mem[cpu->mar];
						break;
					case LDI:
					/* Load the cpu->mdr with the data found at the memory index of cpu->mar.
					Set cpu->mar to the value of cpu->mdr.  Load cpu->mdr with data found at 
					mem[cpu->mar]. */
						cpu->mdr = mem[cpu->mar];
						cpu->mar = cpu->mdr;
						cpu->mdr = mem[cpu->mar];
						break;
					case LDR:
					/* Load the cpu->mdr with the data found at the memory index of cpu->mar. */
						cpu->mdr = mem[cpu->mar];
						break;
					case LEA:  //No operation required.
						break;
					case NOT:  //No operation required.
						break;
					case ST:  //No operation required.
						break;
					case STI:  //No operation required.
						break;
					case STR:
						cpu->mdr = cpu->reg_file[RD];
						break;
					case TRAP:  //No operation required.
						break;
					default:
						break;
				}
				state = EXECUTE;
				break;
			case EXECUTE:
				switch (opcode) {
					case ADD:
					/* Add a and b and place the result in alu->r.  Call setcc
					to load cpu->sw with the correct bits based on the whether the
					result of the addition is positive, negative or zero.*/
						alu->r = alu->a + alu->b;
						setcc(cpu);
						break;
					case AND:
					/* And a and b and place the result in alu->r.  Call setcc
					to load cpu->sw with the correct bits based on the whether the
					result of the addition is positive, negative or zero.*/
						alu->r = alu->a & alu->b;
						setcc(cpu);
						break;
					case BRnzp:
						if ((getBit10(cpu) == 1 && cpu->sw == 0x0001) ||
						(getBit11(cpu) == 1 && cpu->sw == 0x0010) ||
						(getBit12(cpu) == 1  && cpu->sw == 0x0100)) {
							printf("Trying to BRANCH");
							cpu->pc = alu->r;
						}
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
						cpu->reg_file[RD] = alu->r;
						setcc(cpu);
						break;
					case NOT:
						cpu->alu->r = ~(cpu->alu->a);
						break;
					case ST:
						break;
					case STI:
						break;
					case STR:
						mem[cpu->mar]=cpu->mdr;
						break;
					case TRAP:
						if(trapVectorTable(cpu,mem)==1){
							debug(cpu,mem);
							return;
						}
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
					case ADD:
					/* Load reg_file[RD] with the value in alu->r.*/
					cpu->reg_file[RD] = alu->r;
						break;
					case AND:
					/* Load reg_file[RD] with the value in alu->r.*/
					cpu->reg_file[RD] = alu->r;
						break;
					case BRnzp:  //No operation required.
						break;
					case JMP:
					/*Set the cpu->pc to be the value in the RS register. */
						cpu->pc = cpu->reg_file[RS];
						break;
					case JSR:
						if (bit11 > 0) {
							/*JSR case: If bit11 is set, load cpu->pc with the effective address stored in alu->r,
							and then set register 7 to the value of TEMP, which was cpu->pc before the jump.  This
							insures a pc value is stored in register 7 of where the jump started.*/
							cpu->pc = alu->r;
							cpu->reg_file[7] = TEMP;
						} else {
							/*JSRR case: If bit11 is not set, load cpu->pc value stored in cpu->reg_file[RS] (base register),
							and then set register 7 to the value of TEMP, which was cpu->pc before the jump.  This
							insures a pc value is stored in register 7 of where the jump started.*/
							cpu->pc = cpu->reg_file[RS];
							cpu->reg_file[7] = TEMP;
						}
						break;
					case LD:
					/*Load the target register with the data in cpu->mdr. */
						cpu->reg_file[RD] = cpu->mdr;
						break;
					case LDI:
					/*Load the target register with the data in cpu->mdr. */
						cpu->reg_file[RD] = cpu->mdr;
						break;
					case LDR:
					/*Load the target register with the data in cpu->mdr. */
						cpu->reg_file[RD] = cpu->mdr;
						break;
					case LEA:
					/*Load the target register with the value of alu->r. */
						cpu->reg_file[RD] = cpu->alu->r;
						break;
					case NOT:
					/*Load the target register with the value of alu->r. */
						cpu->reg_file[RD] = cpu->alu->r;
						break;
					case ST:  //No operation required.
						break;
					case STI:  //No operation required.
						break;
					case STR:  //No operation required.
						break;
					case TRAP:  //No operation required.
						break;
					default:
						break;
				}
				state = FETCH;
				break;
		}
	}
}





