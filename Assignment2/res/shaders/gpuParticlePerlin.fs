#version 410

in vec4 Colour;
in vec2 TexCoords;
in float Time;

out vec4 fragColour;


void main()
{
	fragColour = Colour;
}