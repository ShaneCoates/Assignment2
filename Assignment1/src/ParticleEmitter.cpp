#include "ParticleEmitter.h"
#include <gl_core_4_4.h>
#include <ext.hpp>
#include "ShaderLoader.h"
#include "Camera.h"
#include <stb_image.h>
ParticleEmitter::ParticleEmitter() : m_particles(nullptr), m_maxParticles(0), m_position(0, 0, 0), m_drawShader(0), m_updateShader(0), m_lastDrawTime(0) {
	for (int i = 0; i <= 1; ++i) {
		m_vao[i] = 0;
		m_vbo[i] = 0;
	}
}
ParticleEmitter::~ParticleEmitter() {
	delete[] m_particles;
	glDeleteVertexArrays(2, m_vao);
	glDeleteBuffers(2, m_vbo);
	glDeleteProgram(m_drawShader);
	glDeleteProgram(m_updateShader);
}
void ParticleEmitter::Init(unsigned int _maxParticles, float _minLifeSpan, float _maxLifeSpan, float _minVelocity, float _maxVelocity, float _startSize, float _endSize, const glm::vec4& _startColour, const glm::vec4& _endColour) {
	m_maxParticles = _maxParticles;
	m_minLifeSpan = _minLifeSpan;
	m_maxLifeSpan = _maxLifeSpan;
	m_minVelocity = _minVelocity;
	m_maxVelocity = _maxVelocity;
	m_startSize = _startSize;
	m_endSize = _endSize;
	m_startColour = _startColour;
	m_endColour = _endColour;
	m_particles = new Particle[_maxParticles];
	m_activeBuffer = 0;
	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();
}
void ParticleEmitter::Draw(float _time, Camera* _camera) {
	glUseProgram(m_updateShader);
	int loc = glGetUniformLocation(m_updateShader, "time");
	glUniform1f(loc, _time);
	float deltaTime = _time - m_lastDrawTime;
	m_lastDrawTime = _time;
	loc = glGetUniformLocation(m_updateShader, "deltaTime");
	glUniform1f(loc, deltaTime);
	loc = glGetUniformLocation(m_updateShader, "emitterPosition");
	glUniform3fv(loc, 1, glm::value_ptr(_camera->GetPosition()));
	glEnable(GL_RASTERIZER_DISCARD);
	glBindVertexArray(m_vao[m_activeBuffer]);
	unsigned int otherBuffer = (m_activeBuffer + 1) % 2;
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbo[otherBuffer]);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
	glUseProgram(m_drawShader);
	loc = glGetUniformLocation(m_drawShader, "projectionView");
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(_camera->GetProjectionView()));
	loc = glGetUniformLocation(m_drawShader, "cameraTransform");
	glUniformMatrix4fv(loc, 1, false, glm::value_ptr(_camera->GetTransform()));
	glBindVertexArray(m_vao[otherBuffer]);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);
	m_activeBuffer = otherBuffer;
}
void ParticleEmitter::CreateBuffers() {
	glGenVertexArrays(2, m_vao);
	glGenBuffers(2, m_vbo);
	glBindVertexArray(m_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(Particle), m_particles, GL_STREAM_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), ((char*)0) + 28);
	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(Particle), 0, GL_STREAM_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), ((char*)0) + 28);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void ParticleEmitter::CreateDrawShader() {
	unsigned int vs = ShaderLoader::LoadShader("res/shaders/gpuParticle.vs", GL_VERTEX_SHADER);
	unsigned int gs = ShaderLoader::LoadShader("res/shaders/gpuParticle.gs", GL_GEOMETRY_SHADER);
	unsigned int fs = ShaderLoader::LoadShader("res/shaders/gpuParticle.fs", GL_FRAGMENT_SHADER);
	m_drawShader = glCreateProgram();
	glAttachShader(m_drawShader, vs);
	glAttachShader(m_drawShader, gs);
	glAttachShader(m_drawShader, fs);
	glLinkProgram(m_drawShader);
	int success = GL_FALSE;
	glGetProgramiv(m_drawShader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(m_drawShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_drawShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s", infoLog);
		printf("\n");
		delete[] infoLog;
	}
	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);
	glUseProgram(m_drawShader);
	int loc = glGetUniformLocation(m_drawShader, "sizeStart");
	glUniform1f(loc, m_startSize);
	loc = glGetUniformLocation(m_drawShader, "sizeEnd");
	glUniform1f(loc, m_endSize);
	loc = glGetUniformLocation(m_drawShader, "colourStart");
	glUniform4fv(loc, 1, &m_startColour[0]);
	loc = glGetUniformLocation(m_drawShader, "colourEnd");
	glUniform4fv(loc, 1, &m_endColour[0]);
}
void ParticleEmitter::CreateUpdateShader() {
	unsigned int vs = ShaderLoader::LoadShader("res/shaders/gpuParticleUpdate.vs", GL_VERTEX_SHADER);
	m_updateShader = glCreateProgram();
	glAttachShader(m_updateShader, vs);
	const char* varyings[] = { "position", "velocity", "lifetime", "lifespan" };
	glTransformFeedbackVaryings(m_updateShader, 4, varyings, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(m_updateShader);
	int success = GL_FALSE;
	glGetProgramiv(m_updateShader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(m_updateShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_updateShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s", infoLog);
		printf("\n");
		delete[] infoLog;
	}
	glDeleteShader(vs);
	glUseProgram(m_updateShader);
	int loc = glGetUniformLocation(m_updateShader, "lifeMin");
	glUniform1f(loc, m_minLifeSpan);
	loc = glGetUniformLocation(m_updateShader, "lifeMax");
	glUniform1f(loc, m_maxLifeSpan);
	loc = glGetUniformLocation(m_updateShader, "velocityMin");
	glUniform1f(loc, m_minVelocity);
	loc = glGetUniformLocation(m_updateShader, "velocityMax");
	glUniform1f(loc, m_maxVelocity);
}
void ParticleEmitter::LoadTexture(const char* _path[2]) {
	for (int i = 0; i < 2; i++) {
		int textureWidth, textureHeight, textureFormat;
		unsigned char* imageData = stbi_load(_path[i], &textureWidth, &textureHeight, &textureFormat, STBI_default);
		glGenTextures(1, &m_texture[i]);
		glBindTexture(GL_TEXTURE_2D, m_texture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		stbi_image_free(imageData);
	}
}