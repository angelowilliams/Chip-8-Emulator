#ifndef GFX_H
	#define GFX_H
#endif

#include "SDL.h"

#ifndef CPU_H
	#include "cpu.h"
#endif

inline struct cpu setup_graphics(struct cpu cpu_state) {
	SDL_Init(SDL_INIT_VIDEO);

	cpu_state.window = SDL_CreateWindow(
		"CHIP-8",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		1280,                               // width, in pixels
		640,                               // height, in pixels
		SDL_WINDOW_SHOWN                 // flags - see below
	);

	int index = -1;
	cpu_state.renderer = SDL_CreateRenderer(
		cpu_state.window,
		index,
		SDL_RENDERER_ACCELERATED);

	cpu_state.rect.w = 20;
	cpu_state.rect.h = 20;

	return cpu_state;
}

inline struct cpu draw_graphics(struct cpu * cpu_state) {
	// Set black background
	SDL_SetRenderDrawColor(cpu_state->renderer, 0, 0, 0, 255);
	SDL_RenderClear(cpu_state->renderer);

	// Switch to white
	SDL_SetRenderDrawColor(cpu_state->renderer, 255, 255, 255, 255);

	// Find pixels
	for (int i = 0; i < 2048; ++i) {
		if (cpu_state->screen[i]) {
			cpu_state->rect.x = (i % 64) * 20;
			cpu_state->rect.y = (i / 64) * 20;
			SDL_RenderFillRect(cpu_state->renderer, &cpu_state->rect);
		}
	}

	// Update screen
	SDL_RenderPresent(cpu_state->renderer);

	return *cpu_state;
}

inline void end_gfx(struct cpu * cpu_state) {
	SDL_DestroyWindow(cpu_state->window);
	SDL_Quit();
	return;
}