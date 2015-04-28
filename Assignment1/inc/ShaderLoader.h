/*
	Author: Shane Coates
	Description: Simple class to load shaders from file
*/

#ifndef _SHADER_LOADER_H_
#define	_SHADER_LOADER_H_
#include <string>
class ShaderLoader
{
public:
	static unsigned int LoadProgram(const char* _szVertexShader, const char* _szPixelShader, const char* _szGeometryShader = nullptr, const char* _szTessellationControlShader = nullptr, const char* a_szTessellationEvaluationShader = nullptr);
	static unsigned int LoadShader(const char* _strName, unsigned int _uiType);
	static unsigned int CreateProgram(unsigned int vs, unsigned int tcs, unsigned int tes, unsigned int gs, unsigned int fs);
	static unsigned char* FileToBuffer(const char* _strName);

};

#endif