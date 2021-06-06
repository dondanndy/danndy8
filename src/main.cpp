#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <fstream>
#include <filesystem>
#include "CHIP8.h"

namespace fs = std::filesystem;

std::vector<char> Load_rom() {

	//List all the roms.
	size_t i = 1;
	std::vector<fs::path> roms;
	for (auto& p : fs::directory_iterator("../ROMS")) {
		std::cout << i << " - " << p.path().filename() << '\n';
		roms.push_back(p.path());
		i++;
	}

	//Propmt the user to select a ROM.
	std::cout << "\nSelect a rom\n>";
	size_t choice;
	while (true) {
		std::cin >> choice;
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Please insert a number.\n>";
		}
		else {
			if (choice > i) {
				std::cout << "Please insert a valid number.\n>";
			}
			else {
				break;
			}
		}
	}
	
	auto chosen_rom = roms[choice - 1];
	std::cout << "ROM Loaded: " << chosen_rom.filename() << std::endl;

	//We now import the ROM.
	std::ifstream rom(chosen_rom, std::ios::binary);
	if (!rom) {
		std::cout << "File could not be opened :(\n";
	}

	size_t file_size = fs::file_size(chosen_rom);
	std::vector<char> rom_buf(file_size);

	rom.seekg(0, std::ios::beg);
	rom.read(&rom_buf[0], file_size);

	return rom_buf;
}

int main(int argc, char* args[]) {

	std::cout << "Welcome to danndy8, a CHIP8 emulator" << std::endl;

	auto rom = Load_rom();

	CHIP8 chip8(rom);

	chip8.createScreen();
	double FREQUENCY = 500;

	bool quit = false;

	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	//Main loop.
	while (!quit) {
		/*while ((std::chrono::high_resolution_clock::now() - start).count() < 1 / FREQUENCY) {

		}*/
		quit = chip8.next();
		//start = std::chrono::high_resolution_clock::now();
	}

	chip8.closeScreen();
	
	return 0;
}