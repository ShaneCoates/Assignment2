uniform float time;
//uniform float speed;
//uniform float noise;
//uniform float sampledist;
//uniform float height;
//uniform float glowfalloff;
//uniform float glowstrength;
//uniform float ambientglow;
//uniform float ambientglowheightscale;
uniform sampler2D noisesampler;
varying vec2 vtexcoord;
void main()
{
    float speed = 0.25;
    float noise = 0.78;        // These variables should be replaced with uniform variables
    float sampledist = 0.0076; // that can be edited from the object's properties
    float height = 0.44;
    float glowfalloff = 0.024;
    float glowstrength = 144.0;
    float ambientglow = 0.5;
    float ambientglowheightscale = 1.68;
 
    vec4 color = vec4(0.965, 0.624, 1.0, 1.0);
    vec2 t = vec2(speed * time * .5871 - noise * abs(vtexcoord.y), speed * time);
    float xs0 = vtexcoord.x - sampledist;
    float xs1 = vtexcoord.x;
    float xs2 = vtexcoord.x + sampledist;
    float noise0 = texture2D(noisesampler, vec2(xs0 , t.y + (t.x * .3)) ).x;    // Change t.y + (t.x * .3) to t.y - etc. to see a sucking electricity
    float noise1 = texture2D(noisesampler, vec2(xs1 , t.y + (t.x * .3)) ).x;
    float noise2 = texture2D(noisesampler, vec2(xs2 , t.y + (t.x * .3)) ).x;
    float mid0 = height * (noise0 * 2.0 - 1.0) * (1.0 - xs0 * xs0);
    float mid1 = height * (noise1 * 2.0 - 1.0) * (1.0 - xs1 * xs1);
    float mid2 = height * (noise2 * 2.0 - 1.0) * (1.0 - xs2 * xs2);
    float dist0 = abs(vtexcoord.y - mid0);
    float dist1 = abs(vtexcoord.y - mid1);
    float dist2 = abs(vtexcoord.y - mid2);
 
    float glow = 1.0 - pow(0.25 * (dist0 + 2.0 * dist1 + dist2), glowfalloff);
 
    float amb = ambientglow * (1.0 - xs1 * xs1) * (1.0 - abs(ambientglowheightscale * vtexcoord.y) );
    //float amb = 0.0;
 
    vec4 final = (glowstrength * glow * glow + amb) * color;
    //final *= .75;
    gl_FragColor = final;
}
