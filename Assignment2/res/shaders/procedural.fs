#version 410

in vec4 vPosition;
in vec3 vTexCoords;
in vec3 vNormal;

out vec4 FragColour;

uniform sampler3D perlinTexture;
uniform sampler2D terrainTexture[3];
uniform float time;
uniform vec3 CameraPos;
uniform vec3 LightDir;
uniform float amplitude;
void main() {
	float height = texture(perlinTexture, vTexCoords).r - (0.5f * amplitude);
	height += texture(perlinTexture, vec3(vTexCoords.x, vTexCoords.y, sin(time * 0.1f))).r - (0.5f * amplitude);
	height *= 0.5f;
	vec2 texcoords = vec2(vTexCoords.x * 40, vTexCoords.y * 40);

	vec4 levelOne = texture(terrainTexture[0], texcoords);
	vec4 levelTwo = texture(terrainTexture[1], texcoords);
	vec4 levelThree = texture(terrainTexture[2], texcoords);
	if(height < 0.4f) {
		FragColour = levelOne;
	} 
	else if(height >= 0.4f && height < 0.5f) {
		FragColour = mix(levelOne, levelTwo, (height - 0.4f) * 10);
	}
	else if(height >= 0.5f && height < 0.6f) {
		FragColour = levelTwo;
	} 
	else if(height >= 0.6f && height < 0.7f) {
		FragColour = mix(levelTwo, levelThree, (height - 0.6f) * 10);
	}
	else{
		FragColour = levelThree;
	} 
	if(height < 1) {
		FragColour *= height;
	}
	FragColour.a = 1;
	float d = max(0, dot(normalize(vNormal.xyz), LightDir));
	vec3 E = normalize(CameraPos - vPosition.xyz);
	vec3 R = reflect(-LightDir, vNormal.xyz);
	float s = max(0, dot(E, R));
	s = pow(s, height * 8);


	FragColour = FragColour * vec4(vec3(1, 1, 1) * d + vec3(1, 1, 1) * s,  1);
}