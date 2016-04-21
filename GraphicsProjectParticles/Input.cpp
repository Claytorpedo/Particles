#include "Input.h"
#include "SDL2\SDL.h"

void Input::newFrameClear() {
	pressedKeys.clear();
	releasedKeys.clear();
	pressedMouseButtons.clear();
	releasedMouseButtons.clear();
	mouse_wheel_value_ = 0;
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


void Input::mouseDownEvent( const SDL_Event& e) {
	pressedMouseButtons[e.button.button] = true;
	heldMouseButtons[e.button.button] = true;
}
void Input::mouseUpEvent( const SDL_Event& e) {
	releasedMouseButtons[e.button.button] = true;
	heldMouseButtons[e.button.button] = false;
}

// When trying to get a bool that doesn't exist, a map defaults to false.
bool Input::isMouseButtonHeld(Uint8 button) {
	return heldMouseButtons[button];
}
// When trying to get a bool that doesn't exist, a map defaults to false.
bool Input::wasMouseButtonPressed(Uint8 button) {
	return pressedMouseButtons[button];
}
// When trying to get a bool that doesn't exist, a map defaults to false.
bool Input::wasMouseButtonReleased(Uint8 button) {
	return releasedMouseButtons[button];
}


void Input::mouseWheelEvent(const SDL_Event& e) {
	mouse_wheel_value_ += e.wheel.y;
}
int Input::getMouseWheelValue() {
	return mouse_wheel_value_;
}