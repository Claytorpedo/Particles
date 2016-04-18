#include "SDL2\SDL.h"
#include "GL\glew.h"
#include "SDL2\SDL_opengl.h"
#include <iostream>
#include <string>

#include "Units.h"
#include "Constants.h"
#include "Graphics.h"
#include "ShaderProgram.h"
#include "AmbientParticleSystem.h"
#include "Input.h"
#include "Camera.h"

#include "glm\glm.hpp"
#include "glm\gtx\transform.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\matrix_inverse.hpp"

using namespace constants;

AmbientParticleSystem* particleSystem = NULL;
Camera *camera;
unsigned int pointSize = DEFAULT_POINT_SIZE;
unsigned int gravForce = DEFAULT_GRAV_FORCE;
unsigned int width = DEFAULT_PARTICLE_EXPONENT;
unsigned int height = DEFAULT_PARTICLE_EXPONENT;

void close () {
	delete particleSystem;
	SDL_Quit();
}
// Returns true if the user closed the window.
bool getInput(Input *input) {
	input->newFrameClear();
	SDL_Event e;
	// get all the events
	while (SDL_PollEvent( &e ) ) {
		switch(e.type) {
		case SDL_KEYDOWN:
			input->keyDownEvent( e );
			break;
		case SDL_KEYUP:
			input->keyUpEvent( e );
			break;
		case SDL_MOUSEBUTTONDOWN:
			input->mouseDownEvent( e );
			break;
		case SDL_MOUSEBUTTONUP:
			input->mouseUpEvent( e );
			break;
		case SDL_MOUSEWHEEL:
			input->mouseWheelEvent( e );
			break;
		case SDL_QUIT:		// user closes window
			return true;
			break;
		default:
			break;
		}
	}
	return false;
}
void processInput(Input *input, Graphics *graphics) {
	// Change colours because why not.
	if ( input->isKeyHeld( SDLK_c ) ) {
		if ( input->wasKeyPressed( SDLK_r ) ) {
			graphics->setClearColour(0.7f,0.3f,0.3f,1);
		} else if ( input->wasKeyPressed( SDLK_g ) ) {
			graphics->setClearColour(0.3f,0.7f,0.3f,1);
		} else if ( input->wasKeyPressed( SDLK_b ) ) {
			graphics->setClearColour(0.3f,0.3f,0.7f,1);
		} else if ( input->wasKeyPressed( SDLK_BACKSPACE ) ) {
			graphics->setClearColour(0,0,0,1);
		}
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
			gravForce = gravForce > MIN_GRAV_FORCE ? gravForce - GRAV_FORCE_INCR : MIN_GRAV_FORCE;
		} else if ( input->wasKeyPressed( SDLK_UP ) ) {
			gravForce = gravForce < MAX_GRAV_FORCE ? gravForce + GRAV_FORCE_INCR : MAX_GRAV_FORCE;
		} else if ( input->wasKeyPressed( SDLK_LEFT ) ) {
			gravForce = MIN_GRAV_FORCE;
		} else if ( input->wasKeyPressed( SDLK_RIGHT ) ) {
			gravForce = MAX_GRAV_FORCE;
		}
	}

	// Update mouse interactions.
	
	if ( input->isKeyHeld( SDLK_LCTRL ) || input->isKeyHeld( SDLK_RCTRL ) ) {
		
		


		int x, y;
		SDL_GetMouseState( &x, &y );
	}
}

int main (int argc, char* args[]) {
	bool quit = false;
	Input input;
	units::MS currentTime, previousTime, elapsedTime;
	Graphics graphics( WINDOW_TITLE, GL_MAJOR_VER, GL_MINOR_VER );
	glm::vec3 pos = glm::vec3(0,0,5);
	glm::vec3 look = glm::vec3(0,0,-1);
	camera = new Camera(pos, pos + look, glm::vec3(0,1,0), FOV, ASPECT, NEAR, FAR, SCREEN_WIDTH, SCREEN_HEIGHT);

	//glm::mat4 projection = glm::perspective(glm::radians(FOV), ASPECT, NEAR, FAR);
	//glm::mat4 view = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,0,-1), glm::vec3(0,1,0));
	//glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.5f, -2.0f));
	//glm::mat4 PVM = projection * view * model;

	if ( !graphics.init() ) {
		std::cerr << "Error: failed to initialize graphics!" << std::endl;
		getchar();
		close();
		return 1;
	}
	particleSystem = new AmbientParticleSystem( constants::DEFAULT_PARTICLE_EXPONENT );
	if ( !particleSystem->init() ) {
		std::cerr << "Error: Failed to initialize particle system!" << std::endl;
		getchar();
		close();
		return 1;
	}
	glm::vec3 gravPos( 2.0f, 2.0f, -1.0f );

	previousTime = SDL_GetTicks();
	while (!quit) {
		if ( getInput(&input) || input.wasKeyPressed(SDLK_ESCAPE) ) {
			break;
		}
		processInput(&input, &graphics);

		if ( input.wasMouseButtonPressed( SDL_BUTTON_LEFT ) ) {
			int x, y;
			SDL_GetMouseState( &x, &y );
			Ray r = camera->getRay(x, y);
			gravPos = r.position + 10.0f * r.direction;
		}

		// Update the scene.

		currentTime = SDL_GetTicks();
		elapsedTime = currentTime - previousTime;
		elapsedTime = elapsedTime < MAX_FRAME_DUR ? elapsedTime : MAX_FRAME_DUR;
		previousTime = currentTime;
		particleSystem->update( elapsedTime, gravPos, gravForce );

		// Draw the scene.

		graphics.clear();

		particleSystem->draw( camera->getProjectionView(), pointSize );

		graphics.present();
	}
	close();
	return 0;
}
/*
	Ideas: Make my bajillion particles
			Make it so that they gravitate towards a 3D mouse position
			Make variable gravity and such.
			Make the ability to place more than one gravity spot, indicated by an object/light or something.
			Add larger secondary particles.
				The small particles bounce off of these ones
				If a larger one hits a larger one, they explode like fireworks or something.



This if I want to texture things maybe? :
"""
In core OpenGL 3.2 and above, geometry shaders are available for use. To give a brief overview, you would only tell OpenGL to 
draw the points that represent the positions of your particles, and these points would pass through the vertex shader normally,
but then when they get to the geometry shader, it constructs the rectangle (out of two tris) by emitting vertices to the next 
stage of the pipeline. Along with the vertices you can programmatically generate normals and texture coordinates.

Since this is all done in hardware, I'm pretty sure it's the fastest way right now to render point-sprite particles.
"""

https://www.packtpub.com/books/content/basics-glsl-40-shaders
Passing arrays or structures to a function

It should be noted that when passing arrays or structures to functions, they are passed by value. 
If a large array or structure is passed, it can incur a large copy operation which may not be desired. 
It would be a better choice to declare these variables in the global scope.


	Create vertices randomly with normal distribution around origin. 

	If things are looking bad, then go take a look at CUDA quick.

	http://www.hackbarth-gfx.com/2013/03/17/making-of-1-million-particles/
	This might be doing velocity and stuff with textures.

	https://github.com/nopjia/webgl-particles/tree/master/app
	Another million thing


	Set particle colour based on speed.
*/