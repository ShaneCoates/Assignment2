#version 410
layout(location=0) in vec4 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in vec2 TexCoord;

uniform mat4 projection, modelView;

out vec3 normalInterp;
out vec3 vertPos;
out vec2 vTexCoord; 

void main()
{
	vec4 MVP = modelView * Position;
	gl_Position = projection * MVP;
	vertPos = vec3(MVP) / MVP.w;
	normalInterp = Normal; 
	vTexCoord = TexCoord; 
};