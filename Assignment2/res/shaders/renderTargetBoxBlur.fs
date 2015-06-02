#version 410

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D diffuse;

vec4 BoxBlur()
{
	vec2 texel = 1.0f / textureSize(diffuse, 0).xy;

	vec4 colour = texture(diffuse, vTexCoord);
	colour += texture(diffuse, vTexCoord + vec2(-texel.x, texel.y));
	colour += texture(diffuse, vTexCoord + vec2(-texel.x, 0));
	colour += texture(diffuse, vTexCoord + vec2(-texel.x, -texel.y));
	colour += texture(diffuse, vTexCoord + vec2(0, texel.y));
	colour += texture(diffuse, vTexCoord + vec2(0, -texel.y));
	colour += texture(diffuse, vTexCoord + vec2(texel.x, texel.y));
	colour += texture(diffuse, vTexCoord + vec2(texel.x, 0));
	colour += texture(diffuse, vTexCoord + vec2(texel.x, -texel.y));

	return colour / 9;
}


void main()
{
	FragColor = BoxBlur();

	if(FragColor.a < 0.5f)
	{
		discard;
	}
}