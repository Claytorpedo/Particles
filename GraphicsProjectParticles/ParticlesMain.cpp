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

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

using namespace constants;

ShaderProgram* basicShader = NULL;
AmbientParticleSystem* particleSystem = NULL;

void close () {
	delete basicShader;
	delete particleSystem;
	SDL_Quit();
}
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
	if ( input->wasKeyPressed( SDLK_r ) ) {
		graphics->setClearColour(0.7,0.3,0.3,1);
	} else if ( input->wasKeyPressed( SDLK_g ) ) {
		graphics->setClearColour(0,1,0,1);
	} else if ( input->wasKeyPressed( SDLK_b ) ) {
		graphics->setClearColour(0,0,1,1);
	} else if ( input->wasKeyPressed( SDLK_BACKSPACE ) ) {
		graphics->setClearColour(0,0,0,1);
	}

}

int main (int argc, char* args[]) {
	bool quit = false;
	Input input;
	units::MS currentTime, previousTime, elapsedTime;
	Graphics graphics( WINDOW_TITLE, GL_MAJOR_VER, GL_MINOR_VER );

	if ( !graphics.init() ) {
		std::cerr << "Error: failed to initialize graphics!" << std::endl;
		getchar();
		close();
		return 1;
	}
	basicShader = new ShaderProgram(BASIC_SHADER_VERT_PATH, BASIC_SHADER_FRAG_PATH);
	if ( !basicShader->load() ) {
		std::cerr << "Error: failed to create shaders!" << std::endl;
		getchar();
		close();
		return 1;
	}
	particleSystem = new AmbientParticleSystem(constants::DEFAULT_PARTICLE_EXPONENT);
	if ( !particleSystem->init() ) {
		std::cerr << "Error: failed to initialize particle system!" << std::endl;
		getchar();
		close();
		return 1;
	}
	
	const GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	GLuint vertexArrayID;
	glGenVertexArrays( 1, &vertexArrayID );
	glBindVertexArray( vertexArrayID );

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Get a handle for the mvp uniform (our uniform called "MVP").
	GLuint PVM_ID = glGetUniformLocation( basicShader->getProgramID(), "uPVM");
	

	// Get view projection.
	glm::mat4 projection = glm::perspective(glm::radians(FOV), ASPECT, NEAR, FAR);
	glm::mat4 view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Move back 3, look at origin, and y is up.
	glm::mat4 model = glm::mat4(1.0f); // Just put the triangle at origin.
	glm::mat4 PVM = projection * view * model;

	glm::vec3 gravPos( -2, 0, 0 );
	
	// start runtime loop
	previousTime = SDL_GetTicks();
	while (!quit) {
		quit = getInput( &input );
		if ( quit ) { // continue to end
		} else if ( input.wasKeyPressed( SDLK_ESCAPE ) ) {
			quit = true;
		} else {
			processInput( &input, &graphics );

			currentTime = SDL_GetTicks();
			elapsedTime = currentTime - previousTime;

			//particleSystem->update( elapsedTime, gravPos );
			
			previousTime = currentTime;

			graphics.clear();
			
			particleSystem->draw( PVM );
			//particleSystem->drawTexture( 0 );


			/*
			basicShader->use();
			// Then for each object we draw, send it the adress of that mat4 under that handle.
			// This sends it to the currently bound shader.
			glUniformMatrix4fv( PVM_ID, 1, GL_FALSE, &PVM[0][0] );

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer );
			glVertexAttribPointer( 
				0 ,			// Attribute 0. Number isn't important other than it must match the shader layout.
				3,			// size
				GL_FLOAT,	// type
				GL_FALSE,	// normalized?
				0,			// stride
				(void*)0 );	// Array buffer offset
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glDisableVertexAttribArray(0);
			basicShader->stopUsing();
			*/
			graphics.present();
		}
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



// THIS IS HOW YOU SET UP INPUT TO THE VERTEX SHADER
	// I'll want to set this up as some kind of parameter for this shader builder function
	 Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color
     Attribute locations must be setup before calling glLinkProgram.
    //glBindAttribLocation(ProgramID, 0, "in_Position");
    //glBindAttribLocation(ProgramID, 1, "in_Color");
Then in vert shader
// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
in  vec2 in_Position;
in  vec3 in_Color;



	Create vertices randomly with normal distribution around origin. 

	If things are looking bad, then go take a look at CUDA quick.

	http://www.hackbarth-gfx.com/2013/03/17/making-of-1-million-particles/
	This might be doing velocity and stuff with textures.

	https://github.com/nopjia/webgl-particles/tree/master/app
	Another million thing


	Set particle colour based on speed.

	Use float textures. Particles are represented by UV coords into the textures.
	The state textures use nearest filtering (so that you don't get any weighted average).

	Can't write to the same textures reading from, so need a dupe set of framebuffers and textures to swap between each frame.
	Draw num of particles based on the compute textures size (so 1024x1024 is over 1m particles).

	Each particles is just represented then by a UV coordinate given to the vert shader, where it can use the UV
	to look up position and colour (and velocity in the computation shader, which isn't used to draw things to the screen I don't think).

	He uses a fixed time step, but I should be able to pass in a uniform elapsed time variable.

	SHADERS:
		Init shader: place the particles positions, velocities, and colours. Everything fancy happens in the frag shader, it uses the same vert shader as ComputeShader
			If I can't think of a good way to input a bunch of random numbers for this initialization, I can use
			http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
			float snoise(vec2 co){
				return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
			}

			void main(void)
			{
				float n = snoise(vec2(textureCoord.x,textureCoord.y)); 
				gl_FragColor = vec4(n, n, n, 1.0 );
			}
			to get some randomness.
			NOTE: According to http://stackoverflow.com/questions/3956478/understanding-randomness/3956538#3956538
			it appears that if I add random numbers together I'll tend to get a uniform distribution
				as in snoise + snoise + snoise as long as I have different seeds for each one somehow...
					Maybe I should make different functions and use them all
					can also look at https://github.com/ashima/webgl-noise/tree/master/src
			
		Compute shader: This shader updates the particle properties for a frame
				vert is really simple just takes a position
				frag is where the magic happens. Outputs pos and vel (also has an unused one in example)
				
		Draw shader: actually draw the particles
			vert takes in the textures the compute shader computed.
					sets the position and can also pass other things along (like if I want to do colour based on velocity)
			frag just sets the colour.
				
		PASS STD::VECTOR to opengl with &my_vector[0] or also &my_vector.front()
			Then for the size you use my_vector.size() * sizeof(<type of data in vector>)
*/