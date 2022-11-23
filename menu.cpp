#include "menu.h"

Menu::Menu(SDL_Renderer* ren, const int num,const char* messages[],menuActions acts[]) {
	//I could try to determine size of the array to get n instead of an extra parameter
	
	pos = 0;
	on = false;

	baseColor = {255,255,255};
	baseSize = 24;
	selectedColor = {255,255,0};
	selectedSize = 42;

	renderer = ren;
	n = num;

	textBoxes=new textBox[n];
	actions = new menuActions[n];

	for (int i = 0;i < n;i++) {

		actions[i] = acts[i];

		textBoxes[i].message = messages[i];
		textBoxes[i].color = baseColor;
		textBoxes[i].size = baseSize;

		textBoxes[i].x = 800 / 2;
		textBoxes[i].y = (640 - ((n - 1) * 100)) / 2 + i * 100; //To change and also in textbox.cpp (for center of window)
	}

	textBoxes[pos].color = selectedColor; 
	textBoxes[pos].size = selectedSize;

	setProperties();
}

Menu::~Menu() {
	delete[] actions; //delete the whole array pointer
	delete[] textBoxes;
}

void Menu::setProperties() {

	for (int i = 0;i < n;i++) { //reset all to default
		textBoxes[i].color = baseColor;
		textBoxes[i].size = baseSize;
	}
	textBoxes[pos].color = selectedColor; //put yellow for the one selected
	textBoxes[pos].size = selectedSize;

	for (int i = 0;i < n;i++) {
		textBoxes[i].setProperties();
	}
}

menuActions Menu::handleEvents() {

	SDL_Event event;

	if (SDL_PollEvent(&event)) {

		switch (event.type) {

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_UP:
				pos -= 1;
				break;
			case SDLK_DOWN:
				pos += 1;
				break;

			case SDLK_RETURN:
				return actions[pos];
			}
			if (pos == n) pos = 0;
			if (pos < 0) pos = n - 1;

		default:
			return None;
			break;
		}
	}

	return None;
}


void Menu::update() { //animate the option selected (change the size)

	static int lastPos=0; //static will execute once. will be remmbered next update

	if (lastPos != pos) { //
		setProperties();
	}
	lastPos = pos;
	
}

void Menu::render() {
	
	SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
	SDL_RenderClear(renderer);

	for (int i = 0;i < n;i++) {
		textBoxes[i].renderText();
	}

	SDL_RenderPresent(renderer);

}