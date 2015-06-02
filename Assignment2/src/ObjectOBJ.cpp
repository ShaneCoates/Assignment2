#include "ObjectOBJ.h"
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Camera.h"
#include <stb_image.h>

ObjectOBJ::ObjectOBJ(unsigned int _programID)
{
	Init(_programID);
}

ObjectOBJ::ObjectOBJ(const char* _path, unsigned int _programID)
{
	Init(_programID);
	LoadFromObj(_path);
}

void ObjectOBJ::Init(unsigned int _programID)
{
	m_indexCount = 0;
	m_vertexCount = 0;
	m_programID = _programID;
	m_MVP = glm::mat4(1);
}

ObjectOBJ::~ObjectOBJ()
{

}

void ObjectOBJ::Update(double dt)
{

}

void ObjectOBJ::Draw(Camera* _camera)
{
	glUseProgram(m_programID);

	unsigned int projectionUniform = glGetUniformLocation(m_programID, "projection");
	unsigned int modelUniform = glGetUniformLocation(m_programID, "modelView");
	unsigned int diffuse = glGetUniformLocation(m_programID, "diffuse");
	unsigned int ambientColorUniform = glGetUniformLocation(m_programID, "ambientColor");
	unsigned int diffuseColorUniform = glGetUniformLocation(m_programID, "diffuseColor");
	unsigned int specColorUniform = glGetUniformLocation(m_programID, "specColor");
	unsigned int lightPosUniform = glGetUniformLocation(m_programID, "lightPos");

	glUniformMatrix4fv(projectionUniform, 1, false, glm::value_ptr(_camera->GetProjectionView()));
	glUniformMatrix4fv(modelUniform, 1, false, glm::value_ptr(m_MVP));
	glUniform1i(diffuse, 0);
	glUniform3fv(ambientColorUniform, 1, glm::value_ptr(ambientColor));
	glUniform3fv(diffuseColorUniform, 1, glm::value_ptr(diffuseColor));
	glUniform3fv(specColorUniform, 1, glm::value_ptr(specColor));

	glBindTexture(GL_TEXTURE_2D, m_texture);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
}

void ObjectOBJ::LoadFromObj(const char* _path)
{
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4) + (sizeof(glm::vec3))));

	std::vector<Vertex> aoVerticies;
	std::vector<unsigned int> auiIndicies;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	std::string mtlPath;

	std::ifstream in(_path, std::ios::in);
	if (!in) { std::cerr << "Cannot open " << _path << std::endl; exit(1); }

	std::string line;
	while (getline(in, line))
	{
		if (line.substr(0, 2) == "v ")
		{
			std::istringstream s(line.substr(2));
			Vertex v;
			s >> v.position.x;
			s >> v.position.y;
			s >> v.position.z;
			v.position.w = 1.0f;

			aoVerticies.push_back(v);
			m_vertexCount++;
		}
		else if (line.substr(0, 2) == "f ")
		{
			char dummyChar;
			std::istringstream s(line.substr(2));
			GLushort a, b, c;
			GLushort aTex, bTex, cTex;
			GLushort aNorm, bNorm, cNorm;

			s >> a; s >> dummyChar; s >> aTex; s >> dummyChar; s >> aNorm;
			s >> b; s >> dummyChar; s >> bTex; s >> dummyChar; s >> bNorm;
			s >> c; s >> dummyChar; s >> cTex; s >> dummyChar; s >> cNorm;
			a--; b--; c--;
			aTex--; bTex--; cTex--;
			aNorm--; bNorm--; cNorm--;
			auiIndicies.push_back(a); auiIndicies.push_back(b); auiIndicies.push_back(c);

			aoVerticies[a].texCoord = texCoords[aTex];
			aoVerticies[b].texCoord = texCoords[bTex];
			aoVerticies[c].texCoord = texCoords[cTex];

			aoVerticies[a].normal = normals[aNorm];
			aoVerticies[b].normal = normals[bNorm];
			aoVerticies[c].normal = normals[cNorm];

			m_indexCount += 3;
		}
		else if (line.substr(0, 3) == "vn ")
		{
			std::istringstream s(line.substr(3));
			glm::vec3 norm;
			s >> norm.x; s >> norm.y;
			normals.push_back(norm);
		}
		else if (line.substr(0, 3) == "vt ")
		{
			std::istringstream s(line.substr(3));
			glm::vec2 coords;
			s >> coords.x; s >> coords.y;
			coords.y *= -1.0f;
			texCoords.push_back(coords);
		}
		else if (line.substr(0, 7) == "mtllib ")
		{
			std::istringstream s(line.substr(7));
			s >> mtlPath;
			mtlPath = "res/models/Dragon/" + mtlPath;
		}
		else if (line[0] == '#') { /* ignoring this line */ }
		else { /* ignoring this line */ }
	}

	glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(Vertex), &aoVerticies[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), &auiIndicies[0], GL_STATIC_DRAW);
	LoadTexture(mtlPath.c_str());
}

void ObjectOBJ::LoadTexture(const char* _path)
{
	std::ifstream in(_path, std::ios::in);
	if (!in) { std::cerr << "Cannot open " << _path << std::endl; exit(1); }

	std::string line;
	GLfloat x, y, z;
	std::string path;
	while (getline(in, line))
	{
		if (line.substr(0, 3) == "Kd ")
		{
			std::istringstream s(line.substr(3));
			s >> x; s >> y; s >> z;
			diffuseColor = glm::vec3(x, y, z);
		}
		else if (line.substr(0, 3) == "Ka ")
		{
			std::istringstream s(line.substr(3));
			s >> x; s >> y; s >> z;
			ambientColor = glm::vec3(x, y, z);
		}
		else if (line.substr(0, 3) == "Ks ")
		{
			std::istringstream s(line.substr(3));
			s >> x; s >> y; s >> z;
			specColor = glm::vec3(x, y, z);
		}
		else if (line.substr(0, 7) == "map_Kd ")
		{
			std::istringstream s(line.substr(7));
			s >> path;
		}
		else if (line[0] == '#') { /* ignoring this line */ }
		else { /* ignoring this line */ }
	}

	if (path.length() > 1)
	{
		std::string data = "data/" + path;

		unsigned char* imageData = stbi_load(data.c_str(), &m_textureWidth, &m_textureHeight, &m_textureFormat, STBI_default);

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_textureWidth, m_textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		stbi_image_free(imageData);
	}
}

void ObjectOBJ::Translate(glm::vec3 _translation)
{
	m_MVP *= glm::translate(_translation);
}

void ObjectOBJ::SetScale(glm::vec3 _scale)
{
	m_MVP *= glm::scale(_scale);
	m_scale = _scale;
}

glm::vec3 ObjectOBJ::GetPosition()
{
	return m_MVP[3].xyz();
}
