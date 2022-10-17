#include "scene.hpp"


using namespace cgp;


void scene_structure::evolve_shape()
{	
    size_t const N = initial_position.size();

	for(float k=0.0f; k<4545.0f; ++k)
    {
		float persistency = 2.0f + 0.4f * (100.0f - (k * 100.0f / 4545.0f));
        vec3 const& p0 = initial_position[k];
        vec3& p        = shape.position[k];
		float const dz = 0.3f * noise_perlin({ p0.x + 0.2f * timer.t, p0.y, 0 }, 2, persistency, 0.3f) 
			+ 0.015f * noise_perlin({ 4 * p0.x, 4 * p0.y, timer.t }, 2);
        p.z = dz;
    }
	for(float k=5662; k<N; ++k)
    {
		float persistency = 2.0f + 0.4f * (((k - 5662) * 100.0f / 4539.0f));
        vec3 const& p0 = initial_position[k];
        vec3& p        = shape.position[k];
		float const dz = 0.3f * noise_perlin({ p0.x + 0.2f * timer.t, p0.y, 0 }, 2, persistency, 0.3f) 
			+ 0.015f * noise_perlin({ 4 * p0.x, 4 * p0.y, timer.t }, 2);
        p.z = dz;
    }
}

void scene_structure::evolve_sun()
{
    size_t const N = sun_position.size();
    float x_pos = camera_control.camera_model.position().x;

	for(int k=0; k<N; ++k)
    {
        sun.position[k].x = sun_position[k].x + x_pos;
    } 
}

void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 0.0f, -0.5f, 1.0f }, {0,0,1}, {0,0,1});

    environment.background_color = {0, 0, 0};

	int N = 101;
	shape = mesh_primitive_grid({ -50,-50,0 }, { 50,-50,0 }, { 50,50,0 }, { -50,50,0 }, N, N);
    sun = mesh_primitive_disc(10, { 0,100,10 }, { 0,-1,0 }, 24);

	initial_position = shape.position;
    sun_position = sun.position;

    opengl_shader_structure shader;
    opengl_shader_structure single_color;

    shader.load("shaders/mesh/vert.glsl", "shaders/mesh/frag.glsl");
    single_color.load("shaders/single_color/vert.glsl", "shaders/single_color/frag.glsl");

	shape_visual.initialize_data_on_gpu(shape, shader);
	shape_visual.material.color = { 0.2f, 0.4f, 1.0f };

	sun_visual.initialize_data_on_gpu(sun, single_color);
	sun_visual.material.color = { 1.0f, 0.7f, 0.0f };

	global_frame.initialize_data_on_gpu(mesh_primitive_frame());
}



void scene_structure::display_frame()
{

	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	
	if (gui.display_frame)
		draw(global_frame, environment);

	timer.update();


	
	draw(shape_visual, environment);
    draw(sun_visual, environment);
	if (gui.display_wireframe) {
    	draw_wireframe(shape_visual, environment, { 0,0,255 });
        draw_wireframe(sun_visual, environment, { 0,0,255 });
    }

	evolve_shape();
    evolve_sun();

	shape_visual.vbo_position.update(shape.position);
    sun_visual.vbo_position.update(sun.position);
	// Recompute normals on the CPU (given the position and the connectivity currently in the mesh structure)
	shape.normal_update();
    sun.normal_update();
	// Send updated normals on the GPU
	shape_visual.vbo_normal.update(shape.normal);
    sun_visual.vbo_normal.update(sun.normal);
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

