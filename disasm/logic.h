#pragma once

/*
	opsz - Operand size

	** 1 ** 2 *** 3 *** 4 **
	* 8b *     *     *     *
	************************
	*    * 16b *     *     *
	************************
	*    *     * 32b *     *
	************************
	*    *     *     * 64b *
	************************

	opcode		- Instruction opcode
	modrm		- ModRM
	sib			- SIB
	disp32/disp64	- Displacement
*/

struct Instruction_push {
	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);
	virtual unsigned long GetSize(unsigned char* memory);

	static bool IsValid(unsigned char* memory);

	unsigned char opsz	= 0;
	unsigned char opcode	= 0;
	unsigned char modrm		= 0;
	unsigned char sib		= 0;
	unsigned long disp32	= 0;
};

struct Instruction_pop {
	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);
	virtual unsigned long GetSize(unsigned char* memory);

	static bool IsValid(unsigned char* memory);

	unsigned char opsz	= 0;
	unsigned char opcode	= 0;
	unsigned char modrm		= 0;
	unsigned char sib		= 0;
	unsigned long disp32	= 0;
};

struct Instruction_call {
	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);
	virtual unsigned long GetSize(unsigned char* memory);

	static bool IsValid(unsigned char* memory);

	unsigned char opsz	= 0;
	unsigned char opcode	= 0;
	unsigned char modrm		= 0;
	unsigned char sib		= 0;
	unsigned long disp32	= 0;
	unsigned long long address = 0;
};

struct Instruction_mov {
	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);
	virtual unsigned long GetSize(unsigned char* memory);

	static bool IsValid(unsigned char* memory);

	unsigned char opsz	= 0;
	unsigned char opcode	= 0;
	unsigned char modrm		= 0;
	unsigned char sib		= 0;
	unsigned long long operand	= 0;
	unsigned long long disp64	= 0;
};

struct Instruction_lea {
	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);
	virtual unsigned long GetSize(unsigned char* memory);

	static bool IsValid(unsigned char* memory);

	unsigned char opsz	= 0;
	unsigned char opcode	= 0;
	unsigned char modrm		= 0;
	unsigned char sib		= 0;
	unsigned long disp32	= 0;
};

struct Instruction_xor {
	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);
	virtual unsigned long GetSize(unsigned char* memory);

	static bool IsValid(unsigned char* memory);

	unsigned char opsz	= 0;
	unsigned char opcode	= 0;
	unsigned char modrm		= 0;
	unsigned char sib		= 0;
	unsigned long disp32	= 0;

	unsigned long operand = 0;
};