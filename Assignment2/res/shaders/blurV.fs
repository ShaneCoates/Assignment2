#version 410

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D diffuse;

const float blurSize = 1.0/1024.0;

void main()
{

	vec4 sum = vec4(0.0);

	sum += texture2D(diffuse, vec2(vTexCoord.x, vTexCoord.y - 4.0*blurSize)) * 0.05;
	sum += texture2D(diffuse, vec2(vTexCoord.x, vTexCoord.y - 3.0*blurSize)) * 0.09;
	sum += texture2D(diffuse, vec2(vTexCoord.x, vTexCoord.y - 2.0*blurSize)) * 0.12;
	sum += texture2D(diffuse, vec2(vTexCoord.x, vTexCoord.y - blurSize)) * 0.15;
	sum += texture2D(diffuse, vec2(vTexCoord.x, vTexCoord.y)) * 0.16;
	sum += texture2D(diffuse, vec2(vTexCoord.x, vTexCoord.y + blurSize)) * 0.15;
	sum += texture2D(diffuse, vec2(vTexCoord.x, vTexCoord.y + 2.0*blurSize)) * 0.12;
	sum += texture2D(diffuse, vec2(vTexCoord.x, vTexCoord.y + 3.0*blurSize)) * 0.09;
	sum += texture2D(diffuse, vec2(vTexCoord.x, vTexCoord.y + 4.0*blurSize)) * 0.05;
	FragColor = sum;
}