#define _USE_MATH_DEFINES
#include <cmath> 
#include <gl_core_4_4.h>
#include <GLFW\glfw3.h>
#include <glm.hpp>
#include <ext.hpp>
#include <stb-master\stb_image.h>
#include "Terrain.h"
#include "ShaderLoader.h"
#include "Camera.h"
Terrain::Terrain() {
	m_zValue = 0;
	m_octaves = 10;
	m_amplitude = 1.0f;
	m_persistence = 0.3f;
	m_size = 1;
	m_lightHeight = 10;
	ReloadShaders();
	const char* path[3];
	path[0] = "res/textures/terrain/water.jpg";
	path[1] = "res/textures/terrain/rock.png";
	path[2] = "res/textures/terrain/grass.jpg";
	LoadTextures(path);
	GenerateGrid(50);
}
Terrain::~Terrain(){

}
void Terrain::Update(double _dt) {

}
void Terrain::Draw(Camera* _camera) {
	glUseProgram(m_program);
	glEnable(GL_DEPTH_TEST);
	
	unsigned int loc = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(_camera->GetProjectionView()));
	loc = glGetUniformLocation(m_program, "CameraPos");
	glUniform3fv(loc, 1, glm::value_ptr(_camera->GetPosition()));
	loc = glGetUniformLocation(m_program, "LightDir");
	glUniform3fv(loc, 1, glm::value_ptr(glm::normalize(glm::vec3(cosf(glfwGetTime()), m_lightHeight, sinf(glfwGetTime())))));

	loc = glGetUniformLocation(m_program, "perlinTexture");
	glUniform1i(loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, m_perlinTexture);
	for (unsigned int i = 0; i < 3; i++)
	{
		glActiveTexture(GL_TEXTURE1 + i);
		glBindTexture(GL_TEXTURE_2D, m_terrainTextures[i]);
		char formatTex[32];
		sprintf(formatTex, "terrainTexture[%i]", i);
		loc = glGetUniformLocation(m_program, formatTex);
		glUniform1i(loc, i + 1);
	}
	loc = glGetUniformLocation(m_program, "time");
	glUniform1f(loc, (float)glfwGetTime());

	loc = glGetUniformLocation(m_program, "zValue");
	glUniform1f(loc, (float)(m_zValue * 0.01f));

	loc = glGetUniformLocation(m_program, "amplitude");
	glUniform1f(loc, (float)(m_amplitude));

	loc = glGetUniformLocation(m_program, "size");
	glUniform1f(loc, (m_size));

	glBindVertexArray(m_VAO);

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
}
void Terrain::ReloadShaders() {
	m_program = ShaderLoader::LoadProgram("res/shaders/procedural.vs", "res/shaders/procedural.fs");
}
void Terrain::GenerateGrid(unsigned int _size) {
	//Create Vertices
	TerrainVertex* vertices = new TerrainVertex[_size * _size];
	//for (unsigned int r = 0; r < _size; ++r) {
	//	for (unsigned int c = 0; c < _size; ++c) {
	//		vertices[r * _size + c].Position = glm::vec4((float)c - (_size * 0.5f), 0, (float)r - (_size * 0.5f), 1);
	//		vertices[r * _size + c].UV = glm::vec2((float)c / _size, (float)r / _size);
	//	}
	//}
	float const R = 1.0f / (float)(_size - 1);
	float const S = 1.0f / (float)(_size - 1);
	for (unsigned int r = 0; r < _size; r++) {
		for (unsigned int s = 0; s < _size; s++) {
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			vertices[r * _size + s].Position = glm::vec4(x * _size, y * _size, z * _size, 1);
			vertices[r * _size + s].UV = glm::vec2(s*S, r*R);
			vertices[r * _size + s].Normal = glm::vec3(x, y, z);

		}
	}



	//Generate Buffers
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	glGenVertexArrays(1, &m_VAO);
	//Bind Buffers
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	//Bind Vertices
	glBufferData(GL_ARRAY_BUFFER, (_size * _size) * sizeof(TerrainVertex), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)(sizeof(glm::vec4)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)(sizeof(glm::vec4) + (sizeof(glm::vec2))));
	//Create Indices
	m_indexCount = _size * _size * 6;

	unsigned int* indices = new unsigned int[m_indexCount];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (_size - 1); ++r) {
		for (unsigned int c = 0; c < (_size - 1); ++c) {
			indices[index++] = r * _size + c;
			indices[index++] = (r + 1) * _size + c;
			indices[index++] = (r + 1) * _size + (c + 1);
	
			indices[index++] = r * _size + c;
			indices[index++] = (r + 1) * _size + (c + 1);
			indices[index++] = r * _size + (c + 1);
		}
	}





	//Bind Indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	//Unbind Buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	delete[] vertices;
	delete[] indices;
	//CreateTexture(_size);
}
void Terrain::CreateTexture(unsigned int _size) {
	float octaves = m_octaves;
	float *perlin_data = new float[_size * _size * _size];
	float scale = (1.0f / _size) * 3;
	for (unsigned int r = 0; r < _size; ++r) {
		for (unsigned int c = 0; c < _size; ++c) {
			for (unsigned int d = 0; d < _size; ++d) {
				float amplidute = 1;
				float persistence = m_persistence;
				perlin_data[r + _size*c + _size*_size*d] = glm::perlin(glm::vec3(r, c, d) * scale) * 0.5f + 0.5f;

				for (int o = 0; o < octaves; ++o) {
					float freq = powf(2, (float)o);
					float perlin_sample = glm::perlin(glm::vec4((float)r, (float)c, (float)d, m_seed) * scale * freq) * 0.5f + 0.5f;
					perlin_data[r + _size*c + _size*_size*d] += perlin_sample * amplidute;
					amplidute *= persistence;
				}
			}
		}
	}

	glGenTextures(1, &m_perlinTexture);
	glBindTexture(GL_TEXTURE_3D, m_perlinTexture);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, _size, _size, _size, 0, GL_RED, GL_FLOAT, perlin_data);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
}
void Terrain::LoadTextures(const char* _path[3]) {
	for (int i = 0; i < 3; i++) {
		int textureWidth, textureHeight, textureFormat;
		unsigned char* imageData = stbi_load(_path[i], &textureWidth, &textureHeight, &textureFormat, STBI_default);
		glGenTextures(1, &m_terrainTextures[i]);
		glBindTexture(GL_TEXTURE_2D, m_terrainTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		stbi_image_free(imageData);
	}
}
void Terrain::NewSeed(unsigned int _seed) {
	m_seed = _seed;
	CreateTexture(20);
}
void Terrain::NewSeed() {
	m_seed = (rand() % 100000);
	CreateTexture(20);
}