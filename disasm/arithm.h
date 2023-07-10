#pragma once

struct Instruction_mul {
	virtual unsigned long Decode(unsigned char *memory);
	virtual unsigned long Encode(unsigned char* memory);
	virtual unsigned long GetSize();

	static bool IsValid(unsigned char* memory);

	unsigned long disp32 = 0;
	unsigned char sib = 0;
	unsigned char modrm = 0;
};

struct Instruction_div {
	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);
	virtual unsigned long GetSize();

	static bool IsValid(unsigned char* memory);

	unsigned long disp32 = 0;
	unsigned char sib = 0;
	unsigned char modrm = 0;
};