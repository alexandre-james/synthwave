#version 330 core

// Inputs coming from VBOs
layout (location = 0) in vec3 vertex_position; // vertex position in local space (x,y,z)
layout (location = 1) in vec3 vertex_normal;   // vertex normal in local space   (nx,ny,nz)
layout (location = 2) in vec3 vertex_color;    // vertex color      (r,g,b)
layout (location = 3) in vec2 vertex_uv;       // vertex uv-texture (u,v)

// Output variables sent to the fragment shader
out struct fragment_data
{
    vec3 position; // vertex position in world space
    vec3 normal;   // normal position in world space
    vec3 color;    // vertex color
    vec2 uv;       // vertex uv
} fragment;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Fill the parameters sent to the fragment shader
	fragment.position = vertex_position;
	fragment.normal = vertex_normal;
	fragment.color = vertex_color;
	fragment.uv = vertex_uv;

	gl_Position = projection * view * model * vec4(vertex_position, 1.0);
}
