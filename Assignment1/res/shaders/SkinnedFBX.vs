#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec4 tangent;
layout(location=3) in vec2 tex_coord;
layout(location=4) in vec4 weights;
layout(location=5) in vec4 indices;

out vec3 frag_normal;
out vec3 frag_position;
out vec3 frag_tangent;
out vec3 frag_bitangent;
out vec2 frag_texcoord;

uniform mat4 ProjectionView;
uniform mat4 global;

// we need to give our bone array a limit
const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];
void main()
{
	frag_position = position.xyz;
	frag_normal = normal.xyz;
	frag_tangent = tangent.xyz;
	frag_bitangent = cross(normal.xyz, tangent.xyz);
	frag_texcoord = tex_coord;
	
	// cast the indices to integer's so they can index an array
	ivec4 index = ivec4(indices);
	
	// sample bones and blend up to 4
	vec4 P = bones[ index.x ] * position * weights.x;
	P += bones[ index.y ] * position * weights.y;
	P += bones[ index.z ] * position * weights.z;
	P += bones[ index.w ] * position * weights.w;
	P.w = 1;
	gl_Position = ProjectionView * global * position;
}

