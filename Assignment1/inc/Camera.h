/*
	Author: Shane Coates
	Description: Base Camera class
				 To be inherited by specific types of cameras
*/

#ifndef _CAMERA_H_
#define	_CAMERA_H_

#include <glm.hpp>
#include <ext.hpp>

class Camera
{
public:
	//Constructors and overloads
	Camera();
	Camera(glm::vec3 _position);
	Camera(glm::mat4 _transform);
	
	//Virtual Update function to be overridden
	virtual void Update(double _deltaTime) = 0;

	//Set and Get Transform
	void SetTransform(glm::mat4 _transform);
	const glm::mat4 GetTransform() const	{ return m_worldTransform; }

	//Set and Get position
	void SetPosition(glm::vec3 _position);
	glm::vec3 GetPosition() const			{ return m_worldTransform[3].xyz(); }

	//Set Look At - 2 options:
	//Using current position, or
	void SetLookAt(glm::vec3 _to, glm::vec3 _up);
	//Using new position
	void SetLookAt(glm::vec3 _from, glm::vec3 _to, glm::vec3 _up);

	//Setup perspective
	void SetPerspective(float _fieldOfView, float _aspectRation, float _near, float _far);

	//Get Transform matrices
	const glm::mat4& GetProjection()		const { return m_projectionTransform; }
	const glm::mat4& GetView()				const { return m_viewTransform; }
	const glm::mat4& GetProjectionView()	const { return m_projectionViewTransform; }

	//Translate
	void Translate(glm::vec3 _translation);	

	void GetFrustumPlanes(glm::vec4* _planes);

protected:

	glm::mat4 m_worldTransform;
	void UpdateProjectionViewTransform();

private:


	glm::mat4 m_viewTransform;
	glm::mat4 m_projectionTransform;
	glm::mat4 m_projectionViewTransform;
};

#endif