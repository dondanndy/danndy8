#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <vector>
#include <array>
#include <string>

#include <wx/wx.h>
#include <wx/aboutdlg.h>
#include <SDL.h>
#include "CHIP8.h"

class cMain : public wxFrame {
public:
	explicit cMain();
	~cMain();

	//Functions for events.
	void onClose(wxCloseEvent& evt);
	void onMenuOpen(wxMenuEvent& evt);
	void onMenuClose(wxMenuEvent& evt);
	void onOpenROM(wxCommandEvent& evt);
	void onReset(wxCommandEvent& evt);
	void onPause(wxCommandEvent& evt);
	void onExit(wxCommandEvent& evt);
	void onAbout(wxCommandEvent& evt);
	void onKeyDown(wxKeyEvent& evt);
	void onIdle(wxIdleEvent& evt);
	void onOneCycle(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();

private:
	//Render timers
	float renderRate = 16.66f;
	std::chrono::time_point<std::chrono::steady_clock> lastTime;
	std::chrono::duration<float, std::milli> accumulator{};

	std::vector<char> ROM;

	//Menu bar and menus
	wxMenuBar* m_pMenuBar = nullptr;
	wxMenu* m_pFile = nullptr;
	wxMenu* m_pHelp = nullptr;

	//SDL window, handles graphics
	SDL_Window* sdlWindow = NULL;
	//SDL renderer, handles graphics
	SDL_Renderer* sdlRenderer = NULL;
	//Pixel color - white default
	int pColor[3] = { 255, 255, 255 };
	//Background color - black default
	int bColor[3] = { 0, 0, 0 };


	//Chip-8
	//CHIP8 theChip8;
	////Keeps track of Chip-8 state for temporary pauses
	//bool runChip8 = false;
	////Keeps track of if a ROM has been loaded
	//bool loadedROM = false;
	////Chip-8 rom size in bytes
	//size_t chip8ROMSize;
	////Chip-8 rom array
	//unsigned char chip8ROM[3584];

	//Default ROM directory
	std::string defaultROMDir;
	//Default save state directory
	std::string defaultStateDir;

	//Load ROM helper
	std::vector<char> loadROM(std::string filename);

	void drawScreen();

	//wxEvent IDs
	enum eventIDs { openROMID, resetID, pauseID, resumeID, aboutID, exitID };
};