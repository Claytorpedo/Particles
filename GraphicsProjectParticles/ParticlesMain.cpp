#include "SDL2/SDL.h"
#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"
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

#include "glm/glm.hpp"

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
	glm::vec4 gravityObjs[MAX_GRAV_OBJECTS]; 
	unsigned int activeGravObjs[MAX_GRAV_OBJECTS];

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

	// Initialize gravity objects.
	for (int i = 0; i < MAX_GRAV_OBJECTS; ++i) {
		activeGravObjs[i] = 0;
		gravityObjs[i].w = DEFAULT_GRAV_FORCE;
	}
	gravityObjs[0] = glm::vec4( DEFAULT_GRAV_POS, DEFAULT_GRAV_FORCE );
	activeGravObjs[0] = 1;

	previousTime = SDL_GetTicks();
	while (!quit) {
		if ( getInput(&input, &graphics, &camera) || input.wasKeyPressed(SDLK_ESCAPE) ) {
			break;
		}
		inputProcessor.processInput( &input, &particleSystem, pointSize, cohesiveness, gravityObjs, activeGravObjs );

		// Update the scene.
		currentTime = SDL_GetTicks();
		elapsedTime = currentTime - previousTime;
		elapsedTime = elapsedTime < MAX_FRAME_DUR ? elapsedTime : MAX_FRAME_DUR;
		previousTime = currentTime;
		particleSystem.update( elapsedTime, gravityObjs, activeGravObjs, cohesiveness );

		// Draw the scene.
		graphics.clear();
		particleSystem.draw( camera.getProjectionView(), pointSize );
		graphics.present();
	}
	close();
	return 0;
}