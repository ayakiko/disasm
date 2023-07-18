#include <windows.h>
#include <stdio.h>

#include <vector>

#include "disasm\disasm.h"

const char* test[] = {
		"Unk",
		"Push",
		"Pop",
		"Call",
		"Mov",
		"Lea",
		"Xor",
		"Sub",
		"Mul",
		"Div"
};

//C7 44 24 28 FE FF FF FF

unsigned char memory[] = {0xC7, 0x44, 0x24, 0x28, 0xFE, 0xFF, 0xFF, 0xFF};

int main(int argc, char** argv) {
	std::vector<Instruction_base> list;

	Instruction_base w;

	w.GetType(memory);
	w.Decode(memory);

	Instruction_mov& test = static_cast<Instruction_mov&>(w);

	printf("modrm %x sib %x disp64 %llx\n", test.modrm, test.sib, test.disp64);

	unsigned char* mem = (unsigned char*)malloc(256);

	w.Encode(mem);

	printf("%llx %llx\n", &memory, mem);

	getchar();

	free(mem);

	return 0;
}