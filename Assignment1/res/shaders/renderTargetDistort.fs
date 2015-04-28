#version 410

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D diffuse;

vec4 Distort()
{
	vec2 mid = vec2(0.5f);

	float distanceFromCentre = distance(vTexCoord, mid);
	vec2 normalizedCoord = normalize(vTexCoord - mid);
	float bias = distanceFromCentre + sin(distanceFromCentre * 15) * 0.01f;

	vec2 newCoord = mid + bias * normalizedCoord;
	return texture(diffuse, newCoord);
}

void main()
{
	FragColor = Distort();

	if(FragColor.a < 0.5f)
	{
		discard;
	}
}