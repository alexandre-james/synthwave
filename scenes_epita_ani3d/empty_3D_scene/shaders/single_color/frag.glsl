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

// Coefficients of phong illumination model
struct phong_structure {
	float ambient;      
	float diffuse;
	float specular;
	float specular_exponent;
};

// Settings for texture display
struct texture_settings_structure {
	bool use_texture;       // Switch the use of texture on/off
	bool texture_inverse_v; // Reverse the texture in the v component (1-v)
	bool two_sided;         // Display a two-sided illuminated surface (doesn't work on Mac)
};

// Material of the mesh (using a Phong model)
struct material_structure
{
	vec3 color;  // Uniform color of the object
	float alpha; // alpha coefficient

	phong_structure phong;                       // Phong coefficients
	texture_settings_structure texture_settings; // Additional settings for the texture
}; 

uniform material_structure material;

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

	FragColor = vec4(material.color, 1.0);
}
