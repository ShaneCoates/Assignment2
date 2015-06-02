/*
Author: Shane Coates
Description: Render Target class
For deffered rendering and post-processing
*/

#ifndef _RENDER_TARGET_H_
#define	_RENDER_TARGET_H_
class Camera;
class RenderTarget
{
public:

	RenderTarget(Camera* _camera);
	~RenderTarget();

	void DrawBegin();
	void DrawEnd();


protected:
private:
	void Init();

	unsigned int m_vao, m_vbo, m_ibo;
	unsigned int m_fbo;
	unsigned int m_fboTexture;
	unsigned int m_fboDepth;
	unsigned int m_program;

	Camera* m_camera;
};
#endif