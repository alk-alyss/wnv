#include "map.h"
#include "gui.h"

int getWidth() {
	/*
	   Get board width from user
	*/

	int width;

	clearScreen();
	std::cout << "Give width: ";
	std::cin >> width;

	return width;
}

int getHeight() {
	/*
	   Get board height from user
	*/

	int height;

	clearScreen();
	std::cout << "Give height: ";
	std::cin >> height;

	return height;
}

std::string getTeam() {
	/*
	   Get player team from user
	*/

	std::string team;

	do {
		clearScreen();
		std::cout << "Select team [Werewolves/Vampires]: ";
		std::cin >> team;
	} while (!(team == "Werewolves" || team == "Vampires"));

	return team;
}


SDL_Color bg = BLACK;

int main(int argc, char* argv[]) {
	srand(time(NULL));
	/* srand(0); */

	int width = 16;
	int height = 10;
	std::string playerTeam = "Werewolves";

	width = getWidth();
	height = getHeight();
	playerTeam = getTeam();

	Map* gameMap = new Map(width, height, playerTeam);

	Gui* gui = nullptr;
	try {
		gui = new Gui(gameMap, 856, 480);
	} catch (guiException& e) {
		std::cout << e.what();

		switch (e.getType()) {
			case SDLFAIL:
				std::cout << " SDL_Error: " << SDL_GetError();
				break;
			case TTFFAIL:
				std::cout << " TTF_Error: " << TTF_GetError();
				break;
		}

		std::cout << std::endl;
		return EXIT_FAILURE;
	}

	while (RUNNING) {
		gui->events();

		if (!PAUSED) {
			gui->prepareScene(bg);

			int result = gameMap->update();

			if (result == WEREWOLVES) {
				std::cout << "Werewolves win" << std::endl;
				RUNNING = false;
			} else if (result == VAMPIRES) {
				std::cout << "Vampires win" << std::endl;
				RUNNING = false;
			}

			gui->drawMap();

			gui->presentScene();
		}

		SDL_Delay(80);
	}

	return EXIT_SUCCESS;
}
