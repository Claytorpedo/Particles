#include "Camera.h"
#include "Units.h"

#include "glm\glm.hpp"
#include "glm\gtx\transform.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\rotate_vector.hpp"
#include "glm\gtc\matrix_inverse.hpp"

Camera::Camera(glm::vec3 origin, glm::vec3 position, glm::vec3 rotation, float FOV, float aspect, float near, float far,
			   units::Pixel screenWidth, units::Pixel screenHeight) 
			   : FOV_rads_(glm::radians(FOV)), aspect_(aspect), near_(near), far_(far), screen_width_(screenWidth), screen_height_(screenHeight),
				origin_(origin), position_(position), rotation_(rotation) {
		glm::mat4 rot = glm::rotate( glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f) );
		rot = glm::rotate( rot, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f) );
		rot = glm::rotate( rot, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f) );
		view_ = glm::translate(glm::translate( glm::mat4(1.0f), position_ ), origin_) * rot;
		projection_ = glm::perspective( FOV_rads_, aspect_, near_, far_ );
		updateProjectionView();
}

void Camera::updateProjectionView() {
	projection_view_ = projection_ * view_;
}

void Camera::setProjection( float FOV, float aspect, float near, float far ) {
	FOV_rads_ = glm::radians(FOV); aspect_ = aspect; near_ = near; far_ = far;
	projection_ = glm::perspective( FOV_rads_, aspect_, near_, far_);
	updateProjectionView();
}
void Camera::setLookAt( glm::vec3 position, glm::vec3 lookAt, glm::vec3 up) {
	view_ = glm::lookAt( position, lookAt, up );
	updateProjectionView();
}
void Camera::resize( units::Pixel screenWidth, units::Pixel screenHeight ) {
	screen_width_ = screenWidth; screen_height_ = screenHeight;
	aspect_ = (float)(screen_width_) / screen_height_;
	projection_ = glm::perspective( FOV_rads_, aspect_, near_, far_);
	updateProjectionView();
}
void Camera::rotate( float pitch, float yaw) {
	view_ = glm::translate( view_, -origin_ );
	view_ = glm::rotate( view_, pitch,	glm::vec3(1.0f, 0.0f, 0.0f) );
	view_ = glm::rotate( view_, yaw,	glm::vec3(0.0f, 1.0f, 0.0f) );
	view_ = glm::translate( view_, origin_ );
	updateProjectionView();
}
glm::mat4 Camera::getProjection() const {
	return projection_;
}
glm::mat4 Camera::getView() const {
	return view_;
}
glm::mat4 Camera::getProjectionView() const {
	return projection_view_;
}

Ray Camera::getRay(int x, int y) const {
		// Convert mouse coords to OpenGL coords.
		float gl_x = (2.0f * x) / screen_width_ - 1.0f;
		float gl_y = 1.0f - (2.0f * y) / screen_height_;

		glm::vec4 clip(gl_x, gl_y, -1.0f, 1.0f);

		// Convert to eye space.
		glm::mat4 projInv = glm::inverse( projection_ );
		glm::vec4 eyeCoords = projInv * clip;
		eyeCoords.z = -1.0f;
		eyeCoords.w = 0.0f;
		// Convert to world space.
		glm::mat4 viewInv = glm::inverse( view_ );
		glm::vec4 ray4 = viewInv * eyeCoords;
		glm::vec3 ray(ray4.x, ray4.y, ray4.z);
		glm::vec3 rayNorm = glm::normalize( ray );

		// Get camera origin.
		glm::vec4 origin = viewInv * glm::vec4(0,0,0,1);

		return Ray(glm::vec3(origin), rayNorm);
}

