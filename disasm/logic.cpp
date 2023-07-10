#include "disasm.h"

bool Instruction_push::IsValid(unsigned char* memory) {
	return (memory[0] == 0xff) && ((memory[1] & 0x38) == 0x30);
}

unsigned long Instruction_push::Decode(unsigned char* memory) {
	unsigned long size = 0;

	if (memory[0] == 0xff && ((memory[1] & 0x38) == 0x30)) {
		unsigned char mod = (memory[1] & 0xc0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

		this->modrm = memory[1];
	}

	return size;
}

unsigned long Instruction_push::Encode(unsigned char* memory) {
	memory[0] = 0xff;
	return 1 + SetModRM(this->modrm, this->sib, this->disp32, &memory[1]);
}

unsigned long Instruction_push::GetSize() {
	return 1 + CalcModRM(this->modrm, this->sib);
}

bool Instruction_call::IsValid(unsigned char* memory) {
	return (memory[0] == 0xff) && ((memory[1] & 0x38) == 0x10);
}

unsigned long Instruction_call::Decode(unsigned char* memory) {
	unsigned long size = 0;

	if (memory[0] == 0xff && ((memory[1] & 0x38) == 0x10)) {
		unsigned char mod = (memory[1] & 0xc0) >> 6;
		unsigned char rm = memory[1] & 7;
		unsigned long disp32 = 0;

		size = 1 + GetModRMDisplacement(mod, rm, this->sib, disp32, &memory[2]);

		if (disp32)
			this->address = ((unsigned long long)&memory[size]) + disp32;

		this->modrm = memory[1];
	}

	return size;
}

unsigned long Instruction_call::Encode(unsigned char* memory) {
	memory[0] = 0xff;

	unsigned long size = 0;

	if (address) {
		unsigned long long disp64 = address - ((unsigned long long)memory) + GetSize();
		
		if (disp64 < 0x7fffffff)
			size = 1 + SetModRM(this->modrm, this->sib, disp64, &memory[1]);
		else {
			/*
				mov rax, address
				call rax
			*/
		}
	}else
		size = 1 + SetModRM(this->modrm, this->sib, 0, &memory[1]);

	return size;
}

unsigned long Instruction_call::GetSize() {
	return 1 + CalcModRM(this->modrm, this->sib);
}

bool Instruction_mov::IsValid(unsigned char* memory) {
	return  (memory[0] == 0xa0) ||
			(memory[0] == 0xa1) ||
			(memory[0] == 0xa2) ||
			(memory[0] == 0xa3) ||
			((memory[0] == 0xc7) && ((memory[1] && 0x38) == 0));
}

unsigned long Instruction_mov::Decode(unsigned char* memory) {
	unsigned long size = 0;

	if ((memory[0] == 0xa0) ||
		(memory[0] == 0xa1) ||
		(memory[0] == 0xa2) ||
		(memory[0] == 0xa3))
	{
		this->opcode = memory[0];
		size = 1 + 8;
	}else if ((memory[0] == 0xc7) && ((memory[1] && 0x38) == 0))
	{
		unsigned char mod = (memory[1] & 0xc0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

		this->modrm = memory[1];
	}

	return size;
}