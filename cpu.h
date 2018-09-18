#ifndef CPU_H
	#define CPU_H
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "SDL.h"

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
	bool keys[16];
	bool draw_flag;

	//graphics
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Rect rect;
} cpu;

static const BYTE FONT_SET[80] =
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

inline struct cpu initialize_cpu(void) {
	struct cpu cpu_state;

	cpu_state.sp = 0;
	cpu_state.pc = 0x200;
	cpu_state.I = 0;
	cpu_state.opcode = 0;

	// Clear display
	for (int i = 0; i < 2048; ++i) {
		cpu_state.screen[i] = 0;
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
		cpu_state.memory[i] = FONT_SET[i];
	}

	// Reset timer
	cpu_state.delay_timer = 0;
	cpu_state.sound_timer = 0;

	// Clear screen once
	cpu_state.draw_flag = true;

	// Randomize rand seed
	srand(time(NULL));

	return cpu_state;
}


inline cpu emulate_cycle(struct cpu cpu_state, bool opcode_provided) {
	// fetch opcode at pc
	if (!opcode_provided)
		cpu_state.opcode = cpu_state.memory[cpu_state.pc] << 8 | cpu_state.memory[cpu_state.pc + 1];

	//printf("%X at %X\n", cpu_state.opcode, cpu_state.pc);

	// decode and execute opcode
	switch (cpu_state.opcode & 0xF000) {
	case 0x0000:
		switch (cpu_state.opcode & 0x000F) {
		case 0x0000:
			// Clear the screen
			for (int i = 0; i < 2048; ++i) {
				cpu_state.screen[i] = 0;
			}
			cpu_state.draw_flag = 1;
			cpu_state.pc += 2;
			break;

		case 0x000E:
			// Return from a subroutine
			cpu_state.pc = cpu_state.stack[cpu_state.sp];
			--cpu_state.sp;
			cpu_state.pc += 2;
			break;
		}
		break;

	case 0x1000:
		// Jump to address NNN
		cpu_state.pc = cpu_state.opcode & 0x0FFF;
		break;

	case 0x2000:
		// Call subroutine at NNN
		++cpu_state.sp;
		cpu_state.stack[cpu_state.sp] = cpu_state.pc;
		cpu_state.pc = cpu_state.opcode & 0x0FFF;
		break;

	case 0x3000:
		// Skip next insruction if VX = NN
		if (cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] == (cpu_state.opcode & 0x00FF))
			cpu_state.pc += 4;
		else
			cpu_state.pc += 2;
		break;

	case 0x4000:
		// Skip next insruction if VX != NN
		if (cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] != (cpu_state.opcode & 0x00FF))
			cpu_state.pc += 4;
		else
			cpu_state.pc += 2;
		break;

	case 0x5000:
		// Skip next instruction if VX = VY
		if (cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] == cpu_state.V[cpu_state.opcode & 0x00F0 >> 4])
			cpu_state.pc += 4;
		else
			cpu_state.pc += 2;
		break;

	case 0x6000:
		// Set VX to NN
		cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] = (cpu_state.opcode & 0x00FF);
		cpu_state.pc += 2;
		break;

	case 0x7000:
		// Add NN to VX
		cpu_state.V[cpu_state.opcode & 0x0F00 >> 8] += (cpu_state.opcode & 0x00FF);
		cpu_state.pc += 2;
		break;

	case 0x8000:
		switch (cpu_state.opcode & 0x000F) {
		case 0x0000:
			// Set VX to VY
			cpu_state.V[(cpu_state.opcode) & 0x0F00 >> 8] = cpu_state.V[(cpu_state.opcode & 0x00F0) >> 4];
			cpu_state.pc += 2;
			break;

		case 0x0001:
			// Set VX to VX | VY
			cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] |= cpu_state.V[(cpu_state.opcode) & 0x00F0 >> 4];
			cpu_state.pc += 2;
			break;

		case 0x0002:
			// Set VX to VX & VY
			cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] &= cpu_state.V[(cpu_state.opcode & 0x00F0) >> 4];
			cpu_state.pc += 2;
			break;

		case 0x0003:
			// Set VX to VX xor VY
			cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] ^= cpu_state.V[(cpu_state.opcode & 0x00F0) >> 4];
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
		if (cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] != cpu_state.V[(cpu_state.opcode & 0x00F0) >> 4])
			cpu_state.pc += 4;
		else
			cpu_state.pc += 2;
		break;

	case 0xA000:
		// Sets I to NNN
		cpu_state.I = (cpu_state.opcode & 0x0FFF);
		cpu_state.pc += 2;
		break;

	case 0xB000:
		// Jump to NNN + V0
		cpu_state.pc = cpu_state.V[0] + (cpu_state.opcode & 0x0FFF);
		break;

	case 0xC000:
		// Set VX to rand_num & NN
		cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (cpu_state.opcode & 0x00FF);
		cpu_state.pc += 2;
		break;

	case 0xD000:
		// Draw a sprite at coord VX, VY with width 8 and height N (and more)
		printf("");
		WORD x, y, height, pixel;
		x = cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8];
		y = cpu_state.V[(cpu_state.opcode & 0x00F0) >> 4];
		height = cpu_state.opcode & 0x000F;

		cpu_state.V[0xF] = 0;
		for (int y_line = 0; y_line < height; ++y_line) {
			pixel = cpu_state.memory[cpu_state.I + y_line];
			for (int x_line = 0; x_line < 8; ++x_line) {
				if ((pixel & (0x80 >> x_line)) != 0) {
					if (cpu_state.screen[x + x_line + ((y + y_line) * 64)] == 1)
						cpu_state.V[0xF] = 1;
					cpu_state.screen[x + x_line + ((y + y_line) * 64)] ^= 1;
				}
			}
		}

		cpu_state.draw_flag = 1;
		cpu_state.pc += 2;
		break;

	case 0xE000:
		switch (cpu_state.opcode & 0x00FF) {
		case 0x009E:
			// Skip next instruction if key VX is pressed
			if (cpu_state.keys[(cpu_state.opcode & 0x0F00) >> 8])
				cpu_state.pc += 2;
			cpu_state.pc += 2;
			break;

		case 0x00A1:
			// Skip next instruction if key VX is not pressed
			if (!cpu_state.keys[(cpu_state.opcode & 0x0F00) >> 8])
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
			printf("");
			bool key_pressed;
			key_pressed = 0;
			while (!key_pressed) {
				for (int i = 0; i < 16; ++i) {
					if (cpu_state.keys[i]) {
						cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8] = cpu_state.keys[i];
						key_pressed = true;
						break;
					}
				}
			}
			cpu_state.pc += 2;
			break;

		case 0x0015:
			// Set the delay timer to VX
			cpu_state.delay_timer = cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8];
			cpu_state.pc += 2;
			break;

		case 0x0018:
			// Set the sound timer to VX
			cpu_state.sound_timer = cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8];
			cpu_state.pc += 2;
			break;

		case 0x001E:
			// Adds VX to I
			if ((cpu_state.I + cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8]) > 0xFFF)
				cpu_state.V[0xF] = 1;
			else
				cpu_state.V[0xF] = 0;
			cpu_state.I += cpu_state.V[(cpu_state.opcode & 0x0F00) >> 8];
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

	return cpu_state;
}


inline cpu load_game(struct cpu cpu_state, char game_name[]) {

	// Open file

	/*char buff[FILENAME_MAX];
	getcwd(buff, FILENAME_MAX);
	printf("Current working dir: %s\n", buff);
	*/

	FILE *fp;
	fopen_s(&fp, game_name, "rb");
	if (fp == NULL) {
		printf("File not found.\n");
	}

	// Get file size
	fseek(fp, 0, SEEK_END);
	long f_size = ftell(fp);
	rewind(fp);

	// Allocate memory
	char *file_buffer = (char*)malloc(sizeof(char) * f_size);

	// Copy file into buffer
	size_t buffer_size = fread(file_buffer, 1, f_size, fp);

	// Copy buffer into memory
	for (int i = 0; i < buffer_size; ++i) {
		cpu_state.memory[i + 512] = file_buffer[i];
	}

	// Close file and free buffer memory
	fclose(fp);
	free(file_buffer);

	return cpu_state;
}