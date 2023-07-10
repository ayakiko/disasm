struct Instruction_push {
	virtual unsigned long Decode(unsigned char* memory);
	virtual unsigned long Encode(unsigned char* memory);
	virtual unsigned long GetSize(unsigned char* memory);

	static bool IsValid(unsigned char* memory);

	unsigned char modrm = 0;
	unsigned char sib = 0;
	unsigned long disp32 = 0;
};