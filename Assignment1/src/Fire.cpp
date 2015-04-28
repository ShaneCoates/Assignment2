#include <iostream>
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include "ShaderLoader.h"
#include "Camera.h"
#include "Fire.h"
#include "ParticleEmitter.h"

Fire::Fire() {
	Init();
	m_emitter = new ParticleEmitter();
	m_emitter->Init(50000, 1.0f, 2.0f, 1.0f, 5.0f, 0.1f, 0.15f, glm::vec4(1.0f, 1.0f, 1.0f, 0.75f), glm::vec4(1.0f, 1.0f, 1.0f, 0.25f));
	const char* path[2];
	path[0] = "res/textures/fire.png";
	path[1] = "res/textures/smoke.png";
	m_emitter->LoadTexture(path);
}
Fire::~Fire() {

}
void Fire::Init() {
	CreateFrameBuffers();
	CreateBufferObjects();
	m_program[0] = ShaderLoader::LoadProgram("res/shaders/blur.vs", "res/shaders/blurH.fs");
	m_program[1] = ShaderLoader::LoadProgram("res/shaders/blur.vs", "res/shaders/blurV.fs");
}
void Fire::CreateFrameBuffers() {
	for (int i = 0; i < 2; i++) 	{
		glGenFramebuffers(1, &m_fbo[i]);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[i]);

		glGenTextures(1, &m_fboTexture[i]);
		glBindTexture(GL_TEXTURE_2D, m_fboTexture[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1240, 768);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fboTexture[i], 0);
	}
	glGenRenderbuffers(1, &m_fboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1240, 768);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer Error!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Fire::CreateBufferObjects() {
	float vertexData[] = {
		-1, -1, 0, 1, 0, 0,
		1, -1, 0, 1, 1, 0,
		1, 1, 0, 1, 1, 1,
		-1, 1, 0, 1, 0, 1
	};
	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3
	};
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4, vertexData, GL_STATIC_DRAW);
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, indexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void Fire::Draw(Camera* _camera) {
	m_camera = _camera;
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[0]);
	glViewport(0, 0, 1240, 768);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_emitter->Draw((float)glfwGetTime(), m_camera);
	for (unsigned int i = 0; i < 2; i++)	{
		DrawToTexture(i);
	}
}
void Fire::DrawToTexture(unsigned int _index) {
	if (_index == 1) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo[_index + 1]);
	}
	glViewport(0, 0, 1240, 768);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_program[_index]);
	int loc = glGetUniformLocation(m_program[_index], "ProjectionView");
	if (loc >= 0) {
		glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fboTexture[_index]);
	glUniform1i(glGetUniformLocation(m_program[_index], "diffuse"), 0);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}