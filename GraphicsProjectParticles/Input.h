#ifndef _INPUT_H
#define _INPUT_H

#include <unordered_map>
#include "SDL2\SDL.h"

class Input {

private:
	std::unordered_map<SDL_Keycode, bool> heldKeys;
	std::unordered_map<SDL_Keycode, bool> pressedKeys;
	std::unordered_map<SDL_Keycode, bool> releasedKeys;

	std::unordered_map<Uint8, bool> heldMouseButtons;
	std::unordered_map<Uint8, bool> pressedMouseButtons;
	std::unordered_map<Uint8, bool> releasedMouseButtons;

	int mouse_wheel_value_;
public:
	Input() : mouse_wheel_value_(0) {}
	~Input() {}	

	void newFrameClear(); // clear maps for new frame

	void keyDownEvent( const SDL_Event& e);
	void keyUpEvent( const SDL_Event& e);

	bool isKeyHeld(SDL_Keycode key);
	bool wasKeyPressed(SDL_Keycode key);
	bool wasKeyReleased(SDL_Keycode key);


	void mouseDownEvent( const SDL_Event& e);
	void mouseUpEvent( const SDL_Event& e);

	bool isMouseButtonHeld(Uint8 button);
	bool wasMouseButtonPressed(Uint8 button);
	bool wasMouseButtonReleased(Uint8 button);

	void mouseWheelEvent(const SDL_Event& e);
	int  mouseWheelValue();
	void resetMouseWheelValue();
};


#endif // _INPUT_H