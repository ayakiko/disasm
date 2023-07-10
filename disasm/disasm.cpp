#include <stdio.h>

#include "disasm.h"

unsigned long GetSibDisplacement(const unsigned char mod, unsigned char sib, unsigned long &disp32, unsigned char* mem) {
	unsigned long size = 0;

	unsigned char i = (sib & 0x38) >> 3;
	unsigned char b = sib & 7;

	switch (mod) {
		case 0:
			if (b == 5) {
				disp32 = *(unsigned long*)mem;
				size = 4;
			}

			break;
		case 1:
			disp32 = (unsigned long)(*mem);
			size = 1;

			break;
		case 2:
			disp32 = *(unsigned long*)mem;
			size = 4;

			break;
	}

	return 1 + size;
}

unsigned long GetModRMDisplacement(unsigned char mod, unsigned char rm, unsigned char &sib, unsigned long &disp32, unsigned char* mem) {
	unsigned long size = 0;

	switch (mod) {
		case 0:
			if (rm == 4) {
				size = GetSibDisplacement(0, mem[0], disp32, (unsigned char*)&mem[1]);
				sib = mem[0];
			}else if (rm == 5) {
				disp32 = *(unsigned long*)mem;
				size = 4;
			}
			break;
		case 1:
			if (rm == 4) {
				size = GetSibDisplacement(1, mem[0], disp32, (unsigned char*)&mem[1]);
				sib = mem[0];
			}else {
				disp32 = mem[0];
				size = 1;
			}
			break;
		case 2:
			if (rm == 4) {
				size = GetSibDisplacement(2, mem[0], disp32, (unsigned char*)&mem[1]);
				sib = mem[0];
			}else {
				disp32 = *(unsigned long*)mem;
				size = 4;
			}
			break;
		case 3: break; /*no displacement in direct register access mode*/
	}

	return 1 + size;
}

unsigned long SetSib(const unsigned char mod, unsigned char sib, unsigned long disp32, unsigned char* mem) {
	unsigned long size = 1;

	unsigned char i = (sib & 0x38) >> 3;
	unsigned char b = (sib & 7);

	mem[0] = sib;

	switch (mod) {
		case 0:
			if (b == 5) {
				*(unsigned long*)(&mem[1]) = disp32;
				size += 4;
			}

			break;
		case 1:
			mem[1] = (unsigned char)disp32;
			size++;

			break;
		case 2:
			*(unsigned long*)(&mem[1]) = disp32;
			size += 4;

			break;
	}

	return size;
}

unsigned long SetModRM(unsigned char modrm, unsigned char sib, unsigned long disp32, unsigned char* mem) {
	unsigned long size = 1;

	unsigned char mod = (modrm & 0xc0) >> 6;
	unsigned char rm = modrm & 7;

	mem[0] = modrm;

	switch (mod) {
		case 0:
			if (rm == 5) {
				*(unsigned long*)(&mem[1]) = disp32;
				size += 4;
			}else if (rm == 4)
				size += SetSib(0, sib, disp32, &mem[1]);

			break;
		case 1:
			if (rm == 4) {
				size += SetSib(1, sib, disp32, &mem[1]);
			}else {
				mem[1] = (unsigned char)disp32;
				size++;
			}

			break;
		case 2:
			if (rm == 4) {
				size += SetSib(2, sib, disp32, &mem[1]);
			}else {
				*(unsigned long*)(&mem[1]) = disp32;
				size += 4;
			}

			break;
		case 3: break; /*no displacement in direct register access mode*/
	}

	return size;
}

unsigned long CalcSib(const unsigned char mod, unsigned char sib) {
	unsigned long size = 1;

	unsigned char i = (sib & 0x38) >> 3;
	unsigned char b = (sib & 7);

	switch (mod) {
		case 0:
			if (b == 5)
				size += 4;

			break;
		case 1:
			size++;

			break;
		case 2:
			size += 4;

			break;
	}

	return size;
}

unsigned long CalcModRM(unsigned char modrm, unsigned char sib) {
	unsigned long size = 1;

	unsigned char mod = (modrm & 0xc0) >> 6;
	unsigned char rm = modrm & 7;

	switch (mod) {
		case 0:
			if (rm == 5) {
				size += 4;
			}else if (rm == 4)
				size += CalcSib(0, sib);

			break;
		case 1:
			if (rm == 4) {
				size += CalcSib(1, sib);
			}else
				size++;

			break;
		case 2:
			if (rm == 4) {
				size += CalcSib(2, sib);
			}else
				size += 4;

			break;
		case 3: break; /*no displacement in direct register access mode*/
	}

	return size;
}

Instruction_base::Type Instruction_base::GetType(unsigned char* memory) {
	if (Instruction_mul::IsValid(memory)) {
		this->type = Type::Mul;
	}else if (Instruction_div::IsValid(memory)) {
		this->type = Type::Div;
	}else if (Instruction_push::IsValid(memory)) {
		this->type = Type::Push;
	}

	return this->type;
}

unsigned long Instruction_base::Decode(unsigned char* memory) {
	unsigned long size = 0;

	switch(this->type) {
		case Type::Unk: {
			break;
		}
		case Type::Mul:
		{
			size = Instruction_mul::Decode(memory);
			break;
		}
		case Type::Div:
		{
			size = Instruction_div::Decode(memory);
			break;
		}
		case Type::Push:
		{
			size = Instruction_push::Decode(memory);
			break;
		}
		case Type::Call:
		{
			size = Instruction_call::Decode(memory);
			break;
		}
	}

	return size;
}

unsigned long Instruction_base::Encode(unsigned char* memory) {
	unsigned long size = 0;

	switch(this->type) {
		case Type::Unk: {
			break;
		}
		case Type::Mul:
		{
			size = Instruction_mul::Encode(memory);
			break;
		}
		case Type::Div:
		{
			size = Instruction_div::Encode(memory);
			break;
		}
		case Type::Push:
		{
			size = Instruction_push::Encode(memory);
			break;
		}
		case Type::Call:
		{
			size = Instruction_call::Encode(memory);
			break;
		}
	}

	return size;
}