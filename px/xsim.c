#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "objdump.h"

#define MemSizeInBytes (1024*1024)
#define NumRegisters 9

typedef unsigned char byte_t;

struct reg
{
	unsigned int eax;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebx;
	/* stack pointer */
	unsigned int esp;
	/* base pointer */
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
	/* instruction pointer */
	unsigned int eip;
	/* Condition code flags */
	byte_t ZF;
	byte_t SF;
	byte_t OF;
} registers;

void printRegisters(struct reg* rp);
unsigned int readUInt32(byte_t* buffer, struct reg* rp);
void getSourceDestId(byte_t b, byte_t* psid, byte_t* pdid);
void setFlags(struct reg* rp, int source, int dest);
void jump(byte_t* buffer, struct reg* rp);
void push(byte_t* buffer, struct reg* rp, unsigned int un);
unsigned int pop(byte_t* buffer, struct reg* rp);

int main(int argc, char** argv)
{
	struct reg* rp = &registers;
	unsigned int* u32rp = (unsigned int*)rp;
	int vflag = 0;
	char opt;
	int index;
	FILE* fp;
	byte_t* buffer;
	byte_t b[1];
	byte_t sourceId;
	byte_t destId;
	/* opcode - used for distinguishing one-operand imul and idiv operations */
	byte_t opcode;
	byte_t curop;
	unsigned int u32n;
	uint64_t u64n;

	//initialize registers to zero
	for (int i=0; i < NumRegisters; i++)
	{
		*((int*)rp + i) = 0;
	}
	rp->ZF = 0;
	rp->SF = 0;
	rp->OF = 0;

	while ((opt = getopt(argc, argv, "i:s:B:a:b:c:d:S:D:v")) != -1)
	{
		switch (opt)
		{
			case 'i':
				rp->eip = atoi(optarg);
				break;
			case 's':
				rp->esp = atoi(optarg);
				break;
			case 'B':
				rp->ebp = atoi(optarg);
				break;
			case 'a':
				rp->eax = atoi(optarg);
				break;
			case 'b':
				rp->ebx = atoi(optarg);
				break;
			case 'c':
				rp->ecx = atoi(optarg);
				break;
			case 'd':
				rp->edx = atoi(optarg);
				break;
			case 'S':
				rp->esi = atoi(optarg);
				break;
			case 'D':
				rp->edi = atoi(optarg);
				break;
			case 'v':
				vflag = 1;
				break;
			default:
				exit(1);
		}
	}
	
	if (optind + 1 != argc)
	{
		printf("Usage: ./xsim <flags> objfile\n");
		exit(1);
	}

	buffer = (byte_t*) calloc(MemSizeInBytes, sizeof(char));
	if (buffer == NULL)
	{
		fprintf(stderr, "Error:xsim.c: Cannot allocate memory size\n");
		exit(1);
	}
	dump(argv[optind]);
	//read dump output into memory
	fp = fopen("output","r");
	if (fp == NULL)
	{
		fprintf(stderr, "Error:xsim.c: cannot open file output\n");
		exit(1);
	}

	index = 0;
	while (fscanf(fp, "%02hhx ", b) != EOF)
	{
		buffer[index++] = *b;
	}

	rp->esp = rp->esp ? rp->esp : MemSizeInBytes;
	rp->ebp = rp->ebp ? rp->ebp : MemSizeInBytes;
	
	while((curop = buffer[rp->eip++]) != 0x90)
	{

		if ((curop & 0xF0) == 0xB0)
		{
			// movl imm32, dest
			getSourceDestId(curop, NULL, &destId);
			*(u32rp + destId) = readUInt32(buffer, rp);
		}
		else if (curop == 0x89)
		{
			// movl source, dest
			getSourceDestId(buffer[rp->eip++], &sourceId, &destId);
			*(u32rp + destId) = *(u32rp + sourceId);
		}
		else if (curop == 0x01)
		{
			// addl source, dest
			getSourceDestId(buffer[rp->eip++], &sourceId, &destId);
			*(u32rp + destId) += *(u32rp + sourceId);
		}
		else if (curop == 0x81)
		{
			getSourceDestId(buffer[rp->eip++], &opcode, &destId);
			if (opcode == 0x0)
			{
				//addl imm32, dest
				*(u32rp + destId) += readUInt32(buffer, rp);
			}
			else if (opcode == 0x5)
			{
				//subl imm32, dest
				*(u32rp + destId) -= readUInt32(buffer, rp);
			}
			else if (opcode == 0x7)
			{
				// cmpl imm32, dest
				setFlags(rp, readUInt32(buffer, rp), *(u32rp + destId));
			}
			else
			{
				printf("Arithmetic opcode not recognized: 0x%x 0x%x\n",
					curop, opcode);
				exit(1);
			}
		}
		else if (curop == 0x05)
		{
			// addl imm32, eax
			rp->eax += readUInt32(buffer, rp);
		}
		else if (curop == 0x83)
		{
			getSourceDestId(buffer[rp->eip++], &opcode, &destId);
			if (opcode == 0x0)
			{
				//addl imm8, dest
				*(u32rp + destId) += buffer[rp->eip];
			}
			else if (opcode == 0x5)
			{
				//subl imm8, dest
				*(u32rp + destId) -= buffer[rp->eip];
			}
			else if (opcode == 0x7)
			{
				// cmpl imm8, dest
				setFlags(rp, buffer[rp->eip], *(u32rp + destId));
			}
			else
			{
				printf("Arithmetic opcode not recognized: 0x%x 0x%x\n",
					curop, opcode);
				exit(1);
			}
			rp->eip++;
		}
		else if (curop == 0x29)
		{
			// subl source, dest
			getSourceDestId(buffer[rp->eip++], &sourceId, &destId);
			*(u32rp + destId) -= *(u32rp + sourceId);
		}
		else if (curop == 0x2d)
		{
			// subl imm32, eax
			rp->eax -= readUInt32(buffer, rp);
		}
		else if (curop == 0x0f && buffer[rp->eip++] == 0xaf)
		{
			// imul S, D
			getSourceDestId(buffer[rp->eip++], &sourceId, &destId);
			*(u32rp + destId) *= *(u32rp + sourceId);
		}
		else if (curop == 0x6b)
		{
			// imul imm8, D
			getSourceDestId(buffer[rp->eip++], NULL, &destId);
			*(u32rp + destId) *= buffer[rp->eip++];
		}
		else if (curop == 0x6b)
		{
			// imul imm32, D
			getSourceDestId(buffer[rp->eip++], NULL, &destId);
			*(u32rp + destId) *= readUInt32(buffer, rp);
		}
		else if (curop == 0xf7)
		{
			getSourceDestId(buffer[rp->eip], &opcode, &sourceId);
			if (opcode == 0x5)
			{
				// imul edx:eax = eax*S
				u64n = (uint64_t) *(u32rp + sourceId) * rp->eax;
				rp->edx = u64n >> 32;
				rp->eax = u64n;
			}
			else if (opcode == 0x7)
			{
				// idiv S, eax
				u64n = rp->edx;
				u64n = (u64n << 32) | rp->eax;
				rp->eax = u64n / *(u32rp + sourceId);
				rp->edx = u64n % *(u32rp + sourceId);
			}
			else
			{
				printf("Arithmetic opcode not recognized: 0x%x 0x%x\n",
					curop, opcode);
				exit(1);
			}
			rp->eip++;
		}
		else if (curop == 0x3d)
		{
			// cmpl imm32, eax
			setFlags(rp, readUInt32(buffer, rp), rp->eax);
		}
		else if (curop == 0x39)
		{
			// cmpl S, D
			getSourceDestId(buffer[rp->eip++], &sourceId, &destId);
			setFlags(rp, *(u32rp + sourceId), *(u32rp + destId));
		}
		else if (curop == 0xeb)
		{
			// jmp short
			jump(buffer, rp);
			rp->eip++;
		}
		else if (curop == 0x7f)
		{
			// jg
			if (1 & ~(rp->SF ^ rp->OF) & ~rp->ZF)
			{
				jump(buffer, rp);
			}
			rp->eip++;
		}
		else if (curop == 0x7c)
		{
			// jl
			if (1 & (rp->SF ^ rp->OF))
			{
				jump(buffer, rp);
			}
			rp->eip++;
		}
		else if (curop == 0x75)
		{
			// jne
			if (1 & ~rp->ZF)
			{
				jump(buffer, rp);
			}
			rp->eip++;
		}
		else if (curop == 0x74)
		{
			// je
			if (1 & rp->ZF)
			{
				jump(buffer, rp);
			}
			rp->eip++;
		}
		else if (curop == 0x7d)
		{
			// jge
			if (1 & ~(rp->SF ^ rp->OF))
			{
				jump(buffer, rp);
			}
			rp->eip++;
		}
		else if (curop == 0x7e)
		{
			// jle
			if (1 & ~(rp->SF ^ rp->OF) & rp->ZF)
			{
				jump(buffer, rp);
			}
			rp->eip++;
		}
		else if (curop == 0xe8)
		{
			// CALL
			u32n = readUInt32(buffer, rp);
			// push return address
			push(buffer, rp, rp->eip);
			// push old base pointer
			push(buffer, rp, rp->ebp);
			// set new base pointer
			rp->ebp = rp->esp;
			// set instruction pointer
			if ((int)u32n > 0)
			{
				rp->eip += u32n;
			}
			else
			{
				rp->eip -= ~u32n + 1;
			}
		}
		else if (curop == 0xc3)
		{
			// RET
			// pop old base poitner
			rp->ebp = pop(buffer, rp);
			// pop return address
			rp->eip = pop(buffer, rp);
		}
		else
		{
			printf("OpCode not recognized: 0x%x\n", curop);
			exit(1);
		}

		if (vflag)
		{
			printRegisters(rp);
			printf("\n");
		}
	}

	if (!vflag)
	{
		rp->eip--;
		printRegisters(rp);
	}

	//cleanup
	free(buffer);
}

/*
 * Print the current state of registers in memory
 * @param rp Pointer to registers in memory
 */
void printRegisters(struct reg* rp)
{
	printf("eip: 0x%x eax: 0x%x ecx: 0x%x edx: 0x%x ebx: 0x%x\n"
		"\tesp: 0x%x ebp: 0x%x esi: 0x%x edi: 0x%x\n"
		"\tcondition_codes: ZF:%x SF:%x OF:%x\n", rp->eip, rp->eax, rp->ecx,
		rp->edx, rp->ebx, rp->esp, rp->ebp, rp->esi, rp->edi,
		rp->ZF, rp->SF, rp->OF);
}

/*
 * Read a 4 byte int from main memory and increment instruction pointer
 * @param buffer Main memory buffer
 * @param rp Pointer to registers
 * @returns Four byte unsigned integer read from main memory
 */
unsigned int readUInt32(byte_t* buffer, struct reg* rp)
{
	unsigned int out = 0;
	for( int i=0; i < sizeof(int); i++, rp->eip++)
	{
		out |= buffer[rp->eip] << 8 * i ;
	}
	return out;
}

/*
 * Parses a one byte instruction modifyer in Mod-Reg/OpCode-R/M format into
 * source and destination pointers. Byte format (MMSSSDDD).
 * @param b The byte being parsed
 * @param psid Pointer to read the source register address or opcode into
 * @param pdestid Pointer to read the destination register address into
 */
void getSourceDestId(byte_t b, byte_t* psid, byte_t* pdestid)
{
	if (psid)
	{
		*psid = (b >> 3) & 0x7;
	}

	if (pdestid)
	{
		*pdestid = b & 0x7;
	}
}

/*
 * Sets the flags on the flag register based on the x86 command
 *     cmpl S,D
 * @param rp Pointer to registers
 * @param source the source operand
 * @param dest the destination operand
 */
void setFlags(struct reg* rp, int source, int dest)
{
	rp->ZF = (dest == source);
	rp->SF = (dest < source);
	rp->OF = ((dest < 0) == (0 < source)) 
			&& ((dest - source < 0) != (dest < 0));
}

/*
 * Jump command reads next byte and increments the instruction pointer (eip) 
 * by that two's complement signed quantity
 * @param buffer main memory buffer
 * @param rp pointer to registers
 */
void jump(byte_t* buffer, struct reg* rp)
{
	rp->eip += (char) buffer[rp->eip];
}

/*
 * Push a 4 byte integer onto the stack in little endian format
 * @param buffer main memory buffer
 * @param rp pointer to registers
 * @param u32n the 4 byte to push to stack
 */
void push(byte_t* buffer, struct reg* rp, unsigned int un)
{
	for (int i = sizeof(int) - 1; i >= 0; i--, rp->esp--)
	{
		buffer[rp->esp] = (byte_t)(un >> 8 * i);
	}
}

/*
 * Pop a 4 byte integer from the stack in little endian format
 * @param buffer main memory buffer
 * @param rp pointer to registers
 * @returns The 4 byte int poped from the stack
 */
unsigned int pop(byte_t* buffer, struct reg* rp)
{	
	unsigned int out = 0;
	for( int i=0; i < sizeof(int); i++)
	{
		out |= buffer[++(rp->esp)] << 8 * i;
	}
	return out;
}
