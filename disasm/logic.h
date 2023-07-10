struct Instruction_push {
	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);
	virtual unsigned long GetSize();

	static bool IsValid(unsigned char* memory);

	unsigned char modrm = 0;
	unsigned char sib = 0;
	unsigned long disp32 = 0;
};

struct Instruction_call {
	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);
	virtual unsigned long GetSize();

	static bool IsValid(unsigned char* memory);

	unsigned char modrm = 0;
	unsigned char sib = 0;
	unsigned long long address = 0;
};

struct Instruction_mov {
	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);
	virtual unsigned long GetSize();

	static bool IsValid(unsigned char* memory);

	unsigned char opcode = 0;
	unsigned char modrm = 0;
	unsigned char sib = 0;
	unsigned long disp32 = 0;
};