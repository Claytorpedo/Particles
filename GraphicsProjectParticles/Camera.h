#ifndef _CAMERA_H
#define	_CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "Constants.h"
#include "Units.h"

struct Ray {
	const glm::vec3 position;
	const glm::vec3 direction;
	Ray( glm::vec3 position, glm::vec3 direction ) : position(position), direction(direction) {}
	~Ray() {}
};

class Camera {
public:
	/*
	origin       - The origin of the camera. Rotations are done relative to the origin.
	position     - The position of the camera relative to its origin. A position of (0,0,-5) will rotate on an axis 5 units back from origin.
	rotation     - The rotation of the camera (relative to forward facing).
	FOV          - Field of view (in degrees) of the projection.
	Aspect       - Aspect ratio of the projection. Generally should be screenWidth/screenHeight
	near         - near clipping plane.
	far          - far clipping plane.
	screenWidth  - width of the window in pixels
	screenHeight - height of the window in pixels
	*/
	Camera(glm::vec3 origin = glm::vec3(0,0,0), glm::vec3 position = glm::vec3(0,0,-5), glm::vec3 rotation = glm::vec3(0,0,-1),
           float FOV = constants::FOV, float aspect = constants::DEFAULT_ASPECT, float near = constants::NEAR, float far = constants::FAR,
           units::Pixel screenWidth = constants::DEFAULT_SCREEN_WIDTH, units::Pixel screenHeight = constants::DEFAULT_SCREEN_HEIGHT);
	~Camera() {}

	void setProjection(float FOV, float aspect, float near, float far );
	void setLookAt(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up);
	void resize(units::Pixel screenWidth, units::Pixel screenHeight );
	void rotate(float horizontalPixels, float verticalPixels);
	void pan(float horizontalPixels, float verticalPixels);
	void setZoom(float zoomAmount);
	float getZoom();
	void reset();
	glm::mat4 getProjection() const;
	glm::mat4 getView() const;
	glm::mat4 getProjectionView() const;
	Ray getRay(int x, int y) const;
	
private:
	glm::mat4 projection_, view_, projection_view_;
	units::Pixel screen_width_, screen_height_;
	float FOV_rads_, aspect_, near_, far_;

	glm::vec3 origin_, initial_position_, position_;
	glm::fquat initial_rotation_, rotation_;
	float degrees_wide_, degrees_tall_; // Pixels per degree of rotation.

	void updateView();
	void updateProjectionView();
};

#endif // _CAMERA_H