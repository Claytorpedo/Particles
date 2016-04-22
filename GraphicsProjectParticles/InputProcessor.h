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
template <typename T>
inline void changeValuesByArrowKey(Input* input, T &val, T small_incr, T large_incr, T min, T max, std::string name, bool log = true ) {
	if ( input->wasKeyPressed( SDLK_DOWN ) ) {
		changeValues( val, -small_incr, min, max, name, log);
	} else if ( input->wasKeyPressed( SDLK_UP ) ) {
		changeValues( val, small_incr, min, max, name, log);
	} else if ( input->wasKeyPressed( SDLK_LEFT ) ) {
		changeValues( val, -large_incr, min, max, name, log);
	} else if ( input->wasKeyPressed( SDLK_RIGHT ) ) {
		changeValues( val, large_incr, min, max, name, log);
	} else if ( input->getMouseWheelValue() != 0 ) {
		changeValues( val, input->getMouseWheelValue() * small_incr, min, max, name, log);
	}
}
inline void changeParticleDimension(int &dimenAdjust, int dimenCurrent, int otherDimen, int incr, int max, bool isWidth = true, bool log = true) {
	int tmp = dimenAdjust + incr;
	int dimen = tmp + dimenCurrent;
	dimenAdjust = dimen < 1 ? (1 - dimenCurrent) : dimen * otherDimen > max ? (max / otherDimen) - dimenCurrent : tmp;
	if (log) {
		int w = isWidth ? dimenAdjust + dimenCurrent : otherDimen;
		int h = isWidth ? otherDimen : dimenAdjust + dimenCurrent;
		std::cout << "Particle system set to: " << w << "x" << h << ".";
		std::cout << " Press Enter to resize particle system with " << w * h << " particles." << std::endl;
	}
}

class InputProcessor {
private:
	Graphics *graphics_;
	Camera *camera_;
	glm::vec2 prev_mouse_left_click_pos_, prev_mouse_right_click_pos_;
	float gravity_dists_[MAX_GRAV_OBJECTS], particle_alpha_;
	int width_, height_;
	int active_grav_obj_;
	const int default_dist_;
public:
	InputProcessor(Graphics* graphics, Camera* camera, float gravDist = DEFAULT_GRAV_DIST, float particleAlpha = DEFAULT_ALPHA ) 
		: graphics_(graphics), camera_(camera), particle_alpha_(particleAlpha), width_(0), height_(0), active_grav_obj_(0), default_dist_(gravDist) {
		for (int i = 0; i < MAX_GRAV_OBJECTS; ++i) {
			gravity_dists_[i] = gravDist;
		}
	}
	~InputProcessor() {}

	void processInput(Input *input, AmbientParticleSystem* particleSystem, int &pointSize, int &cohesiveness, 
					glm::vec4 gravityObjs[MAX_GRAV_OBJECTS], unsigned int activeGravityObjs[MAX_GRAV_OBJECTS]) {
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
		if ( input->wasKeyPressed( SDLK_RETURN ) ) {
			width_ += particleSystem->getWidth();
			height_ += particleSystem->getHeight();
			int numParticles = width_ * height_;
			std::cout << "Reinitializing particle system with " << numParticles << " particle" << (numParticles > 1 ? "s" : "") << "..." << std::endl;
			particleSystem->resize( width_, height_ );
			numParticles = particleSystem->getNumParticles();
			std::cout << "Particle system reinitialized with " << numParticles << " particle" << (numParticles > 1 ? "s" : "") << "." << std::endl;
			width_ = 0;
			height_ = 0;
			return;
		} else if ( input->isKeyHeld( SDLK_LSHIFT ) || input->isKeyHeld( SDLK_RSHIFT ) ) {
			if ( input->wasKeyPressed( SDLK_DOWN ) ) {
				changeParticleDimension(height_, particleSystem->getHeight(), width_ + particleSystem->getWidth(), -PARTICLE_INCR, MAX_PARTICLES, false);
			} else if ( input->wasKeyPressed( SDLK_UP ) ) {
				changeParticleDimension(height_, particleSystem->getHeight(), width_ + particleSystem->getWidth(), +PARTICLE_INCR, MAX_PARTICLES, false);
			} else if ( input->wasKeyPressed( SDLK_LEFT ) ) {
				changeParticleDimension(width_, particleSystem->getWidth(), height_ + particleSystem->getHeight(), -PARTICLE_INCR, MAX_PARTICLES);
			} else if ( input->wasKeyPressed( SDLK_RIGHT ) ) {
				changeParticleDimension(width_, particleSystem->getWidth(), height_ + particleSystem->getHeight(), +PARTICLE_INCR, MAX_PARTICLES);
			}
			return; // Don't do anything else while resizing.
		}
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
			changeValuesByArrowKey( input, cohesiveness, COHESIVENESS_SMALL_INCR, COHESIVENESS_LARGE_INCR, 
				MIN_COHESIVENESS, MAX_COHESIVENESS, "cohesiveness");
		} else if ( input->isKeyHeld( SDLK_LCTRL ) || input->isKeyHeld( SDLK_RCTRL ) ) {
			if ( input->wasKeyPressed( SDLK_BACKSPACE ) ) {
				// Turn off and reset all gravity objects.
				for ( int i = 0; i < MAX_GRAV_OBJECTS; ++i) {
					activeGravityObjs[i] = 0;
					gravityObjs[i] = glm::vec4( 0.0f, 0.0f, 0.0f, DEFAULT_GRAV_FORCE);
					gravity_dists_[i] = default_dist_;
				}
			}
		} else if ( input->wasKeyPressed( SDLK_BACKSPACE ) ) {
			// Turn off all gravity objects.
			for ( int i = 0; i < MAX_GRAV_OBJECTS; ++i) {
				activeGravityObjs[i] = 0;
			}
		}
		// Change particle alpha value.
		if ( input->isKeyHeld( SDLK_a ) ) {
			changeValuesByArrowKey( input, particle_alpha_, ALPHA_SMALL_INCR, ALPHA_LARGE_INCR, MIN_ALPHA, MAX_ALPHA, "alpha");
			particleSystem->setParticleAlpha( particle_alpha_ );
		}
		// Check for change in particle size.
		if ( input->isKeyHeld( SDLK_p ) ) {
			changeValuesByArrowKey( input, pointSize, POINT_SIZE_IRCR, MAX_POINT_SIZE, 
				MIN_POINT_SIZE, MAX_POINT_SIZE, "point size");
		}
		// Check for change in gravity force.
		if ( input->isKeyHeld( SDLK_g ) ) {
			changeValuesByArrowKey( input, gravityObjs[active_grav_obj_].w, GRAV_FORCE_SMALL_INCR, GRAV_FORCE_LARGE_INCR, 
				MIN_GRAV_FORCE, MAX_GRAV_FORCE, "gravity force");
		}
		// Change zoom with keyboard.
		if ( input->isKeyHeld( SDLK_z ) ) {
			float zoom = camera_->getZoom();
			changeValuesByArrowKey( input, zoom, ZOOM_SMALL_INCR, ZOOM_LARGE_INCR, MIN_ZOOM, MAX_ZOOM, "zoom");
			camera_->setZoom(zoom);
		}
		// Change particle colour with keypad keys.
		if ( input->wasKeyPressed( SDLK_KP_0 ) ) {
			particleSystem->setParticleColourNoAlpha( ORANGE );
		} else if ( input->wasKeyPressed( SDLK_KP_1 ) ) {
			particleSystem->setParticleColourNoAlpha( CYAN );
		} else if ( input->wasKeyPressed( SDLK_KP_2 ) ) {
			particleSystem->setParticleColourNoAlpha( VIOLET );
		} else if ( input->wasKeyPressed( SDLK_KP_3 ) ) {
			particleSystem->setParticleColourNoAlpha( INDIGO );
		} else if ( input->wasKeyPressed( SDLK_KP_4 ) ) {
			particleSystem->setParticleColourNoAlpha( OLIVE );
		} else if ( input->wasKeyPressed( SDLK_KP_5 ) ) {
			particleSystem->setParticleColourNoAlpha( YELLOW );
		} else if ( input->wasKeyPressed( SDLK_KP_6 ) ) {
			particleSystem->setParticleColourNoAlpha( BLUE );
		} else if ( input->wasKeyPressed( SDLK_KP_7 ) ) {
			particleSystem->setParticleColourNoAlpha( RED );
		} else if ( input->wasKeyPressed( SDLK_KP_8 ) ) {
			particleSystem->setParticleColourNoAlpha( GREEN );
		} else if ( input->wasKeyPressed( SDLK_KP_9 ) ) {
			particleSystem->setParticleColourNoAlpha( WHITE );
		}

		// Change active gravity object.
		if ( input->wasKeyPressed( SDLK_1 ) ) {
			active_grav_obj_ = 0;
		} else if ( input->wasKeyPressed( SDLK_2 ) ) {
			active_grav_obj_ = 1;
		} else if ( input->wasKeyPressed( SDLK_3 ) ) {
			active_grav_obj_ = 2;
		} else if ( input->wasKeyPressed( SDLK_4 ) ) {
			active_grav_obj_ = 3;
		} else if ( input->wasKeyPressed( SDLK_5 ) ) {
			active_grav_obj_ = 4;
		} else if ( input->wasKeyPressed( SDLK_6 ) ) {
			active_grav_obj_ = 5;
		} else if ( input->wasKeyPressed( SDLK_7 ) ) {
			active_grav_obj_ = 6;
		} else if ( input->wasKeyPressed( SDLK_8 ) ) {
			active_grav_obj_ = 7;
		} else if ( input->wasKeyPressed( SDLK_9 ) ) {
			active_grav_obj_ = 8;
		} else if ( input->wasKeyPressed( SDLK_0 ) ) {
			active_grav_obj_ = 9;
		}

		// Update mouse interactions. 
		if ( input->isKeyHeld( SDLK_LCTRL ) || input->isKeyHeld( SDLK_RCTRL ) ) {
			if ( input->wasKeyPressed( SDLK_r ) ) {
				camera_->reset();
				for (int i = 0; i < MAX_GRAV_OBJECTS; ++i ) {
					gravity_dists_[i] = default_dist_;
					if ( activeGravityObjs[i] == 0 ) {
						gravityObjs[i].w = DEFAULT_GRAV_FORCE;
					}
				}
			}
			// Update rotation.
			if ( input->wasMouseButtonPressed( SDL_BUTTON_LEFT ) ) {
				int x, y;
				SDL_GetMouseState( &x, &y );
				prev_mouse_left_click_pos_ = glm::vec2(x, y);
			} else if ( input->isMouseButtonHeld( SDL_BUTTON_LEFT ) ) {
				int x, y;
				SDL_GetMouseState( &x, &y );
				int delta_x = x - prev_mouse_left_click_pos_.x; // y-axis rotation.
				int delta_y = y - prev_mouse_left_click_pos_.y; // x-axis rotation.
				camera_->rotate( delta_x, delta_y );
				prev_mouse_left_click_pos_.x = x;
				prev_mouse_left_click_pos_.y = y;
			}
			// Update pan.
			if ( input->wasMouseButtonPressed( SDL_BUTTON_RIGHT ) ) {
				int x, y;
				SDL_GetMouseState( &x, &y );
				prev_mouse_right_click_pos_ = glm::vec2(x, y);
			} else if ( input->isMouseButtonHeld( SDL_BUTTON_RIGHT ) ) {
				int x, y;
				SDL_GetMouseState( &x, &y );
				int delta_x = x - prev_mouse_right_click_pos_.x;
				int delta_y = y - prev_mouse_right_click_pos_.y;
				camera_->pan( delta_x, delta_y );
				prev_mouse_right_click_pos_.x = x;
				prev_mouse_right_click_pos_.y = y;
			}
			// Update zoom.
			float zoom = input->getMouseWheelValue();
			if ( zoom != 0 ) {
				zoom *= ZOOM_MOUSE_INCR;
				// Sync gravity object distance with zoom (this seems more intuitive).
				float dist = gravity_dists_[active_grav_obj_] - zoom;
				gravity_dists_[active_grav_obj_] = dist > MAX_GRAV_DIST ? MAX_GRAV_DIST : dist < MIN_GRAV_DIST ? MIN_GRAV_DIST : dist;
				zoom += camera_->getZoom();
				zoom = zoom > MAX_ZOOM ? MAX_ZOOM : zoom < MIN_ZOOM ? MIN_ZOOM : zoom;
				camera_->setZoom( zoom );
			}
		} else { // Control is not held. Mouse actions effect gravity object placement.
			if ( input->wasMouseButtonPressed( SDL_BUTTON_LEFT ) ) {
				int x, y;
				SDL_GetMouseState( &x, &y );
				Ray r = camera_->getRay(x, y);
				glm::vec3 pos = r.position + gravity_dists_[active_grav_obj_] * r.direction;
				gravityObjs[active_grav_obj_] = glm::vec4( pos, gravityObjs[active_grav_obj_].w );
				activeGravityObjs[active_grav_obj_] = 1;
				std::cout << "gravity position < x: " << gravityObjs[active_grav_obj_].x << " y: ";
				std::cout << gravityObjs[active_grav_obj_].y << " z: " << gravityObjs[active_grav_obj_].z << " >" << std::endl;
			} else if ( input->isMouseButtonHeld( SDL_BUTTON_LEFT ) ) {
				int x, y;
				SDL_GetMouseState( &x, &y );
				Ray r = camera_->getRay(x, y);
				glm::vec3 pos = r.position + gravity_dists_[active_grav_obj_] * r.direction;
				gravityObjs[active_grav_obj_] = glm::vec4( pos, gravityObjs[active_grav_obj_].w );
				activeGravityObjs[active_grav_obj_] = 1;
			} else if ( input->wasMouseButtonPressed( SDL_BUTTON_RIGHT ) ) {
				activeGravityObjs[active_grav_obj_] = 0;
			}

			// Update gravity object distance from camera.
			// Especially since there is no visualization, this isn't very intuitive, so I'm hiding it behind the alt key for now.
			if ( input->isKeyHeld( SDLK_RALT ) || input->isKeyHeld( SDLK_LALT ) ) {
				float dist = input->getMouseWheelValue();
				if ( dist != 0 ) {
					dist *= GRAV_DIST_MOUSE_INCR;
					dist += gravity_dists_[active_grav_obj_];
					gravity_dists_[active_grav_obj_] = dist > MAX_GRAV_DIST ? MAX_GRAV_DIST : dist < MIN_GRAV_DIST ? MIN_GRAV_DIST : dist;
				}
				if ( input->wasKeyPressed( SDLK_r ) ) {
					gravity_dists_[active_grav_obj_] = default_dist_;
				}
			}
		}

		// Log some data.
		if ( input->wasKeyPressed( SDLK_l ) ) {
			for (int i = 0; i < MAX_GRAV_OBJECTS; ++i ) {
				std::cout << i << " active: " << activeGravityObjs[i] <<  " gravity position < x: " << gravityObjs[i].x << " y: ";
				std::cout << gravityObjs[i].y << " z: " << gravityObjs[i].z << " >" << " force: " << gravityObjs[i].w << std::endl;
			}
		}
	}
};

#endif // _INPUT_PROCESSOR_H