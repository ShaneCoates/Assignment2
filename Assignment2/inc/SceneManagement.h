/*
	Author: Shane Coates
	Description: Scene Management
				 Has Bounding Box and Bounding Sphere
*/

#ifndef _SCENE_MANAGEMENT_H_
#define	_SCENE_MANAGEMENT_H_
#include <vector>
#include <glm.hpp>

class BoundingBox {
public:
	BoundingBox() { Reset(); }
	~BoundingBox() {}

	void Reset() {
		min.x = min.y = min.z = 1e37f;
		max.x = max.y = max.z = 1e37f;
	}
	void Fit(const std::vector<glm::vec3>& _points) {
		for (auto& p : _points) {
			if (p.x < min.x) min.x = p.x;
			if (p.y < min.y) min.y = p.y;
			if (p.z < min.z) min.z = p.z;
			if (p.x > max.x) max.x = p.x;
			if (p.y > max.y) max.y = p.y;
			if (p.z > max.z) max.z = p.z;
		}
	}

	glm::vec3 min, max;
};

class BoundingSphere {
public:
	BoundingSphere() : centre(0), radius(0) { }
	~BoundingSphere() {}

	void Fit(const std::vector<glm::vec3>& _points) {
		glm::vec3 min(1e37f), max(-1e37f);
		for (auto& p : _points) {
			if (p.x < min.x) min.x = p.x;
			if (p.y < min.y) min.y = p.y;
			if (p.z < min.z) min.z = p.z;
			if (p.x > max.x) max.x = p.x;
			if (p.y > max.y) max.y = p.y;
			if (p.z > max.z) max.z = p.z;
		}
		centre = (min + max) * 0.5f;
		radius = glm::distance(min, centre);
	}

	glm::vec3 centre;
	float radius;
};
#endif