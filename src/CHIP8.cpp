#include "CHIP8.h"

CHIP8::CHIP8(std::vector<char>& rom) {
		//Let's set the registers.
		pc = 0x200;
		V = { 0 };
		keys = { 0 };
		I = 0;
		memory = { 0 };

		gfx = { 0 };
		draw_screen = false;

		stack = { 0 };
		stack_pointer = 0;

		delay_timer = 0;
		sound_timer = 0;

		//We load the ROM.
		for (int i = 0; i < (int)rom.size(); i++) {
			memory[i + 512] = rom[i];
		};

		//We load the fontset.
		for (int i = 0; i < 80; ++i) {
			memory[i] = fontset[i];
		};
}

CHIP8::~CHIP8() {
	closeScreen();
}

bool CHIP8::next() {
	std::chrono::high_resolution_clock::time_point start, end;

	start = std::chrono::high_resolution_clock::now();
	nextInstruction();
	end = std::chrono::high_resolution_clock::now();

	//Time taken for the CPU to evaluate a instruction.
	auto time_elapsed = std::chrono::duration<double, std::milli>(end - start);

	if (draw_screen) {
		updateScreen();
		draw_screen = false;
	}

	while (std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start) < tick_duration) {
		//While we reach the end of the time that the instuction has to take we check for inputs.
		//We return true if the QUIT input has been received.

		if(pullEvents()) return true;
	}

	return false;
}


void CHIP8::nextInstruction() {
	unsigned short opcode;
	opcode = memory[pc] << 8 | memory[pc + 1];

	decodeOpcode(opcode);

	if (delay_timer > 0) {
		delay_timer--;
	}

	if (sound_timer > 0) {
		sound_timer--;
	}

}

void CHIP8::decodeOpcode(unsigned short addr) {
	unsigned short opcode;
	unsigned short data;

	opcode = (addr & 0xF000) >> 12;
	data = (addr & 0x0FFF);
	
	//DEBUG
	//std::cout << "opcode: " << std::hex << opcode << " | data: " << std::hex << data << std::endl;
	//std::cout << "pc: " << std::hex << pc << std::endl;

	//Once we have the opcode and the data associated whith it, let's execute it.
	switch (opcode) {
	case 0x0:
		switch (data)	{
			case 0xE0:        
				op_00E0(data);
				break;

			case 0xEE:     
				op_00EE(data);
				break;
		}
		break;

	case 0x1:
		op_1NNN(data);
		break;

	case 0x2:
		op_2NNN(data);
		break;

	case 0x3:
		op_3XNN(data);
		break;

	case 0x4:
		op_4XNN(data);
		break;

	case 0x5:
		op_5XY0(data);
		break;

	case 0x6:
		op_6XNN(data);
		break;

	case 0x7:
		op_7XNN(data);
		break;

	case 0x8:
		op_8XYN(data);
		break;

	case 0x9:
		op_9XY0(data);
		break;

	case 0xA:
		op_ANNN(data);
		break;

	case 0xB:
		op_BNNN(data);
		break;

	case 0xC:
		op_CXNN(data);
		break;

	case 0xD:
		op_DXYN(data);
		break;

	case 0xE:
		op_EXNN(data);
		break;

	case 0xF:
		op_FXNN(data);
		break;

	default:
		std::cout << "Opcode " << std::hex << opcode << " not implemented yet :(" << std::endl;
		//std::cout << "Data: " << std::hex << data << std::endl;
		pc += 2;
	};
}

void CHIP8::op_00E0(unsigned short data) {
	gfx = { 0 };

	pc += 2;
}

void CHIP8::op_00EE(unsigned short data) {
	stack_pointer--;

	pc = stack[stack_pointer];

	pc += 2;
}

void CHIP8::op_1NNN(unsigned short data) {
	pc = data;
}

void CHIP8::op_2NNN(unsigned short data) {
	stack[stack_pointer] = pc;

	stack_pointer++;

	pc = data;
}

void CHIP8::op_3XNN(unsigned short data) {
	unsigned short reg1 = (data & 0xF00) >> 8;
	unsigned short reg2 = data & 0x0FF;

	if (V[reg1] == reg2) {
		pc += 4;
	}
	else {
		pc += 2;
	}

}

void CHIP8::op_4XNN(unsigned short data) {
	unsigned short reg1 = (data & 0xF00) >> 8;
	unsigned short reg2 = data & 0xFF;


	if (V[reg1] != reg2) {
		pc += 4;
	}
	else {
		pc += 2;
	}

}

void CHIP8::op_5XY0(unsigned short data) {
	unsigned short reg1 = (data & 0xF00) >> 8;
	unsigned short reg2 = (data & 0xF0) >> 4;

	if (V[reg1] == V[reg2]) {
		pc += 4;
	}
	else {
		pc += 2;
	}

}

void CHIP8::op_6XNN(unsigned short data) {
	unsigned char reg1 = (data & 0xF00) >> 8;
	unsigned char reg2 = data & 0xFF;

	V[reg1] = reg2;
		
	pc += 2;
}

void CHIP8::op_7XNN(unsigned short data) {
	unsigned short reg1 = (data & 0xF00) >> 8;
	unsigned short reg2 = data & 0x0FF;

	V[reg1] += reg2;

	pc += 2;
}

void CHIP8::op_8XYN(unsigned short data) {
	//First we have to get the lasts two bits:
	unsigned short selector = (data & 0xF);

	//And the data for choosing the registers
	unsigned short reg1 = (data & 0x0F00) >> 8;
	unsigned short reg2 = (data & 0x00F0) >> 4;

	switch (selector) {
	case 0x0:
		V[reg1] = V[reg2];
		break;

	case 0x1:
		V[reg1] = V[reg1] | V[reg2];
		break;

	case 0x2:
		V[reg1] = V[reg1] & V[reg2];
		break;

	case 0x3:
		V[reg1] = V[reg1] ^ V[reg2];
		break;

	case 0x4:
		if (V[reg2] > (0xFF - V[reg1])) {
			V[0xF] = 1; //carry
		} else{
			V[0xF] = 0;
		}

		V[reg1] += V[reg2];
		break;

	case 0x5:
		if (V[reg1] > V[reg2]) {
			V[0xF] = 1;
		}
		else {
			V[0xF] = 0;
		};

		V[reg1] -= V[reg2];
		break;

	case 0x6: 
		V[0xF] = V[reg1] & 0x0001;
		V[reg1] = V[reg2] >> 1;
		break;

	case 0x7:
		if (V[reg2] > V[reg1]) {
			V[0xF] = 1;
		}
		else {
			V[0xF] = 0;
		};

		V[reg1] = V[reg2] - V[reg1];
		break;

	case 0xE:
		//V[0xF] = (V[reg2] & 0x80) >> 4;

		if (V[reg1] & 0x80)
			V[0xF] = 1;
		else
			V[0xF] = 0;

		V[reg1] = V[reg2] << 1;
		break;

	default:
		std::cout << "Opcode: 8" << std::hex << data << " not implemented :(" << std::endl;
		break;
	}

	pc += 2;
}

void CHIP8::op_9XY0(unsigned short data) {
	unsigned char reg1 = (data & 0x0F00) >> 8;
	unsigned char reg2 = (data & 0x00F0) >> 4;

	if (V[reg1] != V[reg2]) {
		pc += 4;
	}
	else {
		pc += 2;
	}

}

void CHIP8::op_ANNN(unsigned short data) {
	I = data;

	pc += 2;
}

void CHIP8::op_BNNN(unsigned short data) {
	pc = V[0] + data;
}

void CHIP8::op_CXNN(unsigned short data) {
	unsigned char reg1 = (data & 0xF00) >> 8;
	unsigned char reg2 = data & 0x0FF;

	//Random number generator.
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, 255);
	int rand = dist(rng);

	V[reg1] = rand & reg2;

	pc += 2;
}

void CHIP8::op_DXYN(unsigned short data) {
	unsigned short x = V[(data & 0xF00) >> 8];
	unsigned short y = V[(data & 0x0F0) >> 4];
	unsigned short height = data & 0xF;
	unsigned short pixel;

	V[0xF] = 0;
	for (int yline = 0; yline < height; yline++) {
		pixel = memory[I + yline];
		
		for (int xline = 0; xline < 8; xline++) {
			if ((pixel & (0x80 >> xline)) != 0) {
				if (gfx.at(x + xline + (y + yline) * 64) == 1) {
					V[0xF] = 1;
				}
				gfx[x + xline + ((y + yline) * 64)] ^= 1;
			}
		}
	}

	draw_screen = true;
	pc += 2;
}

void CHIP8::op_EXNN(unsigned short data) {
	unsigned char selector = data & 0x0FF;
	unsigned char reg = (data & 0xF00) >> 8;

	switch (selector) {
	case 0x9E: {
		if (keys[V[reg]] != 0) {
			pc += 4;
		}
		else {
			pc += 2;
		}
		break;
	}
	case 0xA1: {
		if (keys[V[reg]] == 0) {
			pc += 4;
		}
		else {
			pc += 2;
		}
		break;

	default:
		std::cout << "Opcode: E" << std::hex << data << " not implemented :(" << std::endl;
		break;
	}
	}
}

void CHIP8::op_FXNN(unsigned short data) {
	unsigned char selector = data & 0x0FF;
	unsigned char reg = (data & 0xF00) >> 8;

	switch (selector)
	{
	case 0x07:
		V[reg] = delay_timer;
		break;

	case 0x15:
		delay_timer = V[reg];
		break;

	case 0x18:
		sound_timer = V[reg];
		break;

	case 0x1E:
		I += V[reg];
		break;

	case 0x29:
		I = V[reg] * 5;
		break;

	case 0x33:
		memory[I] = V[reg] / 100;
		memory[I + 1] = (V[reg] / 10) % 10;
		memory[I + 2] = (V[reg] % 100) % 10;

		break;

	case 0x55:
		for (int i = 0; i <= reg; i++) {
			memory[I + i] = V[i];
		}
		break;

	case 0x65:
		for (int i = 0; i <= reg; i++) {
			V[i] = memory[I + i];
		}
		break;

	default:
		std::cout << "Opcode: F" << std::hex << data << " not implemented :(" << std::endl;
		break;
	}

	pc += 2;
}

bool CHIP8::createScreen()
{
	window = SDL_CreateWindow("dondanndy8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL) {
		std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}
	return true;
}

void CHIP8::updateScreen()
{
	SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

	//Background
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0, 0, 0));

	//Each pixel to be drawn.
	SDL_Rect pixel_pos;
	for (size_t x = 0; x < SCREEN_WIDTH/10; ++x)
	{
		for (size_t y = 0; y < SCREEN_HEIGHT/10; ++y)
		{
			if (gfx[y * 64 + x] == 1)
			{
				pixel_pos.x = x * 10;
				pixel_pos.y = y * 10;
				pixel_pos.w = 10;
				pixel_pos.h = 10;

				SDL_FillRect(screenSurface, &pixel_pos, SDL_MapRGB(screenSurface->format, 255, 255, 255));
			}
		}
	}
	//SDL_Flip(screenSurface);

	//Update the surface
	SDL_UpdateWindowSurface(window);
}

bool CHIP8::pullEvents()
{
	while (SDL_PollEvent(&e) != 0)
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			return true;
		}
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_1: keys[1] = 1; break;
			case SDLK_2: keys[2] = 1; break;
			case SDLK_3: keys[3] = 1; break;
			case SDLK_4: keys[12] = 1; break;
			case SDLK_q: keys[4] = 1; break;
			case SDLK_w: keys[5] = 1; break;
			case SDLK_e: keys[6] = 1; break;
			case SDLK_r: keys[13] = 1; break;
			case SDLK_a: keys[7] = 1; break;
			case SDLK_s: keys[8] = 1; break;
			case SDLK_d: keys[9] = 1; break;
			case SDLK_f: keys[14] = 1; break;
			case SDLK_z: keys[10] = 1; break;
			case SDLK_x: keys[0] = 1; break;
			case SDLK_c: keys[11] = 1; break;
			case SDLK_v: keys[15] = 1; break;
			}
		}
		else if (e.type == SDL_KEYUP) {
			switch (e.key.keysym.sym) {
			case SDLK_1: keys[1] = 0; break;
			case SDLK_2: keys[2] = 0; break;
			case SDLK_3: keys[3] = 0; break;
			case SDLK_4: keys[12] = 0; break;
			case SDLK_q: keys[4] = 0; break;
			case SDLK_w: keys[5] = 0; break;
			case SDLK_e: keys[6] = 0; break;
			case SDLK_r: keys[13] = 0; break;
			case SDLK_a: keys[7] = 0; break;
			case SDLK_s: keys[8] = 0; break;
			case SDLK_d: keys[9] = 0; break;
			case SDLK_f: keys[14] = 0; break;
			case SDLK_z: keys[10] = 0; break;
			case SDLK_x: keys[0] = 0; break;
			case SDLK_c: keys[11] = 0; break;
			case SDLK_v: keys[15] = 0; break;
			}
		}
	}
	return false;
}

void CHIP8::closeScreen()
{
	// Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems 
	SDL_Quit();
}