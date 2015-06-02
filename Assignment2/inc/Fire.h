/*
	Author: Shane Coates
	Description: Fire class
				 For deffered rendering and post-processing
*/

#ifndef _FIRE_H_
#define	_FIRE_H_
class Camera;
class ParticleEmitter;
class Fire
{
public:

	Fire();
	~Fire();

	void Draw(Camera* _camera);

	unsigned int m_fboTexture[2];


protected:
private:
	void Init();
	void CreateFrameBuffers();
	void CreateBufferObjects();
	void DrawToTexture(unsigned int _index);
	unsigned int m_vao, m_vbo, m_ibo;
	unsigned int m_fbo[2];
	unsigned int m_fboDepth;
	unsigned int m_program[2];

	ParticleEmitter* m_emitter;
	Camera* m_camera;
};

#endif