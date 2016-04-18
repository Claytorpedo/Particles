#ifndef _INPUT_PROCESSOR_H
#define	_INPUT_PROCESSOR_H

#include "Camera.h"
#include "Graphics.h"
#include "Input.h"
#include "AmbientParticleSystem.h"
#include "Constants.h"

#include "glm\glm.hpp"

#include <cstdio>
#include <string>

using namespace constants;

template <typename T>
inline void changeValues(T &val, T incr, T min, T max, std::string name, bool log = true) {
	T tmp = val + incr;
	val = tmp < min ? min : tmp > max ? max : tmp;
	if (log) { std::cout << name << ": " << val << std::endl; }
}

class InputProcessor {
private:
	Graphics *graphics_;
	Camera *camera_;

public:
	InputProcessor(Graphics* graphics, Camera* camera) : graphics_(graphics), camera_(camera) {}
	~InputProcessor() {}

	void processInput(Input *input, AmbientParticleSystem* particleSystem, int &pointSize, int &cohesiveness, glm::vec4 &gravity) {
		// Change background colours because why not.
		if ( input->isKeyHeld( SDLK_c ) ) {
			if ( input->wasKeyPressed( SDLK_r ) ) {
				graphics_->setClearColour(0.7f,0.3f,0.3f,1);
			} else if ( input->wasKeyPressed( SDLK_g ) ) {
				graphics_->setClearColour(0.3f,0.7f,0.3f,1);
			} else if ( input->wasKeyPressed( SDLK_b ) ) {
				graphics_->setClearColour(0.3f,0.3f,0.7f,1);
			} else if ( input->wasKeyPressed( SDLK_BACKSPACE ) ) {
				graphics_->setClearColour(0,0,0,1);
			}
		// Check for change in cohesiveness. Using c again here doesn't seem like it should cause slips.
			if ( input->wasKeyPressed( SDLK_DOWN ) ) {
				changeValues( cohesiveness, -COHESIVENESS_SMALL_INCR, MIN_COHESIVENESS, MAX_COHESIVENESS, "cohesiveness");
			} else if ( input->wasKeyPressed( SDLK_UP ) ) {
				changeValues( cohesiveness, COHESIVENESS_SMALL_INCR, MIN_COHESIVENESS, MAX_COHESIVENESS, "cohesiveness");
			} else if ( input->wasKeyPressed( SDLK_LEFT ) ) {
				changeValues( cohesiveness, -COHESIVENESS_LARGE_INCR, MIN_COHESIVENESS, MAX_COHESIVENESS, "cohesiveness");
			} else if ( input->wasKeyPressed( SDLK_RIGHT ) ) {
				changeValues( cohesiveness, COHESIVENESS_LARGE_INCR, MIN_COHESIVENESS, MAX_COHESIVENESS, "cohesiveness");
			}
		}
		if ( input->wasKeyPressed( SDLK_SPACE ) ) {
			particleSystem->togglePause();
		}
		// Have do get this on release, due to what apepars to be an SDL2 bug.
		// Otherwise inputs repeat when switching to fullscreen (which results in switching back).
		if ( input->wasKeyReleased( SDLK_F11 ) ) {
			graphics_->toggleFullscreen();
			int w = DEFAULT_SCREEN_WIDTH, h = DEFAULT_SCREEN_HEIGHT;
			graphics_->getWindowSize(w, h);
			graphics_->setViewport(0,0,w,h);
			camera_->resize(w,h);
		}
		// Check for change in particle size.
		if ( input->isKeyHeld( SDLK_p ) ) {
			if ( input->wasKeyPressed( SDLK_DOWN ) ) {
				changeValues( pointSize, -POINT_SIZE_IRCR, MIN_POINT_SIZE, MAX_POINT_SIZE, "point size");
			} else if ( input->wasKeyPressed( SDLK_UP ) ) {
				changeValues( pointSize, POINT_SIZE_IRCR, MIN_POINT_SIZE, MAX_POINT_SIZE, "point size");
			} else if ( input->wasKeyPressed( SDLK_LEFT ) ) {
				changeValues( pointSize, -MAX_POINT_SIZE, MIN_POINT_SIZE, MAX_POINT_SIZE, "point size");
			} else if ( input->wasKeyPressed( SDLK_RIGHT ) ) {
				changeValues( pointSize, MAX_POINT_SIZE, MIN_POINT_SIZE, MAX_POINT_SIZE, "point size");
			}
		}
		// Check for change in gravity force.
		if ( input->isKeyHeld( SDLK_g ) ) {
			if ( input->wasKeyPressed( SDLK_DOWN ) ) {
				changeValues( gravity.w, -GRAV_FORCE_SMALL_INCR, MIN_GRAV_FORCE, MAX_GRAV_FORCE, "gravity force");
			} else if ( input->wasKeyPressed( SDLK_UP ) ) {
				changeValues( gravity.w, +GRAV_FORCE_SMALL_INCR, MIN_GRAV_FORCE, MAX_GRAV_FORCE, "gravity force");
			} else if ( input->wasKeyPressed( SDLK_LEFT ) ) {
				changeValues( gravity.w, -GRAV_FORCE_LARGE_INCR, MIN_GRAV_FORCE, MAX_GRAV_FORCE, "gravity force");
			} else if ( input->wasKeyPressed( SDLK_RIGHT ) ) {
				changeValues( gravity.w, GRAV_FORCE_LARGE_INCR, MIN_GRAV_FORCE, MAX_GRAV_FORCE, "gravity force");
			}
		}
		// Update mouse interactions.
		if ( input->isKeyHeld( SDLK_LCTRL ) || input->isKeyHeld( SDLK_RCTRL ) ) {
			// Pan and rotate camera. Zoom too?

		} else {
			if ( input->wasMouseButtonPressed( SDL_BUTTON_LEFT ) ) {
				int x, y;
				SDL_GetMouseState( &x, &y );
				Ray r = camera_->getRay(x, y);
				glm::vec3 pos = r.position + 10.0f * r.direction;
				gravity = glm::vec4( pos, gravity.w );
				std::cout << "gravity position < x: " << gravity.x << " y: " << gravity.y << " z: " << gravity.z << " >" << std::endl;
			}
		}
	}

};

#endif // _INPUT_PROCESSOR_H