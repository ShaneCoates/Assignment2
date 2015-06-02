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

	position = Position + Velocity * deltaTime;	
	velocity = Velocity;
	lifetime = Lifetime + deltaTime;
	lifespan = Lifespan;

	if(lifetime > lifespan)
	{
		velocity.x = rand(seed++, 1) - 0.5f;
		velocity.y = rand(seed++, 1) - 0.5f;
		velocity.z = rand(seed++, 1) - 0.5f;

		position = emitterPosition;

		position.x += rand(seed++, 400) - 200;
		position.z += rand(seed++, 400) - 200;
		position.y += rand(seed++, 400) - 200;

		lifetime = 0;
		lifespan = rand(seed++, lifeMax - lifeMin) + lifeMin;// - position.y - velocity.y;
	}
}