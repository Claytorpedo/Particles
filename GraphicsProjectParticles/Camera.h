#ifndef _CAMERA_H
#define	_CAMERA_H

#include "glm\glm.hpp"
#include "Constants.h"
#include "Units.h"

struct Ray {
	const glm::vec3 position;
	const glm::vec3 direction;
	Ray( glm::vec3 position, glm::vec3 direction ) : position(position), direction(direction) {}
	~Ray() {}
};

class Camera {
private:
	glm::mat4 projection_, view_, projection_view_;
	units::Pixel screen_width_, screen_height_;
	float FOV_, aspect_, near_, far_;

	void updateProjectionView();
public:
	Camera(glm::vec3 position = glm::vec3(0,0,0), glm::vec3 lookAt = glm::vec3(0,0,-1), glm::vec3 up = glm::vec3(0,1,0), 
		float FOV = constants::FOV, float aspect = constants::ASPECT, float near = constants::NEAR, float far = constants::FAR,
		units::Pixel screenWidth = constants::SCREEN_WIDTH, units::Pixel screenHeight = constants::SCREEN_HEIGHT);
	~Camera() {}

	void setProjection( float FOV, float aspect, float near, float far );
	void setView( glm::vec3 position, glm::vec3 lookAt, glm::vec3 up);
	void resize( units::Pixel screenWidth, units::Pixel screenHeight );
	glm::mat4 getProjection() const;
	glm::mat4 getView() const;
	glm::mat4 getProjectionView() const;
	Ray getRay(int x, int y) const;
	
};

#endif // _CAMERA_H