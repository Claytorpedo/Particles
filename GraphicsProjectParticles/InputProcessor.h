#ifndef _INPUT_PROCESSOR_H
#define	_INPUT_PROCESSOR_H

#include "Camera.h"
#include "Graphics.h"
#include "Input.h"
#include "AmbientParticleSystem.h"
#include "Constants.h"

#include "glm\glm.hpp"

#include <cstdio>

using namespace constants;

class InputProcessor {
private:
	Graphics *graphics_;
	Camera *camera_;

public:
	InputProcessor(Graphics* graphics, Camera* camera) : graphics_(graphics), camera_(camera) {}
	~InputProcessor() {}

	void processInput(Input *input, AmbientParticleSystem* particleSystem, unsigned int &pointSize, int &cohesiveness, glm::vec4 &gravity) {
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
				float tmp = cohesiveness - COHESIVENESS_SMALL_INCR;
				cohesiveness = tmp > MIN_COHESIVENESS ? tmp : MIN_COHESIVENESS;
			} else if ( input->wasKeyPressed( SDLK_UP ) ) {
				float tmp = cohesiveness + COHESIVENESS_SMALL_INCR;
				cohesiveness = tmp < MAX_COHESIVENESS ? tmp : MAX_COHESIVENESS;
			} else if ( input->wasKeyPressed( SDLK_LEFT ) ) {
				float tmp = cohesiveness - COHESIVENESS_LARGE_INCR;
				cohesiveness = tmp > MIN_COHESIVENESS ? tmp : MIN_COHESIVENESS;
			} else if ( input->wasKeyPressed( SDLK_RIGHT ) ) {
				float tmp = cohesiveness + COHESIVENESS_LARGE_INCR;
				cohesiveness = tmp < MAX_COHESIVENESS ? tmp : MAX_COHESIVENESS;
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
				pointSize = pointSize > MIN_POINT_SIZE ? pointSize - POINT_SIZE_IRCR : MIN_POINT_SIZE;
			} else if ( input->wasKeyPressed( SDLK_UP ) ) {
				pointSize = pointSize < MAX_POINT_SIZE ? pointSize + POINT_SIZE_IRCR : MAX_POINT_SIZE;
			} else if ( input->wasKeyPressed( SDLK_LEFT ) ) {
				pointSize = MIN_POINT_SIZE;
			} else if ( input->wasKeyPressed( SDLK_RIGHT ) ) {
				pointSize = MAX_POINT_SIZE;
			}
		}
		// Check for change in gravity force.
		if ( input->isKeyHeld( SDLK_g ) ) {
			if ( input->wasKeyPressed( SDLK_DOWN ) ) {
				float tmp = gravity.w - GRAV_FORCE_SMALL_INCR;
				gravity.w = tmp > MIN_GRAV_FORCE ? tmp : MIN_GRAV_FORCE;
			} else if ( input->wasKeyPressed( SDLK_UP ) ) {
				float tmp = gravity.w + GRAV_FORCE_SMALL_INCR;
				gravity.w = tmp < MAX_GRAV_FORCE ? tmp : MAX_GRAV_FORCE;
			} else if ( input->wasKeyPressed( SDLK_LEFT ) ) {
				float tmp = gravity.w - GRAV_FORCE_BIG_INCR;
				gravity.w = tmp > MIN_GRAV_FORCE ? tmp : MIN_GRAV_FORCE;
			} else if ( input->wasKeyPressed( SDLK_RIGHT ) ) {
				float tmp = gravity.w + GRAV_FORCE_BIG_INCR;
				gravity.w = tmp < MAX_GRAV_FORCE ? tmp : MAX_GRAV_FORCE;
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
			}
		}
	}

};

#endif // _INPUT_PROCESSOR_H