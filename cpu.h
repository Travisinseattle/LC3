/*
    cpu.h
	
    Programer: George Mobus
    Programer: Travis Holloway
	Date: 4/20/16
	Description:
		This header file contains the definitions for the CPU object.
*/

#ifndef _CPU_H_
#define _CPU_H_

#define POINTER_ERROR -1

#define NO_OF_REGISTERS 8

#define OPCODE_MASK 0xF000
#define RD_MASK 0x0E00
#define RS_MASK 0x01C0
#define IMMED_MASK 0x003F

#define HALT 0x0
#define ADD 1
#define ADI 2 
#define NAND 3
#define LDI 4
#define LD 5
#define ST 6
#define BRZ 7


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
Register signExtend(CPU_p cpu);
void add(CPU_p cpu);
void adi(CPU_p cpu);
void nand(CPU_p cpu);
void parseOP(CPU_p cpu);
Byte setZeroFlag (CPU_p cpu);
void debug (CPU_p cpu, unsigned short mem[32]);
void loadMemory(unsigned short mem[32]);
int controller (CPU_p cpu, unsigned short mem[32], Byte debug_value);

#endif
