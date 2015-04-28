#include "ShaderLoader.h"
#include <gl_core_4_4.h>
unsigned int ShaderLoader::LoadProgram(const char* a_szVertexShader, const char* a_szPixelShader, const char* a_szGeometryShader /*= nullptr*/, const char* a_szTessellationControlShader /*= nullptr*/, const char* a_szTessellationEvaluationShader /*= nullptr*/) {
	GLuint vs = LoadShader(a_szVertexShader, GL_VERTEX_SHADER);
	GLuint fs = LoadShader(a_szPixelShader, GL_FRAGMENT_SHADER);
	GLuint gs = 0;
	GLuint tcs = 0;
	GLuint tes = 0;
	if (a_szGeometryShader != nullptr) {
		gs = LoadShader(a_szGeometryShader, GL_GEOMETRY_SHADER);
	}
	if (a_szTessellationControlShader && a_szTessellationEvaluationShader) {
		tcs = LoadShader(a_szTessellationControlShader, GL_TESS_CONTROL_SHADER);
		tes = LoadShader(a_szTessellationEvaluationShader, GL_TESS_EVALUATION_SHADER);
	}
	unsigned int uiShaderID = CreateProgram(vs, tcs, tes, gs, fs);
	return uiShaderID;
}
unsigned int ShaderLoader::LoadShader(const char* _strName, unsigned int _uiType) {
	int success = GL_FALSE;
	unsigned int handle = glCreateShader(_uiType);
	unsigned char* source = FileToBuffer(_strName);
	glShaderSource(handle, 1, (const char**)&source, 0);
	glCompileShader(handle);
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetShaderInfoLog(handle, infoLogLength, 0, infoLog);
		printf("Error: Failed to compile shader!\n");
		printf("%s", infoLog);
		printf("\n");
		delete[] infoLog;
		return 0;
	}
	delete[] source;
	return handle;
}
unsigned int ShaderLoader::CreateProgram(GLuint vs, GLuint tcs, GLuint tes, GLuint gs, GLuint fs) {
	int success = GL_FALSE;
	unsigned int handle = glCreateProgram();
	glAttachShader(handle, vs);
	glAttachShader(handle, tcs);
	glAttachShader(handle, tes);
	glAttachShader(handle, gs);
	glAttachShader(handle, fs);
	glLinkProgram(handle);
	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(handle, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s", infoLog);
		printf("\n");
		delete[] infoLog;
		return 0;
	}
	return handle;
}
unsigned char* ShaderLoader::FileToBuffer(const char* a_strName) {
	FILE* pFile = fopen(a_strName, "rb");
	if (pFile == nullptr) {
		printf("Error: Unable to open file '%s' for reading!\n", a_strName);
		return nullptr;
	}
	fseek(pFile, 0, SEEK_END);
	unsigned int uiLength = (unsigned int)ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	unsigned char* acBuffer = new unsigned char[uiLength + 1];
	memset(acBuffer, 0, uiLength + 1);
	fread(acBuffer, sizeof(unsigned char), uiLength, pFile);
	fclose(pFile);
	return acBuffer;
}