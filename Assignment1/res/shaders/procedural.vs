#version 410

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal; 

out vec4 vPosition;
out vec3 vTexCoords;
out vec3 vNormal;

uniform mat4 ProjectionView;
uniform sampler3D perlinTexture;
uniform float time;
uniform float zValue;
uniform float amplitude;
uniform float size;
void main() {
	vec2 tex = TexCoords;
	//tex.x *= 2;
	//if(tex.x > 1) tex.x = 2 - tex.x;
	//tex.y *= 2;
	//if(tex.y > 1) tex.y = 2 - tex.y;

	vTexCoords = vec3(tex, zValue * 0.5f + 0.5f);
	tex.x += 0.5f;
	vec3 texcoord = vec3(tex, cos(time) * 0.5f + 0.5f);

	float height = texture(perlinTexture, vTexCoords).r - (0.5f + amplitude);
	height += texture(perlinTexture, texcoord).r - (0.5f + amplitude);
	height *= 0.5f;

	//if(height < 0.5f) {
	//	height = 0.5f;
	//}
	vec4 pos = Position;
	pos.x *= size;
	pos.z *= size;
	pos.y *= size;
	//pos.x += pos.x * (height - 0.5f) * amplitude;
	//pos.y += pos.y * (height - 0.5f) * amplitude;
	//pos.y += pos.y * (height - 0.5f) * amplitude;

	//pos.y -= amplitude * size;
	vPosition = pos;
	gl_Position = ProjectionView * vPosition;
	vNormal = Normal;
}