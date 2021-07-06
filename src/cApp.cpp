#include "cApp.h"

wxIMPLEMENT_APP_NO_MAIN(cApp);

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("Something went wrong with the SDL initialization!\n");
		return 1;
	}

	return wxEntry(argc, argv);
}

cApp::cApp() : wxApp() {
}

cApp::~cApp() {
}

bool cApp::OnInit() {

	new cMain();

	return true;
}

int cApp::OnExit()
{
	
	SDL_Quit();
	return wxApp::OnExit();
}