#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "map.h"

#define WHITE {255, 255, 255, 255}
#define BLACK {0, 0, 0, 255}

enum {SDLFAIL, TTFFAIL};

class Gui {
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		TTF_Font *font;

		Map* map;

		int mapWidth;
		int mapHeight;

		int mapSize;
		int squareSize;
		int windowSize[2] = {0};

	public:
		Gui(Map* _map, const int width, const int height);
		~Gui(void);
		void prepareScene(const SDL_Color bg);
		void presentScene(void);

		void fillRect(const SDL_Rect rect, const SDL_Color c);
		void outlineRect(const SDL_Rect rect, const SDL_Color c);
		void drawText(const std::string text, SDL_Rect rect, SDL_Color color);

		void drawStartup(void);
		void drawMap(void);

		void events(void);
};

class guiException : public std::exception{
	private:
		std::string msg;
		int type;

	public:
		guiException(const std::string _msg, const int _type): msg(_msg), type(_type){}
		int getType(){
			return type;
		}

	virtual const char* what() const throw()
	{
		return msg.c_str();
	}
};
