#include "cMain.h"

namespace fs = std::filesystem;

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_CLOSE(cMain::onClose)
	EVT_MENU_OPEN(cMain::onMenuOpen)
	EVT_MENU_CLOSE(cMain::onMenuClose)
	EVT_KEY_DOWN(cMain::onKeyDown)
	EVT_MENU(openROMID, cMain::onOpenROM)
	EVT_MENU(exitID, cMain::onExit)
	EVT_MENU(resetID, cMain::onReset)
	EVT_MENU(pauseID, cMain::onPause)
	EVT_MENU(aboutID, cMain::onAbout)
	EVT_IDLE(cMain::onIdle)
wxEND_EVENT_TABLE()

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "dandy8 emulator", wxPoint(30, 30), wxSize(1280, 720))
{
	//Create menu bar
	m_pMenuBar = new wxMenuBar();

	//Create file menu
	m_pFile = new wxMenu();
	m_pFile->Append(openROMID, "Open ROM");
	m_pFile->Append(resetID, "Reset");
	m_pFile->AppendSeparator();
	m_pFile->AppendCheckItem(pauseID, "Pause");
	m_pFile->AppendSeparator();
	m_pFile->Append(exitID, "Exit");


	//Create help menu
	m_pHelp = new wxMenu();
	m_pHelp->Append(aboutID, "About");

	//Build and show menu bar on frame
	m_pMenuBar->Append(m_pFile, "File");
	m_pMenuBar->Append(m_pHelp, "Help");
	SetMenuBar(m_pMenuBar);

	//SDL window & renderer initialization
	bool sdlInitialized = true;
	sdlWindow = SDL_CreateWindowFrom((void*)this->GetHandle());
	if (sdlWindow == NULL) {
		std::cout << "SDL Window could not be initialized!" << std::endl;
		sdlInitialized = false;
	}
	else {
		sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
		if (sdlRenderer == NULL) {
			std::cout << "SDL Renderer could not be initialized!" << std::endl;
			sdlInitialized = false;
		}
		else {
			std::cout << "SDL Window and Renderer successfully initialized..." << std::endl;
			SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0xFF, 0xFF, 0x0);
		}
	}

	this->SetMinSize(wxSize(640, 320));
	Show();
}

cMain::~cMain()
{
}

void cMain::onClose(wxCloseEvent& evt)
{
	Destroy();
}

void cMain::onMenuOpen(wxMenuEvent& evt)
{
}

void cMain::onMenuClose(wxMenuEvent& evt)
{
}

void cMain::onOpenROM(wxCommandEvent& evt)
{
	wxFileDialog openFileDialog(this, "Select a Chip-8 ROM", "", "", "", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;

	//Feed file path to load ROM function
	std::string filepath = openFileDialog.GetPath().ToStdString();

	std::cout << "File path : " << filepath << std::endl;

	ROM = loadROM(filepath);
}

void cMain::onReset(wxCommandEvent& evt)
{
}

void cMain::onPause(wxCommandEvent& evt)
{
}

void cMain::onExit(wxCommandEvent& evt)
{
	this->Close();
	evt.Skip();
}

void cMain::onAbout(wxCommandEvent& evt)
{
	wxAboutDialogInfo info;
	info.SetName("danndy8 emulator");
	info.SetVersion("0.0.1");
	info.SetDescription("This is a CHIP8 emulator for educational purposes.");
	wxAboutBox(info);

	evt.Skip();
}

void cMain::onKeyDown(wxKeyEvent& evt)
{
}

void cMain::onIdle(wxIdleEvent& evt)
{
}

void cMain::onOneCycle(wxCommandEvent& evt)
{
}

std::vector<char> loadRom(std::string filepath) {

	//We now import the ROM.
	std::ifstream rom(filepath, std::ios::binary);
	if (!rom) {
		std::cout << "File could not be opened :(\n";
	}

	size_t file_size = fs::file_size(filepath);
	std::vector<char> rom_buf(file_size);

	rom.seekg(0, std::ios::beg);
	rom.read(&rom_buf[0], file_size);

	return rom_buf;
}
