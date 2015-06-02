#include "FlyCamera.h"
#include <GLFW\glfw3.h>

void FlyCamera::Init() {
	//m_flySpeed = 10.0f;
	m_bViewButtonClicked = false;
}
void FlyCamera::Update(double dt) {
	HandleKeyboardInput(dt);
	HandleMouseInput(dt);
}
void FlyCamera::HandleKeyboardInput(double dt) {
	glm::vec3 vRight = m_worldTransform[0].xyz;
	glm::vec3 vUp = m_worldTransform[1].xyz;
	glm::vec3 vForward = m_worldTransform[2].xyz;
	glm::vec3 moveDir(0.0f);
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS ||
		(glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS
		&& glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)) {
		moveDir -= vForward;
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
		moveDir += vForward;
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)	{
		moveDir -= vRight;
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)	{
		moveDir += vRight;
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
	}
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		moveDir -= glm::vec3(0.0f, 1.0f, 0.0f);
	}
	float fLength = glm::length(moveDir);
	if (fLength > 0.01f) {
		moveDir = glm::normalize(moveDir) * (float)dt * m_flySpeed;
		SetPosition(GetPosition() + moveDir);
	}
}
void FlyCamera::HandleMouseInput(double dt) {
	if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		if (m_bViewButtonClicked == false) {
			int width, height;
			glfwGetFramebufferSize(m_window, &width, &height);
			m_dCursorX = width / 2.0;
			m_dCursorY = height / 2.0;
			glfwSetCursorPos(m_window, m_dCursorX, m_dCursorY);
			m_bViewButtonClicked = true;
		} else {
			double mouseX, mouseY;
			glfwGetCursorPos(m_window, &mouseX, &mouseY);
			double xOffset = mouseX - m_dCursorX;
			double yOffset = mouseY - m_dCursorY;
			m_dCursorX = mouseX;
			m_dCursorY = mouseY;
			CalculateRotation(dt, xOffset, yOffset);
		}
	} else {
		m_bViewButtonClicked = false;
	}
}
void FlyCamera::CalculateRotation(double dt, double xOffset, double yOffset) {
	if (xOffset != 0) {
		glm::mat4 rot = glm::rotate((float)(dt * -xOffset), glm::vec3(0, 1, 0));
		SetTransform(GetTransform() * rot);
	}
	if (yOffset != 0) {
		glm::mat4 rot = glm::rotate((float)(dt * -yOffset), glm::vec3(1, 0, 0));
		SetTransform(GetTransform() * rot);
	}
	glm::mat4 oldTrans = GetTransform();
	glm::vec3 worldUp = glm::vec3(0, 1, 0);
	glm::mat4 trans;
	trans[0] = glm::normalize(glm::vec4(glm::cross(worldUp, oldTrans[2].xyz()), 0));
	trans[1] = glm::normalize(glm::vec4(glm::cross(oldTrans[2].xyz(), trans[0].xyz()), 0));
	trans[2] = glm::normalize(oldTrans[2]);
	trans[3] = oldTrans[3];
	SetTransform(trans);
}