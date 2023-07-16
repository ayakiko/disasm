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

int main(int argc, char** argv) {
	std::vector<Instruction_base> list;

	Instruction_base w;

	for (int i = 0; i < 256; i++) {
		unsigned long size = 1;
		unsigned char* addr = (unsigned char*)(((unsigned long long)&main) + i);

		w.GetType(addr);

		if (w.type != Instruction_base::Type::Unk) {
			size = w.Decode(addr);
			list.push_back(w);
			printf("%x %s? %llx %x\n", w.prefix1, test[w.type], addr, *addr);
		}

		i += size - 1;
	}

	unsigned char* mem = (unsigned char*)malloc(512);
	unsigned long size = 0;

	for (auto e : list) {
		size += e.Encode(&mem[size]);
	}

	printf("%llx\n", mem);

	getchar();
	free(mem);
	return 0;
}