#include "textBox.h"
#include <SDL.h>

extern SDL_Renderer* renderer;

textBox::textBox() {
	x = 300;
	y = 300;
	color = { 255,255,255 };
	message = "test";
	size = 24;
	ran = false;
	setProperties();
};

textBox::~textBox() {};

void textBox::setProperties(){
	
	if (ran){ 
		 SDL_DestroyTexture(Message); //If this line does not exist, there is a memory leak since we create a new texture below
	}

	TTF_Init();
	TTF_Font* Sans = TTF_OpenFont("OpenSans-Regular.ttf", size);
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, message, color);
	TTF_CloseFont(Sans);
	TTF_Quit();

	std::cout << SDL_GetError() << std::endl;
	Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	std::cout << SDL_GetError() << std::endl;
	Message_rect.w = surfaceMessage->w; 
	Message_rect.h = surfaceMessage->h; 
	Message_rect.x = x - (Message_rect.w / 2);  
	Message_rect.y = y - (Message_rect.h / 2);

	SDL_FreeSurface(surfaceMessage);
	ran = true;
}

void textBox::renderText() {
	SDL_RenderCopy(renderer,Message,NULL,&Message_rect);
}