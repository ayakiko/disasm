#include <windows.h>
#include <stdio.h>

#include <vector>

#include "disasm\disasm.h"

const char *test = "\xf7\xB4\x00\x00\x00\x00\x81\xf7\xA4\x00\x00\x00\x00\x81\xa4\xa4\x64\xf7\xA4\x00\x00\x00\x13\x12";

int main(int argc, char** argv) {
	std::vector<Instruction_base> list;

	Instruction_base w;
	
	w.GetType((unsigned char*)test);
	w.Decode((unsigned char*)test);

	if (w.type == Instruction_base::Type::Div) {
		printf("div!\n");
	}

	return 0;
}