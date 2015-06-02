/*
	Author: Shane Coates
	Description: GPU Particle Emitter class
*/
#ifndef _PARTICLE_EMITTER_H_
#define _PARTICLE_EMITTER_H_
#include <glm.hpp>
class Camera;
struct Particle
{
	Particle() : lifetime(1), lifespan(0) {}

	glm::vec3 position;
	glm::vec3 velocity;
	float lifetime;
	float lifespan;
};

class ParticleEmitter
{
public:

	ParticleEmitter();
	virtual ~ParticleEmitter();

	void Init(unsigned int _maxParticles, 
				float _minLifeSpan, float _maxLifeSpan, 
				float _minVelocity, float _maxVelocity, 
				float _startSize, float _endSize, 
				const glm::vec4& _startColour, const glm::vec4& _endColour);

	void Draw(float _time, Camera* _camera);

	void LoadTexture(const char* _path[2]);
	void CreateUpdateShader();
	void CreateDrawShader();
protected:

	void CreateBuffers();

	unsigned int m_texture[2];

	Particle* m_particles;

	unsigned int m_maxParticles;

	glm::vec3 m_position;

	float m_minLifeSpan;
	float m_maxLifeSpan;

	float m_minVelocity;
	float m_maxVelocity;

	float m_startSize;
	float m_endSize;

	glm::vec4 m_startColour;
	glm::vec4 m_endColour;

	unsigned int m_activeBuffer;
	unsigned int m_vao[2];
	unsigned int m_vbo[2];

	unsigned int m_drawShader;
	unsigned int m_updateShader;

	float m_lastDrawTime;

private:
};

#endif