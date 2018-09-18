#include <stdio.h>
#include <Windows.h>

#ifndef CPU_H
	#include "cpu.h"
#endif
#ifndef GFX_H
	#include "gfx.h"
#endif

int main(int argc, char **argv) {
	test_main();
	//setup_input();
	struct cpu cpu_state = initialize_cpu();
	cpu_state = load_game(cpu_state, "spaceinvaders.ch8");

	cpu_state = setup_graphics(cpu_state);

	while (1) {
		// Process one opcode
		cpu_state = emulate_cycle(cpu_state, false);
		
		// Draw if needed
		if (cpu_state.draw_flag)
			printf("");
			cpu_state = draw_graphics(&cpu_state);
			cpu_state.draw_flag = 0;

		// Get keypresses
		//set_keys();

		// Sleep for 1/60 second
		Sleep(16.66667);
	}

	end_gfx(&cpu_state);
	return 0;
}