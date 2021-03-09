#ifndef KEYPOLL_H
#define KEYPOLL_H

#include <string>
#include <vector>
#include <map> // FIXME: I should feel very bad for using C++ -flibit

#include "SDL.h"

enum
{
	KEYBOARD_UP = SDLK_u,
	KEYBOARD_DOWN = SDLK_d,
	KEYBOARD_LEFT = SDLK_l,
	KEYBOARD_RIGHT = SDLK_r,
	KEYBOARD_ENTER = SDLK_s,
	KEYBOARD_SPACE = SDLK_a,

	KEYBOARD_w = SDLK_1,
	KEYBOARD_s = SDLK_2,
	KEYBOARD_a = SDLK_3,
	KEYBOARD_d = SDLK_4,
	KEYBOARD_m = SDLK_5,

	KEYBOARD_v = SDLK_m,
	KEYBOARD_z = SDLK_n,

	KEYBOARD_BACKSPACE = SDLK_BACKSPACE
};

class KeyPoll
{
public:
	std::map<SDLKey, bool> keymap;

	bool isActive;

	bool resetWindow;

	bool escapeWasPressedPreviously;
	bool quitProgram;
	bool toggleFullscreen;

	int sensitivity;

	void setSensitivity(int _value);

	KeyPoll();

	void enabletextentry();

	void disabletextentry();

	void Poll();

	bool isDown(int key);
	bool isUp(int key);

	int leftbutton, rightbutton, middlebutton;
	int mx, my;

	bool textentrymode;
	int keyentered, keybufferlen;
	bool pressedbackspace;
	std::string keybuffer;

private:
	int xVel, yVel;
	bool useFullscreenSpaces;
	Uint32 wasFullscreen;
};


#endif /* KEYPOLL_H */
