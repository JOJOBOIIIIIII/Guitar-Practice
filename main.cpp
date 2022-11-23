
//Add the thread library and run the timer on the separate thread for accurate time

#include <chrono>
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>

//For random functions
#include <stdlib.h> 
#include <time.h>

#include "textBox.h"
#include "menu.h"

///////////////////USER PARAM//////////////////////////
int beatspm = 130;
int maxFret = 9;

void askUser() {
	std::cout << "Enter the bpm: " << std::endl;
	std::cin >> beatspm;
	std::cout << "Enter the maximum number of frets : " << std::endl;
	std::cin >> maxFret;

}
///////////////////////////////////////////////////////


/// ////////////////////////Initialization/////////////////////////////////
SDL_Window* window;
SDL_Renderer* renderer;

struct Metronome {
	int x;
	int y;
	int scale;
	SDL_Rect base;
	SDL_Rect indicator;
	SDL_Rect src;

	SDL_Surface* indicatorSurf;
	SDL_Texture* indicatorTex;

	bool reverse;
	//SDL_Sound tic; //Tic sound

	void update();
	void render();

	float increment = 1;
	float angle = 0;
	SDL_Point center;

	Metronome(int x, int y, int scale);
	~Metronome();
};
//separate the struct guitarString functions, and move theses arrays with the rest of the initialized values
int randString;
int randFret;
std::string string[] = { "E","A","D","G","B","e" };
std::string fret[] = { "1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16","17","18","19","20","21","22" };


struct guitarString {
	SDL_Rect s;
	SDL_Color color;
	textBox letter;
	int x;
	int y;

	guitarString(int xpos, int ypos, const char* l) {
		letter.message = l;
		letter.x = xpos;
		letter.y = ypos;
		letter.color = { 50,150,255,255 }; //blueish
		letter.setProperties();

		s = { xpos + 30, ypos, 700, 8 }; //string same height as letter, but decalé à droite (no overlap)
	}
	~guitarString() {}

	void draw() { //render the string and then the letter
		SDL_SetRenderDrawColor(renderer,color.r,color.g,color.b,color.a);
		SDL_RenderFillRect(renderer,&s);
		letter.renderText();
	}
};

struct Tab {
	guitarString* guitarStrings[6];
	int pos;

	Tab() {
		pos = 0;
		for (int i = 0;i < 6;i++) { //spacing of 30 between each
			guitarStrings[i] = new guitarString(30,100+30*i,string[i].c_str()); //refers to strings in initialization
		}
	}
	~Tab() {
		delete[] guitarStrings;
	}

	void update() {
		for (int i = 0;i < 6;i++) {
			guitarStrings[i]->color = {255,255,255,255};
			guitarStrings[i]->letter.color= { 50,150,255,255 }; //blueish
		}
		guitarStrings[pos]->color = { 150,150,0,200 };
		guitarStrings[pos]->letter.color = { 230,20,50,255 }; //blueish
	}

	void draw() {
		for (int i = 0;i < 6;i++) {
			guitarStrings[i]->draw();
		}
	}
};

Menu* menu;
textBox* bpm;
textBox* note;
Metronome* metronome;
Tab* tab;

SDL_AudioSpec wav_spec;
Uint32 wav_length;
Uint8* wav_buffer;
SDL_AudioSpec desired;
SDL_AudioSpec obtained;

SDL_AudioDeviceID deviceId;

typedef std::chrono::high_resolution_clock Clock;
bool gameRunning = false;
double deltaT;
////////////////////////////////////////////////////////////// 


int gameInit(const char* title, int w, int h, bool fullscreen) {
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, 0);

	metronome = new Metronome(400, 450, 1);

	note = new textBox();
	note->x = 400;
	note->y = 320;
	note->size = 60;
	note->message = "E5";
	note->setProperties();

	bpm = new textBox();
	bpm->x = 50;
	bpm->y = 50;
	bpm->setProperties();

	tab = new Tab;

	const char* menuMessages[] = { "Start","Options","Quit" };
	menuActions menuActions[] = { Start,Options,Quit };
	menu = new Menu(renderer, 3, menuMessages, menuActions);
	menu->baseColor = { 255,255,255 };
	menu->selectedColor = { 50,255,100 };
	menu->setProperties();
	menu->on = true;

	SDL_zero(desired);
	desired.freq = 48000;
	desired.format = AUDIO_F32;
	desired.channels = 2;
	desired.samples = 4096;
	desired.callback = NULL;

	//Load sound
	if (!SDL_LoadWAV("assets/tick.wav", &wav_spec, &wav_buffer, &wav_length)) SDL_ShowSimpleMessageBox(0, "Error", "wav failed to load", NULL);
	deviceId = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
	if (!deviceId) SDL_ShowSimpleMessageBox(0, "Error", "Audio driver failed to initialize", NULL);

	wav_length = 400; //bytes that will be read for the sound. If sound is too long, reduce the wav length
	
	return true;
};

Metronome::Metronome(int xpos, int ypos, int s) {
	x = xpos;
	y = ypos;
	scale = s;
	reverse = false;

	base = {x-100,y,200,15};
	indicator = { x-76,y+3,76,6 };
	src = { 0,0,76,6 };

	indicatorSurf = SDL_LoadBMP("assets/WHT.BMP");
	
	std::cout<<SDL_GetError()<<std::endl;
	indicatorTex = SDL_CreateTextureFromSurface(renderer, indicatorSurf);
	SDL_FreeSurface(indicatorSurf);

	center.x = 76;
	center.y = 3;
};
Metronome::~Metronome() {};

void Metronome::update() {
	angle = (180. / (60000000000. / beatspm)) * deltaT;
	if (reverse) angle = 180 - angle;
};

void Metronome::render() {
	SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); //white
	SDL_RenderFillRect(renderer, &base);
	SDL_RenderCopyEx(renderer, indicatorTex, &src, &indicator, angle, &center, SDL_FLIP_NONE);
};

void update() {
	metronome->update();
	tab->update();
}

void render() {
	SDL_SetRenderDrawColor(renderer,0,0,0,0); //Black background
	SDL_RenderClear(renderer);

	bpm->renderText();
	note->renderText();
	metronome->render();

	tab->draw();

	SDL_RenderPresent(renderer);
}


int main(int argc, char* argv[]) {

	if (gameInit("Guitar Practice", 800, 640, 0)) {
		gameRunning = true;
	}
	else std::cout << "Initialization failed." << std::endl;

	srand(time(NULL));
	askUser();
	std::string temp = std::to_string(beatspm) + " BPM";
	bpm->message = temp.c_str();
	bpm->setProperties();
	auto t1 = Clock::now(); //start timer
	
	while (gameRunning) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			//necessary to have a functionning window
		}

		if (deltaT > 60000000000./beatspm) { //If difference > number of ns between beats
			SDL_PauseAudioDevice(deviceId, 1);
			SDL_ClearQueuedAudio(deviceId);
			t1 = Clock::now(); //New reference point after "x" seconds
			SDL_QueueAudio(deviceId, wav_buffer, wav_length);
			SDL_PauseAudioDevice(deviceId, 0);
			
			metronome->reverse = !metronome->reverse;

			int strNum = rand() % 6;
			tab->pos = strNum;
			std::string temp = string[strNum] + fret[(rand() % maxFret)];
			note->message = temp.c_str();
			note->setProperties();

			update();

			for (int i = 0;i < 6;i++) {
				tab->guitarStrings[i]->letter.setProperties();
			}
		}

		update();
		render();

		auto t2 = Clock::now(); //End of code loop, measure time again

		//calculate difference
		deltaT = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count(); 
	}





	//if not running...
	delete metronome;
	delete note;
	delete bpm;
	delete tab;

	SDL_FreeWAV(wav_buffer);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	bool confirm = false;
	char* input = new char[1];

	std::cout << "Type \"y\" to close this window." << std::endl;

	while (!confirm) {
		std::cin >> input;
		if (*input == *"y") { confirm = true; } 
	}

	return 0;
}