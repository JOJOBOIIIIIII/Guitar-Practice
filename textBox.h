#pragma once
#include <SDL_ttf.h>
#include <iostream>

struct textBox {

	int x;
	int y;
	SDL_Color color;
	const char* message;
	int size;
	bool ran;

	SDL_Texture* Message;
	SDL_Rect Message_rect;


	textBox();
	~textBox();

	void renderText();
	void setProperties();

};