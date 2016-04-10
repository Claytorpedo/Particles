#include "Input.h"
#include "SDL2\SDL.h"

void Input::newFrameClear() {
	pressedKeys.clear();
	releasedKeys.clear();
}
void Input::keyDownEvent( const SDL_Event& e) {
	pressedKeys[e.key.keysym.sym] = true;
	heldKeys[e.key.keysym.sym] = true;
}
void Input::keyUpEvent( const SDL_Event& e) {
	releasedKeys[e.key.keysym.sym] = true;
	heldKeys[e.key.keysym.sym] = false;
}


// When trying to get a bool that doesn't exist, a map defaults to false.
bool Input::isKeyHeld(SDL_Keycode key) {
	return heldKeys[key];
}
// When trying to get a bool that doesn't exist, a map defaults to false.
bool Input::wasKeyPressed(SDL_Keycode key) {
	return pressedKeys[key];
}
// When trying to get a bool that doesn't exist, a map defaults to false.
bool Input::wasKeyReleased(SDL_Keycode key) {
	return releasedKeys[key];
}