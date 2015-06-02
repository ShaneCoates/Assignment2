#include "Skybox.h"
#include "Camera.h"
#include <gl_core_4_4.h>
#include <stb_image.h>
#include "ShaderLoader.h"
Skybox::Skybox() {
	m_program = ShaderLoader::LoadProgram("res/shaders/skybox.vs", "res/shaders/skybox.fs");
	CreateBuffers();
	std::vector<const char*> faces;
	faces.push_back("res/textures/skybox/starfield/right.jpg");
	faces.push_back("res/textures/skybox/starfield/left.jpg");
	faces.push_back("res/textures/skybox/starfield/top.jpg");
	faces.push_back("res/textures/skybox/starfield/left.jpg");
	faces.push_back("res/textures/skybox/starfield/front.jpg");
	faces.push_back("res/textures/skybox/starfield/back.jpg");
	LoadCubemap(faces);
}
Skybox::~Skybox() {

}
void Skybox::Draw(Camera* _camera) {
	glDepthMask(GL_FALSE);
	glUseProgram(m_program);
	int loc = glGetUniformLocation(m_program, "projectionView");
	glm::mat4 projectionView = _camera->GetProjection() * glm::mat4(glm::mat3(_camera->GetView()));
	glUniformMatrix4fv(loc, 1, false, &projectionView[0][0]);
	glBindVertexArray(m_vao);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(m_program, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}
void Skybox::LoadCubemap(std::vector<const char*> _faces) {
	glGenTextures(1, &m_texture);
	glActiveTexture(GL_TEXTURE0);
	int width, height;
	unsigned char* imageData;
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	for (GLuint i = 0; i < _faces.size(); i++) {
		imageData = stbi_load(_faces[i], &width, &height, 0, STBI_default);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		stbi_image_free(imageData);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
void Skybox::CreateBuffers() {
	GLfloat skyboxVertices[] = {
		// Positions          
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
}