#include "disasm.h"

bool Instruction_sub::IsValid(unsigned char* memory) {
	return (memory[0] >= 0x28 && memory[0] <= 0x2D) ||
			((memory[0] == 0x80 || memory[0] == 0x81 || memory[0] == 0x83) && ((memory[1] & 0x38) == 0x28));
}

unsigned long Instruction_sub::Decode(unsigned char* memory) {
	unsigned long size = 0;

	if (memory[0] == 0x28 || memory[0] == 0x2A) /* 8 bit */
	{
		unsigned char mod = (memory[1] & 0xC0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

		this->opcode = memory[0];
		this->modrm = memory[1];
	}else if (memory[0] == 0x29 || memory[0] == 0x2B) /* 16/32/64 bit */
	{
		unsigned char mod = (memory[1] & 0xC0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

		this->opcode = memory[0];
		this->modrm = memory[1];
	}else if (memory[0] == 0x2C) /* 8 bit */
	{
		this->opcode = 0x2C;
		this->disp32 = (unsigned long)memory[1];

		size = 1 + 1;
	}else if (memory[0] == 0x2D) /* 16/32 bit but precision override prefix expected*/
	{
		this->opcode = 0x2D;
		this->disp32 = *(unsigned long*)&memory[1];

		size = 1 + 4;
	}else if (memory[0] == 0x80 || memory[0] == 0x83) {
		unsigned char ext = (memory[1] & 0x38) >> 3;

		if (ext == 5) {
			unsigned char mod = (memory[1] & 0xC0) >> 6;
			unsigned char rm = memory[1] & 7;

			size = 1 + 1 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

			this->opcode = memory[0];
			this->modrm = memory[1];
			this->operand = (unsigned char)memory[size - 1];
		}
	}else if (memory[0] == 0x81) {
		unsigned char ext = (memory[1] & 0x38) >> 3;

		if (ext == 5) {
			unsigned char mod = (memory[1] & 0xC0) >> 6;
			unsigned char rm = memory[1] & 7;

			size = 1 + 4 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

			this->opcode = memory[0];
			this->modrm = memory[1];
			this->operand = *(unsigned long*)&memory[size - 4];
		}
	}

	return size;
}

unsigned long Instruction_sub::Encode(unsigned char* memory) {
	unsigned long size = 1;

	memory[0] = this->opcode;

	if (this->opcode == 0x28 || this->opcode == 0x2A ||
		this->opcode == 0x29 || this->opcode == 0x2B) {
		size += SetModRM(this->modrm, this->sib, this->disp32, &memory[1]);
	}else if (this->opcode == 0x2C) {
		memory[1] = (unsigned char)this->disp32;
		size += 1;
	}else if (this->opcode == 0x2D) {
		*(unsigned long*)&memory[1] = this->disp32;
		size += 4;
	}else if (this->opcode == 0x80 || this->opcode == 0x83) {
		size += 1 + SetModRM(this->modrm, this->sib, this->disp32, &memory[1]);
		memory[size - 1] = (unsigned char)this->operand;
	}else if (this->opcode == 0x81) {
		size += 4 + SetModRM(this->modrm, this->sib, this->disp32, &memory[1]);
		*(unsigned long*)&memory[size - 4] = this->operand;
	}

	return size;
}

unsigned long Instruction_sub::GetSize(unsigned char* memory) {
	if (this->opcode == 0x28 || this->opcode == 0x2A ||
		this->opcode == 0x29 || this->opcode == 0x2B) {
		return 1 + CalcModRM(this->modrm, this->sib);
	}else if (this->opcode == 0x80 || this->opcode == 0x81) {
		return 1 + 1 + CalcModRM(this->modrm, this->sib);
	}else if (this->opcode == 0x81) {
		return 1 + 4 + CalcModRM(this->modrm, this->sib);
	}else if (this->opcode == 0x2D) {
		return 1 + 4;
	}

	return 1 + 1;
}

bool Instruction_mul::IsValid(unsigned char* memory) {
	return 0;
}

unsigned long Instruction_mul::Decode(unsigned char* memory) {
	return 0;
}

unsigned long Instruction_mul::Encode(unsigned char* memory) {
	return 0;
}

unsigned long Instruction_mul::GetSize(unsigned char* memory) {
	return 0;
}

bool Instruction_div::IsValid(unsigned char* memory) {
	return 0;
}

unsigned long Instruction_div::Decode(unsigned char* memory) {
	return 0;
}

unsigned long Instruction_div::Encode(unsigned char* memory) {
	return 0;
}

unsigned long Instruction_div::GetSize(unsigned char* memory) {
	return 0;
}