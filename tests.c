#include <stdbool.h>
#include <assert.h>
#ifndef CPU_H
	#include "cpu.h"
#endif


bool test_0x0000() {
	struct cpu c = initialize_cpu();
	c.opcode = 0x0000;

	// Set up test
	for (int i = 0; i < 2048; ++i) {
		c.screen[i] = 1;
	}
	c = emulate_cycle(c, true);
	
	// Check tests
	bool pass = true;
	for (int i = 0; i < 2048; ++i) {
		if (c.screen[i] != 0) 
			pass = false;
	}
	if ((c.draw_flag != 1) || (c.pc != 0x200 + 2))
		pass = false;

	return pass;
}

bool test_0x000E() {
	struct cpu c = initialize_cpu();
	c.opcode = 0x000E;

	// Set up test
	c.stack[2] = 0x300;
	c.sp = 2;
	c = emulate_cycle(c, true);

	// Check tests
	bool pass = true;
	if ((c.sp != 1) || (c.pc != 0x300 + 2))
		pass = false;

	return pass;
}

int test_main() {
	bool pass;

	// 0x0000
	pass = test_0x0000();
	assert(pass);

	// 0x000E
	pass = test_0x000E();
	assert(pass);

	return 0;
}