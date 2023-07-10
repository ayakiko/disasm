#include "disasm.h"

bool Instruction_push::IsValid(unsigned char* memory) {
	return (memory[0] == 0xff) && ((memory[1] & 0x38) == 0x30);
}

unsigned long Instruction_push::Decode(unsigned char* memory) {
	unsigned long size = 0;

	if (memory[0] == 0xff && ((memory[1] & 0x38) == 0x30)) {
		unsigned char mod = (memory[1] & 0xc0) >> 6;
		unsigned char rm = memory[1] & 7;
		unsigned char sib = 0;

		size = 1 + GetModRMDisplacement(mod, rm, sib, this->disp32, &memory[2]);

		this->modrm = memory[1];
		this->sib = sib;
	}

	return size;
}

unsigned long Instruction_push::Encode(unsigned char* memory) {
	memory[0] = 0xff;
	return 1 + SetModRM(this->modrm, this->sib, this->disp32, &memory[1]);
}

unsigned long Instruction_push::GetSize(unsigned char*) {
	return 1 + CalcModRM(this->modrm, this->sib);
}