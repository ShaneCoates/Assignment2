varying vec2 vtexcoord;
void main()
{
    vec2 position;
    position.xy = sign(gl_Vertex.xy);
    vtexcoord = position.xy;
    //gl_Position = ftransform();
    gl_Position = vec4(position.xy, 0.0, 1.0);    // Screen aligned, if you want that
}
