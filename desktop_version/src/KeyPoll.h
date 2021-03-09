#ifndef KEYPOLL_H
#define KEYPOLL_H

#include <string>
#include <vector>
#include <map> // FIXME: I should feel very bad for using C++ -flibit

#include "SDL.h"

enum 
{
	KEYBOARD_UP = SDLK_UP,
	KEYBOARD_DOWN = SDLK_DOWN,
	KEYBOARD_LEFT = SDLK_LEFT,
	KEYBOARD_RIGHT = SDLK_RIGHT,
	KEYBOARD_ENTER = SDLK_RETURN,
	KEYBOARD_SPACE = SDLK_SPACE,

	KEYBOARD_w = SDLK_w,
	KEYBOARD_s = SDLK_s,
	KEYBOARD_a = SDLK_a,
	KEYBOARD_d = SDLK_d,
	KEYBOARD_m = SDLK_m,

	KEYBOARD_v = SDLK_LSHIFT,
	KEYBOARD_z = SDLK_LALT,

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
