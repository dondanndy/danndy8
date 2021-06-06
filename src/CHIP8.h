#pragma once

#include <array>
#include <vector>
#include <random>
#include <iostream>
#include <chrono>
#include <SDL.h>
#include <SDL_keycode.h>

class CHIP8 {
public:
	CHIP8(std::vector<char>&);
	~CHIP8();

	void nextInstruction();
	void decodeOpcode(unsigned short);
	bool next();

	// Graphics
	bool createScreen();
	void updateScreen();
	void closeScreen();

	bool pullEvents();

private:
	//Memory
	std::array<unsigned char, 4096> memory;
	std::array<unsigned char, 16> keys;

	std::array<unsigned short, 16> stack;
	unsigned short stack_pointer;
	
	//GPU buffer
	std::array<unsigned char, 64 * 32> gfx;
	bool draw_screen;

	// Registers
	unsigned short pc;
	unsigned short I;
	std::array<unsigned char, 16> V;

	//SDL2 variables
	SDL_Event e;
	SDL_Window* window;

	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 320;

	//timers
	unsigned char sound_timer;
	unsigned char delay_timer;

	//Instructions, names by opcode.
	void op_00E0(unsigned short);
	void op_00EE(unsigned short);
	void op_1NNN(unsigned short);
	void op_2NNN(unsigned short);
	void op_3XNN(unsigned short);
	void op_4XNN(unsigned short);
	void op_5XY0(unsigned short);
	void op_6XNN(unsigned short);
	void op_7XNN(unsigned short);
	void op_8XYN(unsigned short);
	void op_9XY0(unsigned short);
	void op_ANNN(unsigned short);
	void op_BNNN(unsigned short);
	void op_CXNN(unsigned short);
	void op_DXYN(unsigned short);
	void op_EXNN(unsigned short);
	void op_FXNN(unsigned short);

	//fontset
	std::array<unsigned char, 80> fontset = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

};
