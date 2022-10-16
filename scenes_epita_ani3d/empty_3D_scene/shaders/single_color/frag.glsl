#version 330 core

// Inputs coming from the vertex shader
in struct fragment_data
{
    vec3 position; // position in the world space
    vec3 normal;   // normal in the world space
    vec3 color;    // current color on the fragment
    vec2 uv;       // current uv-texture on the fragment

} fragment;

// Output of the fragment shader - output color
layout(location=0) out vec4 FragColor;

uniform vec3 light = vec3(1.0, 1.0, 1.0); // position of the light

void main()
{
    // Renormalize normal
	vec3 N = normalize(fragment.normal);
	// Unit direction toward the light
	vec3 L = normalize(light-fragment.color);
	// Diffuse coefficient
	float diffuse_component = max(dot(N,L),0.0);

    vec3 color_shading = diffuse_component * vec3(0.5, 1, 1);

	FragColor = vec4(color_shading, 1.0);
}
