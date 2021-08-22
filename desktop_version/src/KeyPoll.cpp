#include "KeyPoll.h"
#include <stdio.h>
#include <string.h>

#include "UtilityClass.h"
#include "Screen.h"


void KeyPoll::setSensitivity(int _value)
{
	switch (_value)
	{
		case 0:
			sensitivity = 28000;
			break;
		case 1:
			sensitivity = 16000;
			break;
		case 2:
			sensitivity = 8000;
			break;
		case 3:
			sensitivity = 4000;
			break;
		case 4:
			sensitivity = 2000;
			break;
	}

}

KeyPoll::KeyPoll(Screen* screen) : screen(screen)
{
	xVel = 0;
	yVel = 0;
	setSensitivity(2);

	quitProgram = 0;
	textentrymode=true;
	keybuffer="";
	leftbutton=0; rightbutton=0; middlebutton=0;
	mx=0; my=0;
	resetWindow = 0;
	toggleFullscreen = false;
	pressedbackspace=false;

	useFullscreenSpaces = false;
}

void KeyPoll::enabletextentry()
{
	keybuffer="";
	textentrymode = true;
}

void KeyPoll::disabletextentry()
{
	textentrymode = false;
}

void KeyPoll::Poll()
{
	SDL_Event evt;
	while (SDL_PollEvent(&evt))
	{
		/* Keyboard Input */
		if (evt.type == SDL_KEYDOWN)
		{
			keymap[evt.key.keysym.sym] = true;
			if (evt.key.keysym.sym == SDLK_BACKSPACE)
			{
				pressedbackspace = true;
			}
			else if (	(	evt.key.keysym.sym == SDLK_RETURN ||
						evt.key.keysym.sym == SDLK_f	) &&
#ifdef __APPLE__ /* OSX prefers the command key over the alt keys. -flibit */
					keymap[SDLK_LGUI]	)
#else
					(	keymap[SDLK_LALT] ||
						keymap[SDLK_RALT]	)	)
#endif
			{
				toggleFullscreen = true;
			}
		}
		else if (evt.type == SDL_KEYUP)
		{
      if (evt.key.keysym.sym == SDLK_h)
      {
        screen->nextScaleMode();
      }
      
      keymap[evt.key.keysym.sym] = false;
			if (evt.key.keysym.sym == SDLK_BACKSPACE)
			{
				pressedbackspace = false;
			}
		}

		/* Mouse Input */
		else if (evt.type == SDL_MOUSEMOTION)
		{
			mx = evt.motion.x;
			my = evt.motion.y;
		}
		else if (evt.type == SDL_MOUSEBUTTONDOWN)
		{
			if (evt.button.button == SDL_BUTTON_LEFT)
			{
				mx = evt.button.x;
				my = evt.button.y;
				leftbutton = 1;
			}
			else if (evt.button.button == SDL_BUTTON_RIGHT)
			{
				mx = evt.button.x;
				my = evt.button.y;
				rightbutton = 1;
			}
			else if (evt.button.button == SDL_BUTTON_MIDDLE)
			{
				mx = evt.button.x;
				my = evt.button.y;
				middlebutton = 1;
			}
		}
		else if (evt.type == SDL_MOUSEBUTTONUP)
		{
			if (evt.button.button == SDL_BUTTON_LEFT)
			{
				mx = evt.button.x;
				my = evt.button.y;
				leftbutton=0;
			}
			else if (evt.button.button == SDL_BUTTON_RIGHT)
			{
				mx = evt.button.x;
				my = evt.button.y;
				rightbutton=0;
			}
			else if (evt.button.button == SDL_BUTTON_MIDDLE)
			{
				mx = evt.button.x;
				my = evt.button.y;
				middlebutton=0;
			}
		}

		/* Quit Event */
		else if (evt.type == SDL_QUIT)
		{
			quitProgram = true;
		}
	}
}

bool KeyPoll::isDown(int key)
{
	return keymap[(SDLKey)key];
}

bool KeyPoll::isUp(int key)
{
	return !keymap[(SDLKey)key];
}
