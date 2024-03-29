#include "disasm.h"

bool Instruction_push::IsValid(unsigned char* memory) {
	return ((memory[0] >= 0x50) && (memory[0] <= 0x57)) ||
			((memory[0] == 0x68) /*16/32 bit*/ || (memory[0] == 0x6A /*8 bit*/)) ||
			((memory[0] == 0xff) && ((memory[1] & 0x38) == 0));
}

unsigned long Instruction_push::Decode(unsigned char* memory) {
	unsigned long size = 1;

	if ((memory[0] >= 0x50) && (memory[0] <= 0x57)) {
		this->opcode = memory[0];
	}else if (memory[0] == 0x68) {
		this->opcode = memory[0];
		this->disp32 = *(unsigned long*)(&memory[1]);

		size += 4;
	}else if (memory[0] == 0x6A) {
		this->opcode = memory[0];
		this->disp32 = (unsigned long)memory[1];

		size++;
	}else if ((memory[0] == 0xff) && ((memory[1] & 0x38) == 0)) {
		unsigned char mod = (memory[1] & 0xc0) >> 6;
		unsigned char rm = memory[1] & 7;

		size += GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

		this->opcode = memory[0];
		this->modrm = memory[1];
	}

	return size;
}

unsigned long Instruction_push::Encode(unsigned char* memory) {
	unsigned long size = 1;

	memory[0] = this->opcode;

	if (this->opcode == 0x68) {
		*(unsigned long*)(&memory[1]) = this->disp32;

		size += 4;
	}else if (this->opcode == 0x6A) {
		memory[1] = (unsigned char)this->disp32;

		size++;
	}else if (this->opcode == 0xff) {
		size += SetModRM(this->modrm, this->sib, this->disp32, &memory[1]);
	}

	return size;
}

unsigned long Instruction_push::GetSize(unsigned char* memory) {
	if (this->opcode == 0xff) {
		return 1 + CalcModRM(this->modrm, this->sib);
	}else if (this->opcode == 0x6A) {
		return 1 + 1;
	}else if (this->opcode == 0x68) {
		return 1 + 4;
	}else
		return 1;
}

bool Instruction_pop::IsValid(unsigned char* memory) {
	return (memory[0] >= 0x58 && memory[0] <= 0x5f) ||
			(memory[0] == 0x8F);
}

unsigned long Instruction_pop::Decode(unsigned char* memory) {
	unsigned long size = 0;

	if (memory[0] >= 0x58 && memory[0] <= 0x5f) {
		this->opcode = memory[0];
		size = 1;
	}else if (memory[0] == 0x8F) {
		unsigned char mod = (memory[1] & 0xC0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

		this->opcode = 0x8F;
		this->modrm = memory[1];
	}

	return size;
}

unsigned long Instruction_pop::Encode(unsigned char* memory) {
	unsigned long size = 1;

	if (this->opcode >= 0x58 && this->opcode <= 0x5F) {
		memory[0] = this->opcode;
	}else if (this->opcode == 0x8F) {
		memory[0] = 0x8F;
		size += SetModRM(this->modrm, this->sib, this->disp32, &memory[1]);
	}

	return size;
}

unsigned long Instruction_pop::GetSize(unsigned char* memory) {
	if (this->opcode == 0x8F)
		return 1 + CalcModRM(this->modrm, this->sib);
	else
		return 1;
}

bool Instruction_call::IsValid(unsigned char* memory) {
	return ((memory[0] == 0xFF) && ((memory[1] & 0x38) == 0x10 || (memory[1] & 0x38) == 0x18)) ||
			(memory[0] == 0xE8);
}

unsigned long Instruction_call::Decode(unsigned char* memory) {
	unsigned long size = 0;

	if (memory[0] == 0xFF) {
		unsigned char ext = (memory[1] & 0x38) >> 3;

		if (ext == 2 || ext == 3) {
			unsigned char mod = (memory[1] & 0xC0) >> 6;
			unsigned char rm = memory[1] & 7;

			size = 1 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

			unsigned char i = (this->sib & 0x38) >> 3;
			unsigned char b = this->sib & 7;

			if (rm == 5) {
				//eip + disp32
				this->address = ((unsigned long long)&memory[size]) + this->disp32;
			}

			this->opcode = memory[0];
		}
	}else if (memory[0] == 0xE8) {
		size = 1 + 4;

		this->opcode = memory[0];
		this->address = ((unsigned long long)&memory[size]) + *(unsigned long*)&memory[1];
	}

	return size;
}

unsigned long Instruction_call::Encode(unsigned char* memory) {
	unsigned long size = 1;

	memory[0] = this->opcode;

	if (this->opcode == 0xFF) {
		size += SetModRM(this->modrm, this->sib, this->disp32, &memory[1]);

		unsigned char rm = this->modrm & 7;

		if (rm == 5) {
			signed long long offset = (signed long long)this->address - (((signed long long)&memory[0]) + size);

			if (offset > 0x7fffffff || offset < -0x7fffffff) {
				//call eip+2 jmp eip+8 address64

				*(unsigned long*)&memory[2] = 1 + 4;
				memory[6] = 0xE9;
				*(unsigned long*)&memory[7] = 8;
				*(unsigned long long*)&memory[11] = this->address;

				printf("%llx\n", this->address);

				size += 1 + 4 + 8;
			}
		}
	}else if (this->opcode == 0xE8) {
		signed long long offset = ((signed long long)this->address) - (((signed long long)&memory[0]) + 5);

		if (offset > 0x7fffffff || offset < -0x7fffffff) {
			//call eip+2 jmp eip+8 address64

			*(unsigned short*)&memory[0] = 0x1DFF;
			*(unsigned long*)&memory[2] = 1 + 4;
			memory[6] = 0xE9;
			*(unsigned long*)&memory[7] = 8;
			*(unsigned long long*)&memory[11] = this->address;

			size += 1 + 4 + 1 + 4 + 8;
		}else {
			*(unsigned long*)&memory[1] = offset;
			size += 5;
		}
	}

	return size;
}

unsigned long Instruction_call::GetSize(unsigned char* memory) {
	unsigned long size = 1;

	if (this->opcode == 0xFF) {
		unsigned char rm = this->modrm & 7;
		size += CalcModRM(this->modrm, this->sib);

		if (rm == 5) {
			signed long long offset = (signed long long)this->address - (((signed long long)&memory[0]) + size);


			if (offset > 0x7fffffff || offset < -0x7fffffff)
				size += 1 + 4 + 8;
		}
	}else {
		signed long long offset = (signed long long)this->address - (((signed long long)&memory[0]) + 5);

		if (offset > 0x7fffffff || offset < -0x7fffffff)
			size += 1 + 4 + 1 + 4 + 8;
		else
			size += 4;
	}

	return size;
}

bool Instruction_mov::IsValid(unsigned char* memory) {
	return (memory[0] >= 0xB0 && memory[0] <= 0xB7) ||
			(memory[0] >= 0xB8 && memory[0] <= 0xBF) ||
			((memory[0] == 0xC6) || (memory[0] == 0xC7)) ||
			(memory[0] >= 0xA0 && memory[0] <= 0xA3) ||
			(memory[0] >= 0x88 && memory[0] <= 0x8C) ||
			(memory[0] == 0x8E); /*8E and 8C is SREG mov operation*/
}

unsigned long Instruction_mov::Decode(unsigned char* memory) {
	unsigned long size = 0;

	if (memory[0] >= 0xB0 && memory[0] <= 0xB7) {
		this->opcode = memory[0];
		size = 1;
	}else if (memory[0] >= 0xB8 && memory[0] <= 0xBF) {
		this->opcode = memory[0];
		size = 1;
	}else if (memory[0] == 0xC6 && (memory[1] & 0x38) == 0) {
		this->opcode = memory[0];

		unsigned char mod = (memory[1] & 0xC0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + 1 + GetModRMDisplacement(mod, rm, this->sib, (unsigned long&)this->disp64, &memory[2]);

		this->operand = memory[size - 1];
		this->modrm = memory[1];
	}else if (memory[0] == 0xC7 && (memory[1] & 0x38) == 0) {
		this->opcode = memory[0];

		unsigned char mod = (memory[1] & 0xC0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + 4 + GetModRMDisplacement(mod, rm, this->sib, (unsigned long&)this->disp64, &memory[2]);

		this->operand = *(unsigned long*)&memory[size - 4];
		this->modrm = memory[1];
	}else if (memory[0] == 0xA0 || memory[0] == 0xA2) {
		this->opcode = memory[0];
		this->disp64 = (unsigned long long)memory[1];

		size = 1 + 1;
	}else if (memory[0] == 0xA1 || memory[0] == 0xA3) {
		this->opcode = memory[0];
		this->disp64 = *(unsigned long long*)&memory[1];

		size = 1 + 8;
	}else if (memory[0] == 0x88 || memory[0] == 0x89 || memory[0] == 0x8A || memory[0] == 0x8B) {
		unsigned char mod = (memory[1] & 0xC0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + GetModRMDisplacement(mod, rm, this->sib, (unsigned long&)this->disp64, &memory[2]);

		this->opcode = memory[0];
		this->modrm = memory[1];
	}else if (memory[0] == 0x8C || memory[0] == 0x8E) {
		unsigned char mod = (memory[1] & 0xC0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + GetModRMDisplacement(mod, rm, this->sib, (unsigned long&)this->disp64, &memory[2]);

		this->opcode = memory[0];
		this->modrm = memory[1];
	}

	return size;
}

unsigned long Instruction_mov::Encode(unsigned char* memory) {
	unsigned long size = 1;

	memory[0] = this->opcode;

	if (this->opcode == 0xA0 || this->opcode == 0xA2) {
		memory[1] = (unsigned char)this->disp64;
		size++;
	}else if (this->opcode == 0xA1 || this->opcode == 0xA3) {
		*(unsigned long long*)(&memory[1]) = this->disp64;
		size += 8;
	}else if (this->opcode == 0x88 || this->opcode == 0x89 || this->opcode == 0x8A || this->opcode == 0x8B) {
		size += SetModRM(this->modrm, this->sib, (unsigned long)this->disp64, &memory[1]); //MOV 8/16/32 (ModRM, Sib)
	}else if (this->opcode == 0x8C || this->opcode == 0x8E) {
		size += SetModRM(this->modrm, this->sib, (unsigned long)this->disp64, &memory[1]); //SREG 16 (Sib)
	}else if (this->opcode == 0xC6) {
		size += 1 + SetModRM(this->modrm, this->sib, (unsigned long)this->disp64, &memory[1]);
		memory[size - 1] = this->operand;
	}else if (this->opcode == 0xC7) {
		size += 4 + SetModRM(this->modrm, this->sib, (unsigned long)this->disp64, &memory[1]);
		*(unsigned long*)&memory[size - 4] = this->operand;
	}

	return size;
}

unsigned long Instruction_mov::GetSize(unsigned char* memory) {
	if (this->opcode == 0xA0 || this->opcode == 0xA2)
		return 1 + 1;
	else if (this->opcode == 0xA1 || this->opcode == 0xA3)
		return 1 + 8;
	else if (this->opcode == 0x88 || this->opcode == 0x89 || this->opcode == 0x8A || this->opcode == 0x8B)
		return 1 + CalcModRM(this->modrm, this->sib);
	else if (this->opcode == 0x8C || this->opcode == 0x8E)
		return 1 + CalcModRM(this->modrm, this->sib);
	else if (this->opcode == 0xC6)
		return 1 + 1 + CalcModRM(this->modrm, this->sib);
	else if (this->opcode == 0xC7)
		return 1 + 4 + CalcModRM(this->modrm, this->sib);
	else
		return 1;
}

bool Instruction_lea::IsValid(unsigned char* memory) {
	return (memory[0] == 0x8D);
}

unsigned long Instruction_lea::Decode(unsigned char* memory) {
	unsigned long size = 0;

	if (memory[0] == 0x8D) {
		unsigned char mod = (memory[1] & 0xC0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

		this->opcode = 0x8D;
		this->modrm = memory[1];
	}

	return size;
}

/*
	Commonly it use 32 bit displacement
	Encode it to 64 bit offset
*/

unsigned long Instruction_lea::Encode(unsigned char* memory) {
	memory[0] = this->opcode;
	return 1 + SetModRM(this->modrm, this->sib, this->disp32, &memory[1]);
}

unsigned long Instruction_lea::GetSize(unsigned char* memory) {
	return 1 + CalcModRM(this->modrm, this->sib);
}

bool Instruction_xor::IsValid(unsigned char* memory) {
	return (memory[0] >= 0x30 && memory[0] <= 0x35) ||
			((memory[0] == 0x80 || memory[0] == 0x81 || memory[0] == 0x83) && (memory[1] & 0x38) == 0x30);
}

unsigned long Instruction_xor::Decode(unsigned char* memory) {
	unsigned long size = 0;

	if (memory[0] == 0x30 || memory[0] == 0x32) /* 8 bit r/m */
	{
		unsigned char mod = (memory[1] & 0xC0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

		this->opcode = memory[0];
		this->modrm = memory[1];
	}else if (memory[0] == 0x31 || memory[0] == 0x33) {
		unsigned char mod = (memory[1] & 0xC0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

		this->opcode = memory[0];
		this->modrm = memory[1];
	}else if (memory[0] == 0x34) {
		this->opcode = 0x34;
		this->operand = (unsigned long)memory[1];

		size = 1 + 1;
	}else if (memory[0] == 0x35) {
		this->opcode = 0x35;
		this->operand = *(unsigned long*)&memory[1];

		size = 1 + 4;
	}else if ((memory[0] == 0x80 || memory[0] == 0x83) && (memory[1] & 0x38) == 0x30) {
		unsigned char mod = (memory[1] & 0xC0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + 1 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

		this->opcode = memory[0];
		this->modrm = memory[1];
		this->operand = (unsigned long)memory[size - 1];
	}else if ((memory[0] == 0x81) && (memory[1] & 0x38) == 0x30) {
		unsigned char mod = (memory[1] & 0xC0) >> 6;
		unsigned char rm = memory[1] & 7;

		size = 1 + 4 + GetModRMDisplacement(mod, rm, this->sib, this->disp32, &memory[2]);

		this->opcode = memory[0];
		this->modrm = memory[1];
		this->operand = *(unsigned long*)&memory[size - 4];
	}

	return size;
}

unsigned long Instruction_xor::Encode(unsigned char* memory) {
	unsigned long size = 1;

	memory[0] = this->opcode;

	if (this->opcode == 0x30 || this->opcode == 0x32 ||
		this->opcode == 0x31 || this->opcode == 0x33) {
		size += SetModRM(this->modrm, this->sib, this->disp32, &memory[1]);
	}else if (this->opcode == 0x80 || this->opcode == 0x83) {
		size += 1 + SetModRM(this->modrm, this->sib, this->disp32, &memory[1]);
		memory[size - 1] = (unsigned char)this->operand;
	}else if (this->opcode == 0x81) {
		size += 4 + SetModRM(this->modrm, this->sib, this->disp32, &memory[1]);
		*(unsigned long*)&memory[size - 4] = this->operand;
	}else if (this->opcode == 0x34) {
		memory[1] = (unsigned char)this->operand;
		size += 1;
	}else if (this->opcode == 0x35) {
		*(unsigned long*)&memory[1] = this->operand;
		size += 4;
	}

	return size;
}

unsigned long Instruction_xor::GetSize(unsigned char* memory) {
	if (this->opcode == 0x30 || this->opcode == 0x32 ||
		this->opcode == 0x31 || this->opcode == 0x33) {
		return 1 + CalcModRM(this->modrm, this->sib);
	}else if (this->opcode == 0x80 || this->opcode == 0x83) {
		return 1 + 1 + CalcModRM(this->modrm, this->sib);
	}else if (this->opcode == 0x81) {
		return 1 + 4 + CalcModRM(this->modrm, this->sib);
	}else if (this->opcode == 0x35) {
		return 1 + 4;
	}

	return 1 + 1;
}