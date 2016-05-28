/*
    cpu.h
	
    Programer: George Mobus
    Programer: Travis Holloway
	Programmer: Alec Walsh
	Programmer: Tyler Horn
	Date: 4/20/16
	Description:
		This header file contains the definitions for the CPU object.
*/

#ifndef _CPU_H_
#define _CPU_H_

#define POINTER_ERROR -1

#define NO_OF_REGISTERS 8
#define RET_REG 7
#define OPCODE_MASK 0xF000
#define RD_MASK 0x0E00
#define RS_MASK 0x01C0
#define IMMED_MASK_5 0x003F
#define IMMED_MASK_6 0x003F
#define IMMED_MASK_8 0x00FF
#define IMMED_MASK_9 0x01FF
#define IMMED_MASK_11 0x07FF
#define BIT_FIVE 0x0020
#define BIT_NINE 0x0200
#define BIT_TEN 0x0400
#define BIT_ELEVEN 0x0800

#define ADD 1 //double (ADD, ADI), parse on bit 5
#define AND 5 //double (AND, ANDI), parse on bit 5
#define BRnzp 0
#define JMP 12 
#define JSR 4 //double (JSR, JSRR) parse on bit 11
#define LD 2
#define LDI 10
#define LDR 6
#define LEA 13
#define NOT 9
#define RET 12
#define ST 3
#define STI 11
#define STR 7
#define TRAP 15

typedef unsigned short Register;
typedef unsigned char Byte;

typedef struct alu_s {
	Register  a, b, r;
} ALU_s;

typedef ALU_s * ALU_p;

typedef struct cpu_s {
	Register reg_file[NO_OF_REGISTERS];
	ALU_p alu;
	Register ir, pc, sw; 	// IR, PC, and SW (status word)
	Register mar, mdr;
	Register sext;
	Byte run_bit, zero;
} CPU_s;

typedef CPU_s * CPU_p;

// prototypes
ALU_p constructALU (void);
CPU_p constructCPU (void);
int initCPU (CPU_p cpu);
Register getIR (CPU_p cpu);
Register getOPCODE (CPU_p cpu);
Register getRD (CPU_p cpu);
Register getRS (CPU_p cpu);
void loadRegisters(CPU_p cpu, Register reg1, Register reg2);
Register signExtend(CPU_p cpu, int len);
Byte setZeroFlag (CPU_p cpu);
void debug (CPU_p cpu, unsigned short mem[32]);
void loadMemory(unsigned short mem[32]);
int controller (CPU_p cpu, unsigned short mem[32], Byte debug_value);

#endif
