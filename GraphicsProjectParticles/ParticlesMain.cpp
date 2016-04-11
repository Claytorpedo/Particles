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

ShaderProgram* initShader = NULL;
ShaderProgram* drawShader = NULL;
ShaderProgram* updateShader = NULL;
AmbientParticleSystem* particleSystem = NULL;

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
void printFramebufferInfo(GLenum target, GLuint fbo) {

	glBindFramebuffer(target,fbo);

	int res;

	GLint buffer;
	int i = 0;
	do {
		glGetIntegerv(GL_DRAW_BUFFER0+i, &buffer);
		if (buffer != GL_NONE) {
			printf("Shader Output Location %d - color attachment %d\n", i, buffer - GL_COLOR_ATTACHMENT0);

			glGetFramebufferAttachmentParameteriv(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
			printf("\tAttachment Type: %s\n", res==GL_TEXTURE?"Texture":"Render Buffer");
			glGetFramebufferAttachmentParameteriv(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
			printf("\tAttachment object name: %d\n",res);
		}
		++i;
	} while (buffer != GL_NONE);
}

void genTex( GLuint &tex, unsigned int w, unsigned int h) {
	glGenTextures( 1, &tex );
	glBindTexture(GL_TEXTURE_2D, tex);
	// Give empty image to OpenGL.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, 0);
	// Use GL_NEAREST, since we don't want any kind of averaging across values:
	// we just want one pixel to represent a particle's data.
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// This probably isn't necessary, but we don't want to have UV coords past the image edges.
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
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

	GLuint texPos;
	genTex( texPos, width, height);
	GLuint texVel;
	genTex( texVel, width, height);
	
	GLuint fbo;
	glGenFramebuffers( 1, &fbo );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo );

	// Bind our textures to the framebuffer.
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texPos, 0);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texVel, 0);
	glDrawBuffers( 2, drawBuffers );

	if ( glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
		std::cerr << "Error: Failed to create framebuffer." << std::endl;
		return false;
	}
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0);

	
	GLuint texPos2;
	genTex(texPos2, width, height);
	GLuint texVel2;
	genTex(texVel2, width, height);

	GLuint fbo2;
	glGenFramebuffers( 1, &fbo2 );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo2 );

	// Bind our textures to the framebuffer.
	GLenum drawBuffers2[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texPos2, 0);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, texVel2, 0);
	glDrawBuffers( 2, drawBuffers2 );

	if ( glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ) {
		std::cerr << "Error: Failed to create framebuffer." << std::endl;
		return false;
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
	// Get the vertex and buffer array ids.
	GLuint quadArray;
	glGenVertexArrays( 1, &quadArray );
	glBindVertexArray( quadArray );
	GLuint quadBuf;
	glGenBuffers(1, &quadBuf);
	glBindBuffer(GL_ARRAY_BUFFER, quadBuf);
	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), quad_array, GL_STATIC_DRAW);

	unsigned int count = width * height * 2;
	std::vector<GLfloat> uv_data;
	uv_data.reserve(count);
	// We do a lot of these, so compute the divisions once for faster multiplication operations in the loop.
	const float heightRecip = 1.0f/height;
	const float widthRecip = 1.0f/width;
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			uv_data.push_back((GLfloat)(x) * widthRecip);
			uv_data.push_back((GLfloat)(y) * heightRecip);
		}
	}
	GLuint uvArray;
	glGenVertexArrays( 1, &uvArray );
	glBindVertexArray( uvArray );
	GLuint uvBuff;
	glGenBuffers(1, &uvBuff);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuff);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(GLfloat), uv_data.data(), GL_STATIC_DRAW);



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
	GLint uUpdateDeltaT = glGetUniformLocation( updateShader->getProgramID(), "uDeltaT" );
	GLint uUpdateInputPos = glGetUniformLocation( updateShader->getProgramID(), "uInputPos" );
	GLint uUpdateKForce = glGetUniformLocation( updateShader->getProgramID(), "uKForce" );
	GLint uUpdateTexPos = glGetUniformLocation( updateShader->getProgramID(), "uTexPos" );
	GLint uUpdateTexVel = glGetUniformLocation( updateShader->getProgramID(), "uTexVel" );
	GLint uUpdateTexOther = glGetUniformLocation( updateShader->getProgramID(), "uTexOther" );

	// Get view projection.
	glm::mat4 projection = glm::perspective(glm::radians(FOV), ASPECT, NEAR, FAR);
	glm::mat4 view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Move back 3, look at origin, and y is up.
	glm::mat4 model = glm::mat4(1.0f); // Just put the triangle at origin.
	glm::mat4 PVM = projection * view * model;

	glm::vec3 gravPos( -2, 0, 0 );



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////// INITIALIZE PARTICLE POSITIONS //////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo);
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
	glEnableVertexAttribArray( inVertexPos );
	glBindBuffer( GL_ARRAY_BUFFER, quadBuf );
	glVertexAttribPointer( inVertexPos, 3,
		GL_FLOAT,	// type
		GL_FALSE,	// normalized?
		0,			// stride
		(void*)0 );	// Array buffer offset
	glDrawArrays( GL_TRIANGLES, 0, 6 );
	
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDisableVertexAttribArray( inVertexPos );
	initShader->stopUsing();
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	// Return the viewport to its previous state.
	glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3] );

	previousTime = SDL_GetTicks();
	bool first = true;
	while (!quit) {
		if ( getInput(&input) ) {
			break;
		}
		processInput(&input, &graphics);

		graphics.clear();
		if (first) {
			first = false;
			glBindFramebuffer( GL_FRAMEBUFFER, fbo);
			GLfloat *pixels = new GLfloat[width * height * 4];
			glReadPixels( 0, 0, width, height , GL_RGBA, GL_FLOAT, pixels);

			std::cout << "some pixel entries:\n";
			std::cout << "pixel0:     " << pixels[0] << "  " << pixels[1] << "  " << pixels[2] << "  " << pixels[3] << std::endl;
			std::cout << "pixel10:    " << pixels[40] << "  " << pixels[41] << "  " << pixels[42] << "  " << pixels[43] << std::endl;
			std::cout << "pixel100:   " << pixels[400] << "  " << pixels[401] << "  " << pixels[402] << "  " << pixels[403] << std::endl;
			//std::cout << "pixel10000: " << pixels[40000] << "  " << pixels[40001] << "  " << pixels[40002] << "  " << pixels[40003] << std::endl;
			//std::cout << "pixel100000:" << pixels[400000] << "  " << pixels[400001] << "  " << pixels[400002] << "  " << pixels[400003] << std::endl;

			GLfloat *pixels2 = new GLfloat[width * height * 4];
			glReadBuffer( GL_COLOR_ATTACHMENT1 );
			glReadPixels( 0, 0, width, height , GL_RGBA, GL_FLOAT, pixels2);

			std::cout << "some pixel entries:\n";
			std::cout << "pixel0:     " << pixels2[0] << "  " << pixels2[1] << "  " << pixels2[2] << "  " << pixels2[3] << std::endl;
			std::cout << "pixel10:    " << pixels2[40] << "  " << pixels2[41] << "  " << pixels2[42] << "  " << pixels2[43] << std::endl;
			std::cout << "pixel100:   " << pixels2[400] << "  " << pixels2[401] << "  " << pixels2[402] << "  " << pixels2[403] << std::endl;
			//std::cout << "pixel10000: " << pixels2[40000] << "  " << pixels2[40001] << "  " << pixels2[40002] << "  " << pixels2[40003] << std::endl;
			//std::cout << "pixel100000:" << pixels2[400000] << "  " << pixels2[400001] << "  " << pixels2[400002] << "  " << pixels2[400003] << std::endl;

			delete pixels2;
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////// UPDATE PARTICLE POSITIONS //////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		currentTime = SDL_GetTicks();
		elapsedTime = currentTime - previousTime;
		float elapsedTimeSecs = units::millisToSeconds(elapsedTime);
		
		updateShader->use();
		// Bind the framebuffer that wasn't updated last time (or initialized from).
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo2 );
		// Bind our uniform/variable handles to the update shader (resolution is already bound).
		glUniform2f( uUpdateResolution, width, height );
		glUniform1f( uUpdateDeltaT, elapsedTimeSecs > 0.2f ? 0.2f : elapsedTimeSecs );
		glUniform1f( uUpdateKForce, 10.0f);
		glUniform3f( uUpdateInputPos, 0.5f, 10.5f, 0.0f );

		// Store the previous viewport.
		glGetIntegerv( GL_VIEWPORT, prevViewport );
		glViewport( 0, 0, width, height );
		glClear( GL_COLOR_BUFFER_BIT );
		glDisable( GL_DEPTH_TEST ); // Draw everything flat.
		glBlendFunc( GL_ONE, GL_ZERO );

		// Bind our textures.
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texPos );
		glUniform1i( uUpdateTexPos, 0 );
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, texVel );
		glUniform1i( uUpdateTexVel, 1 );

		glEnableVertexAttribArray( inUpdateVertexPos );
		glBindBuffer( GL_ARRAY_BUFFER, quadBuf );
		glVertexAttribPointer( inUpdateVertexPos, 3,
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,			// stride
			(void*)0 );	// Array buffer offset

		glDrawArrays( GL_TRIANGLES, 0, 6);

		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glDisableVertexAttribArray( inUpdateVertexPos );
		// Return the viewport to its previous state.
		glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3] );
		// Swap buffers.
		GLint temp = fbo;
		fbo = fbo2;
		fbo2 = temp;
		temp = texPos;
		GLint temp2 = texVel;
		texPos = texPos2;
		texVel = texPos2;
		texPos2 = temp;
		texVel2 = temp2;

		glBindTexture( GL_TEXTURE_2D, 0);
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////// DRAW PARTICLES /////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		
		graphics.clear();

		//glEnable( GL_DEPTH_TEST );

		drawShader->use();
		// Enable additive blending, so overlapping particles appear brighter.
		glBlendFunc( GL_ONE , GL_ONE );

		glUniformMatrix4fv(uPVM, 1, GL_FALSE, &PVM[0][0] );
		glUniform4f(uColour, 0.4f, 0.1f, 0.4f, 0.6f);
		glUniform1f(uPointSize, 4.0f);
		
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, texPos );
		glUniform1i( uTexPos, 0 );

		glEnableVertexAttribArray( inUV );
		glBindBuffer( GL_ARRAY_BUFFER, uvBuff );
		glVertexAttribPointer( inUV, 2,
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,			// stride
			(void*)0 );	// Array buffer offset

		glDrawArrays( GL_POINTS, 0, width*height );

		glBindTexture( GL_TEXTURE_2D, 0 );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glDisableVertexAttribArray( inUV );
		drawShader->stopUsing();
		
		graphics.present();
		previousTime = currentTime;
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