#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned short WORD;
typedef unsigned char BYTE;

typedef struct cpu{
	WORD opcode;
	BYTE V[16];
	WORD I;
	WORD pc;
	BYTE memory[4096];
	BYTE screen[64 * 32];
	BYTE delay_timer;
	BYTE sound_timer;
	WORD stack[16];
	WORD sp;
	_Bool key[16];
	_Bool draw_flag;
};

WORD font_set[80] =
{
	0xF0, 0x90, 0x90, 0x90, 0xF0, //0
	0x20, 0x60, 0x20, 0x20, 0x70, //1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
	0x90, 0x90, 0xF0, 0x10, 0x10, //4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
	0xF0, 0x10, 0x20, 0x40, 0x40, //7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
	0xF0, 0x90, 0xF0, 0x90, 0x90, //A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
	0xF0, 0x80, 0x80, 0x80, 0xF0, //C
	0xE0, 0x90, 0x90, 0x90, 0xE0, //D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
	0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

cpu initialize_cpu(void) {
	struct cpu cpu_state;

	cpu_state.sp = 0;
	cpu_state.pc = 0x200;
	cpu_state.I = 0;
	cpu_state.opcode = 0;

	// Clear display
	for (int i = 0; i < 2048; ++i) {
		cpu_state.display[i] = 0;
	}

	// Clear stack
	for (int i = 0; i < 16; ++i) {
		cpu_state.stack[i] = 0;
	}
	
	// Clear keys
	for (int i = 0; i < 16; ++i) {
		cpu_state.keys[i] = 0;
	}

	// Clear V0-VF
	for (int i = 0; i < 16; ++i) {
		cpu_state.V[i] = 0;
	}

	// Clear memory
	for (int i = 0; i < 4096; ++i) {
		cpu_state.memory[i] = 0;
	}

	// Load fontset
	for (int i = 0; i < 80; i++) {
		memory[i] = chip8_fontset[i];
	}

	// Reset timer
	cpu_state.memory = memset(memory, 0, 4096);
	cpu_state.screen = &cpu_state->memory[0xF00];

	// Clear screen once
	cpu_state.draw_flag = 1;

	// Randomize rand seed
	srand(time(NULL));

	return cpu_state;
}


void emulate_cycle(cpu cpu_state) {
	

	// fetch opcode at pc
	cpu_state.opcode = memory[pc] << 8 | memory[pc + 1];

	// decode and execute opcode
	switch (cpu_state.opcode & 0xF000) {
		case 0x0000:
			switch (cpu_state.opcode & 0x000F) {
				case 0x0000:
					// Clear the screen
					break;
				case 0x000E:
					// Return from a subroutine
					break;
			}
			break;

		case 0x1000:
			// Jump to address NNN
			cpu_state.pc = cpu_state.opcode & 0x0FFF;
			break;

		case 0x2000:
			// Call subroutine at NNN
			cpu_state.stack[sp] = cpu_state.pc;
			++cpu_state.sp;
			pc = cpu_state.opcode & 0x0FFF;
			break;

		case 0x3000:
			// Skip next insruction if VX = NN
			if (cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] == (cpu_state.opcode & 0x00FF))
				cpu_state.pc += 2;
			cpu_state.pc += 2;
			break;
		
		case 0x4000:
			// Skip next insruction if VX != NN
			if (cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] != (cpu_state.opcode & 0x00FF))
				cpu_state.pc += 2;
			cpu_state.pc += 2;
			break;

		case 0x5000:
			// Skip next instruction if VX = VY
			if (cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] == cpu_state.V[cpu_state.opcode & 0x00F0 >> 4])
				cpu_state.pc += 2;
			cpu_state.pc += 2;
			break;

		case 0x6000:
			// Set VX to NN
			cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] = (cpu_state.opcode & 0x00FF);
			cpu_state.pc += 2
			break;

		case 0x7000:
			// Add NN to VX
			cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] += (cpu_state.opcode & 0x00FF);
			cpu_state.pc += 2
			break;

		case 0x8000:
			switch (cpu_state.opcode & 0x000F) {
				case 0x0000:
					// Set VX to VY
					cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] = cpu_state.V[cpu_state.opcode & 0x00F0 >> 4];
					cpu_state.pc += 2;
					break;

				case 0x0001:
					// Set VX to VX | VY
					cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] |= cpu_state.V[cpu_state.opcode & 0x00F0 >> 4]);
					cpu_state.pc += 2;
					break;

				case 0x0002:
					// Set VX to VX & VY
					cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] &= cpu_state.V[cpu_state.opcode & 0x00F0 >> 4]);
					cpu_state.pc += 2;
					break;

				case 0x0003:
					// Set VX to VX xor VY
					cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] ^= V[cpu_state.opcode & 0x00F0 >> 4]);
					cpu_state.pc += 2;
					break;

				case 0x0004:
					// Add VY to VX (and more)
					if (cpu_state.V[(cpu_state.opcode & 0x00F0) >> 4] > (0xFF - cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8]))
						cpu_state.V[0xF] = 1;
					else
						cpu_state.V[0xF] = 0;
					cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] += cpu_state.V[(cpu_state.opcode & 0x00F0) >> 4];
					cpu_state.pc += 2;
					break;

				case 0x0005:
					// Subtrct VY from VX
					if (cpu_state.V[(cpu_state.opcode & 0x00F0) >> 4] > cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8])
						cpu_state.V[0xF] = 0;
					else
						cpu_state.V[0xF] = 1;
					cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] -= cpu_state.V[(cpu_state.opcode & 0x00F0) >> 4];
					cpu_state.pc += 2;
					break;

				case 0x0006:
					// Stores the least significant bit of VX in VF, shifts VX >> 1
					cpu_state.V[0xF] = cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] & 0x1;
					cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] >>= 1;
					cpu_state.pc += 2;
					break;

				case 0x0007:
					// Sets VX to VY - VX (and more)
					if (cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] > cpu_state.V[(cpu_state.opcode & 0x00F0) >> 4])
						cpu_state.V[0xF] = 0;
					else
						cpu_state.V[0xF] = 1;
					cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] = cpu_state.V[(cpu_state.opcode & 0x00F0) >> 4] - cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8];
					cpu_state.pc += 2;
					break;

				case 0x000E:
					// Stores the most significant bit of VX in VF, shifts VX << 1
					cpu_state.V[0xF] = (cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] & 0xF000) >> 7;
					cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] <<= 1;
					cpu_state.pc += 2;
					break;
			}
			break;

		case 0x9000:
			// Skip next instruction if VX != VY
			if (cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] != cpu_state.V[(cpu_state.opcode & 0x00F0) >> 4]);
				cpu_state.pc += 2;
			cpu_state.pc += 2;
			break;

		case 0xA000:
			// Sets I to NNN
			cpu_state.I = (cpu_state.opcode & 0x0FFF);
			cpu_state.pc += 2;
			break;

		case 0xB000:
			// Jump to NNN + V0
			cpu_state.pc = V[0] + (cpu_state.opcode & 0x0FFF);
			break;

		case 0xC000:
			// Set VX to rand_num & NN
			cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (cpu_state.opcode & 0x00FF);
			cpu_state.pc += 2;
			break;

		case 0xD000:
			// Draw a sprite at coord VX, VY with width 8 and height N (and more)
			WORD x = V[(cpu_state.opcode & 0x0F00) >> 8];
			WORD y = V[(cpu_state.opcode & 0x00F0) >> 4];
			WORD height = cpu_state.opcode * 0x000F;
			WORD pixel;

			V[0xF] = 0;
			for (int y_line = 0; y_line < height; ++y_line) {
				pixel = memory[i + y_line];
				for (int x_line = 0; x_line < 8; ++y_line) {
					if(cpu_state.screen[x + x_line + ((y + y_line) * 64)] == 1)
						cpu_state.V[0xF] = 1
					cpu_state.screen[x + x_line + ((y + y_line) * 64)] ^= 1
				}
			}

			cpu_state.draw_flag = 1;
			cpu_state.pc += 2;
			break;

		case 0xE000:
			switch (cpu_state.opcode & 0x00FF) {
				case 0x009E:
					// Skip next instruction if key VX is pressed
					if (key[(cpu_state.opcode & 0x0F00) >> 8] == 0)
						cpu_state.pc += 2;
					cpu_state.pc += 2;
					break;

				case 0x00A1:
					// Skip next instruction if key VX is not pressed
					if (key[(cpu_state.opcode & 0x0F00) >> 8] != 0)
						cpu_state.pc += 2;
					cpu_state.pc += 2;
					break;
			}
			break;

		case 0xF000:
			switch (cpu_state.opcode & 0x00FF) {
				case 0x0007:
					// Set VX to value of delay timer
					cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] = cpu_state.delay_timer;
					cpu_state.pc += 2;
					break;

				case 0x000A:
					// wait for key press, then store in VX
					_Bool key_pressed = 0;
					while (!key_pressed) {
						for (int i = 0; i < 16; ++i) {
							if (cpu_state.key[i]) {
								cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] = cpu_state.key[i];
								key_pressed = 1;
								break;
							}
						}
					}
					cpu_state.pc += 2;
					break;

				case 0x0015:
					// Set the delay timer to VX
					cpu_state.delay_timer = cpu_state.V[(cpu_state.opcode * 0x0F00) >> 8];
					cpu_state.pc += 2;
					break;

				case 0x0018:
					// Set the sound timer to VX
					cpu_state.sound_timer = cpu_state.V[(cpu_state.opcode * 0x0F00) >> 8];
					cpu_state.pc += 2;
					break;

				case 0x001E:
					// Adds VX to I
					if ((cpu_state.I + cpu_state.V[(cpu_state.opcode * 0x0F00) >> 8]) > 0xFFF)
						cpu_state.V[0xF] = 1;
					else
						cpu_state.V[0xF] = 0;
					cpu_state.I += cpu_state.V[(cpu_state.opcode * 0x0F00) >> 8];
					cpu_state.pc += 2;
					break;

				case 0x0029:
					// Set I to the location of the sprite for the char in VX
					cpu_state.I = cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] * 0x5;
					cpu_state.pc += 2;
					break;

				case 0x0033:
					// a lot
					cpu_state.memory[cpu_state.I] = cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] / 100;
					cpu_state.memory[cpu_state.I + 1] = (cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] / 10) % 10;
					cpu_state.memory[cpu_state.I + 2] = (cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] % 100) % 10;
					cpu_state.pc += 2;
					break;

				case 0x0055:
					// Stores V0 to VX (inclusive) in memory starting at address I
					for (int i = 0; i < ((cpu_state.opcode & 0x0F00) >> 8); ++i) {
						cpu_state.memory[cpu_state.I + i] = cpu_state.V[i];
					}
					cpu_state.pc += 2;
					break;

				case 0x0065:
					// Fills V0 to VX (inclusive) with values memory starting at I
					for (int i = 0; i < ((cpu_state.opcode & 0x0F00) >> 8); ++i) {
						 cpu_state.V[i] = cpu_state.memory[cpu_state.I + i];
					}
					cpu_state.pc += 2;
					break;
			}
			break;
		default:
			printf("Unknown opcode\n");
	}

	//update timers
	if (cpu_state.delay_timer > 0)
		--cpu_state.delay_timer;
	
	if (cpu_state.sound_timer > 0) {
		if (cpu_state.sound_timer == 1)
			printf("\a");
		--cpu_state.sound_timer;
	}

	return;
}


void load_game(cpu cpu_state, char[] game_name) {
	// Open file
	FILE *fp;
	fp = fopen(game_name, "rb");

	// Get file size
	fseek(fp, 0, SEEK_END);
	long f_size = ftell(fp);
	rewing(fp);

	// Allocate memory
	char *file_buffer = (char*)malloc(sizeof(char) * f_size);

	// Copy file into buffer
	size_t buffer_size = fread(file_buffer, 1, f_size, fp);

	// Copy buffer into memory
	for (int i = 0; i < buffer_size; ++i) {
		cpu_state.memory[i + 512] = buffer[i];
	}

	// Close file and free buffer memory
	fclose(fp);
	free(file_buffer);

	return;
}


int main(void) {
	/*setup_graphics();
	setup_input();
	*/

	cpu_state = initialize_cpu();
	/*load_game(GameName);*/

	while (1) {
		// Process one opcode
		emulate_cycle(cpu_state);

		// Draw if needed
		if (cpu_state.draw_flag)
			draw_graphics();

		// Get keypresses
		set_keys();

		// Sleep for 1/60 second
		nanosleep(16.66667);
	}

	return 0;
}