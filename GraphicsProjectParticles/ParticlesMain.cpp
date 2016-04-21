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
#include "InputProcessor.h"
#include "Camera.h"

#include "glm\glm.hpp"

using namespace constants;

void close () {
	SDL_Quit();
}
// Returns true if the user closed the window.
bool getInput(Input *input, Graphics *graphics, Camera *camera) {
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
		case SDL_WINDOWEVENT:
			if ( e.window.event == SDL_WINDOWEVENT_RESIZED ) {
				camera->resize(e.window.data1, e.window.data2);
				graphics->setViewport(0,0,e.window.data1, e.window.data2);
			}
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

int main (int argc, char* args[]) {
	bool quit = false;
	units::MS currentTime, previousTime, elapsedTime;
	glm::vec4 gravity( -3, -3, 0, DEFAULT_GRAV_FORCE );
	int pointSize = DEFAULT_POINT_SIZE;
	int cohesiveness = DEFAULT_COHESIVENESS;
	glm::vec3 origin( 3.0f, 3.0f, 0.0f );
	glm::vec3 position(0.0f, 0.0f, DEFAULT_ZOOM);
	glm::vec3 rotation(0.0f,0.0f,0.0f);
	Camera camera(origin, position, rotation, FOV, DEFAULT_ASPECT, NEAR, FAR, DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
	Graphics graphics( WINDOW_TITLE, GL_MAJOR_VER, GL_MINOR_VER );
	AmbientParticleSystem particleSystem( DEFAULT_PARTICLE_DIMENSIONS, glm::vec4(ORANGE, DEFAULT_ALPHA) );
	Input input;
	InputProcessor inputProcessor( &graphics, &camera );

	if ( !graphics.init() ) {
		std::cerr << "Error: failed to initialize graphics!" << std::endl;
		getchar();
		close();
		return 1;
	}
	if ( !particleSystem.init() ) {
		std::cerr << "Error: Failed to initialize particle system!" << std::endl;
		getchar();
		close();
		return 1;
	}
	std::cout << "Particle system initialized with " << particleSystem.getNumParticles() << " particles." << std::endl;
	previousTime = SDL_GetTicks();
	while (!quit) {
		if ( getInput(&input, &graphics, &camera) || input.wasKeyPressed(SDLK_ESCAPE) ) {
			break;
		}
		inputProcessor.processInput( &input, &particleSystem, pointSize, cohesiveness, gravity );
		// Update the scene.
		currentTime = SDL_GetTicks();
		elapsedTime = currentTime - previousTime;
		elapsedTime = elapsedTime < MAX_FRAME_DUR ? elapsedTime : MAX_FRAME_DUR;
		previousTime = currentTime;
		particleSystem.update( elapsedTime, gravity, cohesiveness );

		// Draw the scene.

		graphics.clear();

		particleSystem.draw( camera.getProjectionView(), pointSize );

		graphics.present();
	}
	close();
	return 0;
}
/*

SOME KIND OF DEPTH VISUALIZATION

MULTIPLE GRAV OBJECTS




	Ideas: Make my bajillion particles
			Make it so that they gravitate towards a 3D mouse position
			Make variable gravity and such.
			Make the ability to place more than one gravity spot, indicated by an object/light or something.
			Add larger secondary particles.
				The small particles bounce off of these ones
				If a larger one hits a larger one, they explode like fireworks or something.

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