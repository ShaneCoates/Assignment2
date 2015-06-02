#version 410

layout(location=0) in vec3 Position;
layout(location=1) in vec3 Velocity;
layout(location=2) in float Lifetime;
layout(location=3) in float Lifespan;

out vec3 position;
out vec3 velocity;
out float lifetime;
out float lifespan;

uniform float time;
uniform float deltaTime;
uniform float lifeMin;
uniform float lifeMax;
uniform float velocityMin;
uniform float velocityMax;

uniform vec3 emitterPosition;

uniform sampler3D perlin;

uniform bool emitting;

const float INVERSE_MAX_UINT = 1.0f / 4294967295.0f;

float rand(uint seed, float range)
{
	uint i = (seed ^ 12345391u) * 2654435769u;
	i ^= (i << 6u) ^ (i >> 26u);
	i *= 2654435769u;
	i += (i << 5u) ^ (i >> 12u);
	return float(range * i) * INVERSE_MAX_UINT;
}

void main()
{
	uint seed = uint(time * 1000.0) + uint(gl_VertexID);
	velocity = Velocity;
	position = Position + Velocity * deltaTime;	
	lifetime = Lifetime + deltaTime;
	lifespan = Lifespan;

	vec3 perlinPos = position;// * 0.1f;
	perlinPos.x = perlinPos.x * 0.5f + 0.5f;
	perlinPos.y = perlinPos.y * 0.5f + 0.5f;
	perlinPos.z = perlinPos.z * 0.5f + 0.5f;
	float height = texture(perlin, perlinPos).r;
	
	//if(height > 1f) {
	//	velocity = velocity * 0.9f;
	//} else {
	//	velocity = velocity * 1.01f;
	//}
	velocity *= height;

	if(emitting) {
		if(lifetime > lifespan)
		{
			velocity.x = rand(seed++, 1) - 0.5f;
			velocity.y = rand(seed++, 1) - 0.5f;
			velocity.z = rand(seed++, 1) - 0.5f;
			velocity = normalize(velocity);
			//vec3 perlinVel = velocity;
			//perlinVel.x = perlinVel.x * 0.5f + 0.5f;
			//perlinVel.y = perlinVel.y * 0.5f + 0.5f;
			//perlinVel.z = perlinVel.z * 0.5f + 0.5f;

			//velocity *= texture(perlin, perlinVel).r;
			velocity *= rand(seed++, velocityMax - velocityMin) + velocityMin;
			position = emitterPosition;

			lifetime = 0;
			lifespan = rand(seed++, lifeMax - lifeMin) + lifeMin;// - position.y - velocity.y;
		}
	}
}