#pragma once

#include <stdio.h>

#include "arithm.h"
#include "logic.h"

struct Instruction_base :
		Instruction_push, Instruction_pop,
		Instruction_call, Instruction_mov,
		Instruction_lea, Instruction_xor,
		Instruction_mul, Instruction_div,
		Instruction_sub
{
	enum Type {
		Unk,
		Push,
		Pop,
		Call,
		Mov,
		Lea,
		Xor,
		Sub,
		Mul,
		Div
	};

	Type GetType(unsigned char* memory);

	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);

	unsigned char prefix1 = 0;
	unsigned char prefix2 = 0;
	unsigned char prefix3 = 0;

	Type type = Type::Unk;
};

unsigned long GetModRMDisplacement(unsigned char mod, unsigned char rm, unsigned char &sib, unsigned long &size, unsigned char *mem);
unsigned long CalcModRM(unsigned char modrm, unsigned char sib);
unsigned long SetModRM(unsigned char modrm, unsigned char sib, unsigned long disp32, unsigned char *mem);