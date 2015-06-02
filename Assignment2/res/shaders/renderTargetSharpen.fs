#version 410

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D diffuse;

vec4 Sharpen()
{
	vec2 texel = 1.0f / textureSize(diffuse, 0);

	vec4 colour = texture(diffuse, vTexCoord) * (11 / 3);
	colour += texture(diffuse, vTexCoord + vec2(0, texel.y)) * (-2 / 3);
	colour += texture(diffuse, vTexCoord - vec2(0, texel.y)) * (-2 / 3);
	colour += texture(diffuse, vTexCoord + vec2(texel.x, 0)) * (-3 / 3);
	colour += texture(diffuse, vTexCoord - vec2(texel.x, 0)) * (-3 / 3);

	return colour;
}

void main()
{
	FragColor = Sharpen();

	if(FragColor.a < 0.5f)
	{
		discard;
	}
}