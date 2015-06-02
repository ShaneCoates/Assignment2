#version 410
precision mediump float;

in vec3 frag_normal;
in vec3 frag_position;
in vec3 frag_tangent;
in vec3 frag_bitangent;
in vec2 frag_texcoord;

uniform sampler2D diffuse; 
uniform sampler2D normal;
//uniform vec3 lightPos;
//uniform vec3 ambientColor;
//uniform vec3 diffuseColor; 
//uniform vec3 specColor;
uniform float time;
out vec4 FragColor;

void main()
{
	mat3 TBN = mat3(normalize(frag_tangent), normalize(frag_bitangent), normalize(frag_normal));
	vec3 N = texture(normal, frag_texcoord).xyz * 2 - 1;
	float d = max(0, dot(normalize(TBN * N), normalize(vec3(sin(time) * 100, 100, cos(time) * 100))));
	//vec3 normal = normalize(frag_normal);
	//vec3 lightDir = normalize(lightPos - frag_position);
	//float lambertian = max(dot(normal, lightDir), 0.0);
	//float specular = 0.0;
	//if(lambertian > 0.0) 
	//{
	//	vec3 viewDir = normalize(-vertPos);
	//	vec3 halfDir = normalize(lightDir + viewDir);
	//	float specAngle = max(dot(halfDir, normal), 0.0);
	//	specular = pow(specAngle, 128.0);
	//}
	FragColor = texture(diffuse, frag_texcoord);
	FragColor.rbg = FragColor.rbg * d;// * vec4(ambientColor + lambertian * diffuseColor + specular * specColor, 1.0);
};