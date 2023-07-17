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

	sib = 0;
	disp32 = 0;

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
	if (memory[0] >= 0x40 && memory[0] <= 0x4F)
	{
		this->prefix1 = memory[0];
		this->prefix2 = 0;
		memory++;
	}else if (memory[0] >= 0x64 && memory[0] <= 0x67) {
		this->prefix1 = memory[0];
		memory++;

		if (memory[0] >= 0x40 && memory[0] <= 0x4F) {
			this->prefix2 = memory[0];
			memory++;
		}else
			this->prefix2 = 0;
	}else {
		this->prefix1 = 0;
		this->prefix2 = 0;
	}

	if (Instruction_push::IsValid(memory)) {
		this->type = Type::Push;
	}else if (Instruction_pop::IsValid(memory)) {
		this->type = Type::Pop;
	}else if (Instruction_call::IsValid(memory)) {
		this->type = Type::Call;
	}else if (Instruction_mov::IsValid(memory)) {
		this->type = Type::Mov;
	}else if (Instruction_lea::IsValid(memory)) {
		this->type = Type::Lea;
	}else if (Instruction_xor::IsValid(memory)) {
		this->type = Type::Xor;
	}else if (Instruction_sub::IsValid(memory)) {
		this->type = Type::Sub;
	}else if (Instruction_mul::IsValid(memory)) {
		this->type = Type::Mul;
	}else if (Instruction_div::IsValid(memory)) {
		this->type = Type::Div;
	}else
		this->type = Type::Unk;

	return this->type;
}

unsigned long Instruction_base::Decode(unsigned char* memory) {
	unsigned long size = 0;

	if (this->prefix1) {
		memory++;
		size++;

		if (this->prefix2) {
			memory++;
			size++;
		}
	}

	switch(this->type) {
		case Type::Unk: break;
		case Type::Push:
		{
			size += Instruction_push::Decode(memory);
			break;
		}
		case Type::Pop:
		{
			size += Instruction_pop::Decode(memory);
			break;
		}
		case Type::Call:
		{
			size += Instruction_call::Decode(memory);
			break;
		}
		case Type::Mov:
		{
			size += Instruction_mov::Decode(memory);
			break;
		}
		case Type::Lea:
		{
			size += Instruction_lea::Decode(memory);
			break;
		}
		case Type::Xor:
		{
			size += Instruction_xor::Decode(memory);
			break;
		}
		case Type::Sub:
		{
			size += Instruction_sub::Decode(memory);
			break;
		}
		case Type::Mul:
		{
			size += Instruction_mul::Decode(memory);
			break;
		}
		case Type::Div:
		{
			size += Instruction_div::Decode(memory);
			break;
		}
	}

	return size;
}

unsigned long Instruction_base::Encode(unsigned char* memory) {
	unsigned long size = 0;

	if (this->prefix1) {
		memory[0] = this->prefix1;
		memory++;
		size++;

		if (this->prefix2) {
			memory[0] = this->prefix2;
			memory++;
			size++;
		}
	}

	switch(this->type) {
		case Type::Unk: {
			break;
		}
		case Type::Push:
		{
			size += Instruction_push::Encode(memory);
			break;
		}
		case Type::Pop:
		{
			size += Instruction_pop::Encode(memory);
			break;
		}
		case Type::Call:
		{
			size += Instruction_call::Encode(memory);
			break;
		}
		case Type::Mov:
		{
			size += Instruction_mov::Encode(memory);
			break;
		}
		case Type::Lea:
		{
			size += Instruction_lea::Encode(memory);
			break;
		}
		case Type::Xor:
		{
			size += Instruction_xor::Encode(memory);
			break;
		}
		case Type::Sub:
		{
			size += Instruction_sub::Encode(memory);
			break;
		}
		case Type::Mul:
		{
			size += Instruction_mul::Encode(memory);
			break;
		}
		case Type::Div:
		{
			size += Instruction_div::Encode(memory);
			break;
		}
	}

	return size;
}