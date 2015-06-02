#version 410
precision mediump float;

in vec3 normalInterp;
in vec3 vertPos;
in vec2 vTexCoord; 

uniform sampler2D diffuse; 
uniform vec3 lightPos;
uniform vec3 ambientColor;
uniform vec3 diffuseColor; 
uniform vec3 specColor;

out vec4 FragColor;

void main()
{
	vec3 normal = normalize(normalInterp);
	vec3 lightDir = normalize(lightPos - vertPos);
	float lambertian = max(dot(normal, lightDir), 0.0);
	float specular = 0.0;
	if(lambertian > 0.0) 
	{
		vec3 viewDir = normalize(-vertPos);
		vec3 halfDir = normalize(lightDir + viewDir);
		float specAngle = max(dot(halfDir, normal), 0.0);
		specular = pow(specAngle, 128.0);
	}
	FragColor = texture(diffuse, vTexCoord) * vec4(ambientColor + lambertian * diffuseColor + specular * specColor, 1.0);
};