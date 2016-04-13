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
#include "glm\gtx\transform.hpp"
#include "glm\gtc\matrix_transform.hpp"






#include "Framebuffer.h"
#include "VertexBuffer.h"



using namespace constants;

ShaderProgram* initShader = NULL;
ShaderProgram* drawShader = NULL;
ShaderProgram* updateShader = NULL;
AmbientParticleSystem* particleSystem = NULL;

Framebuffer *framebuffer1;
Framebuffer *framebuffer2;
VertexBuffer *quadBuffer;
VertexBuffer *uvBuffer;

bool debug = false;

void close () {
	delete initShader;
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

	initShader = new ShaderProgram(AMB_PART_INIT_VERT_PATH, AMB_PART_INIT_FRAG_PATH);
	initShader->load();
	drawShader = new ShaderProgram(AMB_PART_DRAW_VERT_PATH, AMB_PART_DRAW_FRAG_PATH);
	drawShader->load();
	updateShader = new ShaderProgram(AMB_PART_UPDATE_VERT_PATH, AMB_PART_UPDATE_FRAG_PATH);
	updateShader->load();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////// CREATE FRAMEBUFFER OBJECTS /////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	unsigned int width = units::getPowerOf2(constants::DEFAULT_PARTICLE_EXPONENT);
	unsigned int height = units::getPowerOf2(constants::DEFAULT_PARTICLE_EXPONENT);

	framebuffer1 = new Framebuffer(width, height, 3);
	framebuffer2 = new Framebuffer(width, height, 3);
	if ( !framebuffer1->init() || !framebuffer2->init() ) {
		std::cerr << "Error: Framebuffers could not be initialized." << std::endl;
		return 0;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////// CREATE VERTEX BUFFERS //////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create quad for textures to draw onto.
	static const GLfloat quad_array[] = {
		-1.0f,	-1.0f,  0.0f,	
		 1.0f,	 1.0f,  0.0f,
		-1.0f,	 1.0f,  0.0f,
		-1.0f,	-1.0f,  0.0f,
		 1.0f,	-1.0f,  0.0f,
		 1.0f,	 1.0f,  0.0f,
	};
	quadBuffer = new VertexBuffer( 3, 6, &quad_array[0] );

	unsigned int count = width * height;
	std::vector<GLfloat> uv_data;
	uv_data.reserve(count * 2);
	// We do a lot of these, so compute the divisions once for faster multiplication operations in the loop.
	const float heightRecip = 1.0f/height;
	const float widthRecip = 1.0f/width;
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			uv_data.push_back((GLfloat)(x) * widthRecip);
			uv_data.push_back((GLfloat)(y) * heightRecip);
		}
	}
	uvBuffer = new VertexBuffer( 2, count, uv_data );
	uv_data.clear();
	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////// GET UNIFORM AND ATTRIBUTE IDS //////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	glEnable( GL_PROGRAM_POINT_SIZE );

	GLint inVertexPos = glGetAttribLocation( initShader->getProgramID(), "inVertexPos" );
	GLint res = glGetUniformLocation( initShader->getProgramID(), "uResolution" );

	GLint inUV = glGetAttribLocation( drawShader->getProgramID(), "inUV" );
	GLint uPointSize = glGetUniformLocation( drawShader->getProgramID(), "uPointSize" );
	GLint uPVM = glGetUniformLocation( drawShader->getProgramID(), "uPVM" );
	GLint uColour = glGetUniformLocation( drawShader->getProgramID(), "uColour" );
	GLint uTexPos = glGetUniformLocation( drawShader->getProgramID(), "uTexPos" );

	GLint inUpdateVertexPos = glGetAttribLocation( updateShader->getProgramID(), "inVertexPos" );
	GLint uUpdateResolution = glGetUniformLocation( updateShader->getProgramID(), "uResolution" );
	GLint uUpdateElapsedTime = glGetUniformLocation( updateShader->getProgramID(), "uElapsedTime" );
	GLint uUpdateInputPos = glGetUniformLocation( updateShader->getProgramID(), "uInputPos" );
	GLint uUpdateKForce = glGetUniformLocation( updateShader->getProgramID(), "uKForce" );
	GLint uUpdateTexPos = glGetUniformLocation( updateShader->getProgramID(), "uTexPos" );
	GLint uUpdateTexVel = glGetUniformLocation( updateShader->getProgramID(), "uTexVel" );
	GLint uUpdateTexOther = glGetUniformLocation( updateShader->getProgramID(), "uTexOther" );

	// Get view projection.
	glm::mat4 projection = glm::perspective(glm::radians(FOV), ASPECT, NEAR, FAR);
	glm::mat4 view = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0,-1,0), glm::vec3(0,1,0)); // Move back 3, look at origin, and y is up.
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.5f, 0.0f));
	glm::mat4 PVM = projection * view * model;

	glm::vec3 gravPos( -2, 0, 0 );

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////// INITIALIZE PARTICLE POSITIONS //////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	framebuffer1->drawTo();
	// Store the previous viewport.
	GLint prevViewport[4];
	glGetIntegerv( GL_VIEWPORT, prevViewport );
	glViewport( 0, 0, width, height );
	glClear( GL_COLOR_BUFFER_BIT );
	glDisable( GL_DEPTH_TEST );
	glBlendFunc( GL_ONE, GL_ZERO);
	graphics.clear();
	initShader->use();
	glUniform2f( res, width, height );

	quadBuffer->bind();
	glEnableVertexAttribArray( inVertexPos );
	glVertexAttribPointer( inVertexPos, 3,
		GL_FLOAT,	// type
		GL_FALSE,	// normalized?
		0,			// stride
		(void*)0 );	// Array buffer offset
	glDrawArrays( GL_TRIANGLES, 0, 6 );
	
	quadBuffer->unbind();
	glDisableVertexAttribArray( inVertexPos );
	initShader->stopUsing();
	framebuffer1->stopDrawingTo();
	// Return the viewport to its previous state.
	glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3] );

	previousTime = SDL_GetTicks();
	while (!quit) {
		if ( getInput(&input) || input.wasKeyPressed(SDLK_ESCAPE) ) {
			break;
		}
		processInput(&input, &graphics);

		graphics.clear();

		currentTime = SDL_GetTicks();
		elapsedTime = currentTime - previousTime;
		previousTime = currentTime;
		float elapsedTimeSecs = units::millisToSeconds(elapsedTime);

		if (debug) {
			framebuffer1->readFrom();
			glReadBuffer( GL_COLOR_ATTACHMENT0 );
			GLfloat *pixels = new GLfloat[ 4];
			glReadPixels( 0, 1023, 1, 1 , GL_RGBA, GL_FLOAT, pixels);
			std::cout << "Position:     " << pixels[0] << "  " << pixels[1] << "  " << pixels[2] << "  " << pixels[3] << std::endl;
			glReadBuffer( GL_COLOR_ATTACHMENT1 );
			glReadPixels( 0, 1023, 1, 1, GL_RGBA, GL_FLOAT, pixels);
			std::cout << "Velocity:     " << pixels[0] << "  " << pixels[1] << "  " << pixels[2] << "  " << pixels[3] << std::endl;
			delete pixels;
			framebuffer1->stopReading();
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////// UPDATE PARTICLE POSITIONS //////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		updateShader->use();
		// Bind the framebuffer that wasn't updated last time (or initialized from).
		framebuffer2->drawTo();
		// Bind our uniform/variable handles to the update shader (resolution is already bound).
		glUniform2f( uUpdateResolution, width, height );
		glUniform1f( uUpdateElapsedTime, elapsedTimeSecs );//* 0.5f);//elapsedTimeSecs > 0.2f ? 0.2f : elapsedTimeSecs );
		glUniform1f( uUpdateKForce, 5.0f);
		glUniform3f( uUpdateInputPos, 2.0f, 2.0f, 0.0f );

		// Store the previous viewport.
		glGetIntegerv( GL_VIEWPORT, prevViewport );
		glViewport( 0, 0, width, height );
		glClear( GL_COLOR_BUFFER_BIT );
		glDisable( GL_DEPTH_TEST ); // Draw everything flat.
		glBlendFunc( GL_ONE, GL_ZERO );

		// Bind our textures.
		std::vector<GLint> uniformTextureUpdateLocations(2);
		uniformTextureUpdateLocations[0] = uUpdateTexPos;
		uniformTextureUpdateLocations[1] = uUpdateTexVel;
		framebuffer1->bindTextures( uniformTextureUpdateLocations );

		quadBuffer->bind();
		glEnableVertexAttribArray( inUpdateVertexPos );
		glVertexAttribPointer( inUpdateVertexPos, 3,
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,			// stride
			(void*)0 );	// Array buffer offset

		glDrawArrays( GL_TRIANGLES, 0, 6);

		quadBuffer->unbind();
		glDisableVertexAttribArray( inUpdateVertexPos );
		// Return the viewport to its previous state.
		glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3] );

		framebuffer1->unbindTextures();
		framebuffer2->stopDrawingTo();
		// Swap buffers.
		Framebuffer *temp = framebuffer1;
		framebuffer1 = framebuffer2;
		framebuffer2 = temp;
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////// DRAW PARTICLES /////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		graphics.clear();

		glEnable( GL_DEPTH_TEST );

		drawShader->use();
		// Enable additive blending, so overlapping particles appear brighter.
		glBlendFunc( GL_SRC_ALPHA , GL_ONE );

		glUniformMatrix4fv(uPVM, 1, GL_FALSE, &PVM[0][0] );
		glUniform4f(uColour, 0.1f, 0.6f, 0.6f, 0.6f);
		glUniform1f(uPointSize, 1.0f);
		
		std::vector<GLint> uniformTextureDrawLocations(1);
		uniformTextureDrawLocations[0] = uTexPos;
		framebuffer1->bindTextures( uniformTextureDrawLocations );

		uvBuffer->bind();
		glEnableVertexAttribArray( inUV );
		glVertexAttribPointer( inUV, 2,
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,			// stride
			(void*)0 );	// Array buffer offset

		glDrawArrays( GL_POINTS, 0, width*height );

		framebuffer1->unbindTextures();
		uvBuffer->unbind();
		glDisableVertexAttribArray( inUV );
		drawShader->stopUsing();
		
		graphics.present();
		
		// Limit loop to 60hz.
		if ( elapsedTime < SIXTY_FPS_FRAME_DUR ) {
			SDL_Delay( SIXTY_FPS_FRAME_DUR - elapsedTime );
		}
	}
	close();
	return 0;
}
	/*
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
			
	
			graphics.present();
		}
	}
	close();
	return 0;
}
*/
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

*/