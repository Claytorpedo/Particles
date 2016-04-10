#ifndef _INPUT_H
#define _INPUT_H

#include <map>
#include "SDL2\SDL.h"


class Input {

private:
	std::map<SDL_Keycode, bool> heldKeys;
	std::map<SDL_Keycode, bool> pressedKeys;
	std::map<SDL_Keycode, bool> releasedKeys;
public:
	Input() {}	// empty constructor
	~Input() {}	// empty destructor

	void newFrameClear(); // clear maps for new frame

	void keyDownEvent( const SDL_Event& e);
	void keyUpEvent( const SDL_Event& e);

	// return whether keys were held/pressed/released
	bool isKeyHeld(SDL_Keycode key);
	bool wasKeyPressed(SDL_Keycode key);
	bool wasKeyReleased(SDL_Keycode key);
};


#endif // _INPUT_H