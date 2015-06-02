#include "Camera.h"
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
Camera::Camera() {
	m_worldTransform = glm::mat4(1);
	UpdateProjectionViewTransform();
}

Camera::Camera(glm::vec3 _position) {
	m_worldTransform = glm::mat4(1);
	m_worldTransform[3] = glm::vec4(_position, 1);
	UpdateProjectionViewTransform();
}

Camera::Camera(glm::mat4 _transform) : m_worldTransform(_transform) {
	UpdateProjectionViewTransform();
}

void Camera::SetTransform(glm::mat4 _transform) {
	m_worldTransform = _transform;
	UpdateProjectionViewTransform();
}

void Camera::SetPosition(glm::vec3 _position) {
	m_worldTransform[3] = glm::vec4(_position, 1);
	UpdateProjectionViewTransform();
}

void Camera::SetLookAt(glm::vec3 _to, glm::vec3 _up) {
	glm::vec4 vLocation = m_worldTransform[3];
	m_worldTransform = glm::inverse(glm::lookAt(vLocation.xyz(), _to, _up));
	UpdateProjectionViewTransform();
}

void Camera::SetLookAt(glm::vec3 _from, glm::vec3 _to, glm::vec3 _up) {
	m_worldTransform = glm::inverse(glm::lookAt(_from, _to, _up));
	UpdateProjectionViewTransform();
}

void Camera::SetPerspective(float _fieldOfView, float _aspectRation, float _near, float _far) {
	m_projectionTransform = glm::perspective(_fieldOfView, _aspectRation, _near, _far);
	UpdateProjectionViewTransform();
}

void Camera::Translate(glm::vec3 _translation) {
	glm::vec4 pos = m_worldTransform[3];
	m_worldTransform[3] = pos + glm::vec4(_translation, 0);
	UpdateProjectionViewTransform();
}
void Camera::UpdateProjectionViewTransform() {
	m_viewTransform = glm::inverse(m_worldTransform);
	m_projectionViewTransform = m_projectionTransform * m_viewTransform;
}
void Camera::GetFrustumPlanes(glm::vec4* _planes) {
	glm::mat4 transform = m_projectionViewTransform;

	//right side
	_planes[0] = glm::vec4( transform[0][3] - transform[0][0],
							transform[1][3] - transform[1][0],
							transform[2][3] - transform[2][0],
							transform[3][3] - transform[3][0]);
	//left side
	_planes[1] = glm::vec4( transform[0][3] + transform[0][0],
							transform[1][3] + transform[1][0],
							transform[2][3] + transform[2][0],
							transform[3][3] + transform[3][0]);
	//top
	_planes[2] = glm::vec4( transform[0][3] - transform[0][1],
							transform[1][3] - transform[1][1],
							transform[2][3] - transform[2][1],
							transform[3][3] - transform[3][1]);
	//bottom
	_planes[3] = glm::vec4( transform[0][3] + transform[0][1],
							transform[1][3] + transform[1][1],
							transform[2][3] + transform[2][1],
							transform[3][3] + transform[3][1]);
	//far
	_planes[4] = glm::vec4( transform[0][3] - transform[0][2],
							transform[1][3] - transform[1][2],
							transform[2][3] - transform[2][2],
							transform[3][3] - transform[3][2]);
	//near
	_planes[5] = glm::vec4( transform[0][3] + transform[0][2],
							transform[1][3] + transform[1][2],
							transform[2][3] + transform[2][2],
							transform[3][3] + transform[3][2]);
	for (int i = 0; i < 6; i++) {
		_planes[i] = glm::normalize(_planes[i]);
	}
}

glm::vec3 Camera::PickAgainstPlane(float x, float y, const glm::vec4& _plane) {
	int width = 0;
	int height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

	glm::vec3 screenPos(x / width * 2 - 1, (y / height * 2 - 1) * -1, -1);

	screenPos.x /= m_projectionTransform[0][0];
	screenPos.y /= m_projectionTransform[1][1];

	glm::vec3 dir = glm::normalize(m_worldTransform * glm::vec4(screenPos, 0)).xyz();

	float d = (_plane.w - glm::dot(m_worldTransform[3].xyz(), _plane.xyz()) / glm::dot(dir, _plane.xyz()));

	if (d >= 0) {
		return m_worldTransform[3].xyz() + dir * d;
	} else {
		return glm::vec3(0);
	}

}

