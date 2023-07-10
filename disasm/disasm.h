#pragma once

#include <stdio.h>

#include "arithm.h"
#include "logic.h"

struct Instruction_base :
		Instruction_div, Instruction_mul,
		Instruction_push
{
	enum Type {
		Unk,
		Mul,
		Div,
		Push
	};

	Type GetType(unsigned char* memory);

	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);

	Type type = Type::Unk;
};

unsigned long GetModRMDisplacement(unsigned char mod, unsigned char rm, unsigned char &sib, unsigned long &size, unsigned char *mem);
unsigned long CalcModRM(unsigned char modrm, unsigned char sib);
unsigned long SetModRM(unsigned char modrm, unsigned char sib, unsigned long disp32, unsigned char *mem);