/*
	Author: Shane Coates
	Description: Skybox
*/

#ifndef _SKYBOX_H_
#define	_SKYBOX_H_
#include <vector>
class Camera;
class Skybox {
public:
	Skybox();
	~Skybox();

	void Draw(Camera* _camera);
private:

	void CreateBuffers();
	void LoadCubemap(std::vector<const char*> _faces);

	unsigned int m_texture;
	unsigned int m_program;
	unsigned int m_vao;
	unsigned int m_vbo;
};

#endif