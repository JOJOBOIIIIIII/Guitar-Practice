#pragma once
#include <SDL.h>
#include "textBox.h"


enum menuActions {None,Start,Continue,Quit,Options,TitleScreen,Pause};

struct Menu {
	
	SDL_Renderer* renderer;
	textBox* textBoxes;

	SDL_Color baseColor, selectedColor;
	int baseSize, selectedSize;

	menuActions* actions;

	int pos;  //Initial position of the cursor 
	bool on; //The menu in question can be on screen or not

	int n; //How many boxes
	//textBox textBoxes[n]; //The actual textboxes list --> try to put in constructor

	Menu(SDL_Renderer* ren, const int n, const char* messages[],menuActions acts[]); //Constructor will create the textboxes based on n
	//Size of messages will be determined by the number of messages we input

	~Menu();

	void update();
	void render();
	menuActions handleEvents();

	void setProperties();
};