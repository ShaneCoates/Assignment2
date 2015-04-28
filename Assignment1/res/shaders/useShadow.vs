#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec4 Normal;
layout(location=2) in vec4 tangent;
layout(location=3) in vec2 tex_coord;
layout(location=4) in vec4 weights;
layout(location=5) in vec4 indices;

out vec4 vNormal;
out vec4 vShadowCoord;

uniform mat4 ProjectionView;

uniform mat4 LightMatrix;

void main() {
	vNormal = Normal;
	vec4 pos = Position;
	pos.x *= 0.01f;
	pos.y *= 0.01f;
	pos.z *= 0.01f;

	gl_Position = ProjectionView * pos;

	vShadowCoord = LightMatrix * Position;
}